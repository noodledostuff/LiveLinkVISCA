#include "ViscaLiveLinkSource.h"

#include "Async/Async.h"
#include "Common/TcpSocketBuilder.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/RunnableThread.h"
#include "ILiveLinkClient.h"
#include "LiveLinkSourceSettings.h"
#include "LiveLinkTypes.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "SocketTypes.h"
#include "Net/Common/Sockets/SocketErrors.h"
#include "Misc/App.h"
#include "Misc/QualifiedFrameTime.h"
#include "Misc/ScopeLock.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformTime.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "ViscaLiveLinkSource"

namespace
{
	constexpr int32 MaxUdpPacketSize = 65507;
	/** Scene time for Live Link frames (Timecode / display paths); world evaluation still uses WorldTime. */
	static const FFrameRate ViscaSceneFrameRate(60, 1);
	constexpr float StandardStep = 0.02f;
	constexpr float VariableStepScale = 0.005f;
	/** Lerp(-30°, 90°, x) == 0° when x == 0.25. */
	constexpr float DefaultTiltNormalizedForLevel = 0.25f;
	/** Pan/tilt normalized position change per second at VISCA speed 1.0. */
	constexpr float PanTiltDriveUnitsPerSec = 0.65f;
	/** Standard zoom tele/wide sustained rate (normalized zoom / sec). */
	constexpr float ZoomStandardVelocity = 0.42f;
	/** Variable zoom: scales (speedNibble+1)/8 to normalized zoom / sec. */
	constexpr float ZoomVariableVelocityScale = 0.55f;
	/** High-res zoom max magnitude (normalized zoom / sec). */
	constexpr float ZoomHighResVelocityScale = 0.65f;

	int32 ParsePortString(const FString& PortString)
	{
		return FMath::Clamp(FCString::Atoi(*PortString.TrimStartAndEnd()), 1, 65535);
	}
}

FViscaLiveLinkSource::FViscaLiveLinkSource(const FViscaLiveLinkConnectionSettings& InSettings)
	: Settings(InSettings)
{
	SourceType = LOCTEXT("SourceType", "VISCA LiveLink");
	SourceMachineName = FText::FromString(Settings.ListenInterfaceIp);

	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	RebuildSockets();

	Start();
}

FViscaLiveLinkSource::~FViscaLiveLinkSource()
{
	PerformShutdownOnce();
}

void FViscaLiveLinkSource::Start()
{
	bIsRunning = true;
	const FString ThreadName = FString::Printf(TEXT("VISCAReceiver_%d"), FAsyncThreadIndex::GetNext());
	Thread = FRunnableThread::Create(this, *ThreadName, 0, TPri_AboveNormal);
}

void FViscaLiveLinkSource::RestartWithSettings(const FViscaLiveLinkConnectionSettings& InSettings)
{
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}

	ClearSubjects();
	Settings = InSettings;
	SourceMachineName = FText::FromString(Settings.ListenInterfaceIp);
	LastPtzIntegrateTimeSecs = 0.0;
	RebuildSockets();
	bShuttingDown = false;
	Start();
	PublishConfiguredSubjects();
	RefreshSourceStatus();
}

void FViscaLiveLinkSource::RebuildSockets()
{
	ShutdownSockets();
	Receivers.Empty();

	FIPv4Address BindAddress = FIPv4Address::Any;
	if (!Settings.ListenInterfaceIp.Equals(TEXT("0.0.0.0"), ESearchCase::IgnoreCase))
	{
		if (!FIPv4Address::Parse(Settings.ListenInterfaceIp, BindAddress))
		{
			BindAddress = FIPv4Address::Any;
		}
	}

	for (const FViscaReceiverConfig& ReceiverConfig : Settings.Receivers)
	{
		if (ReceiverConfig.CameraName.IsNone())
		{
			continue;
		}

		const int32 Port = ParsePortString(ReceiverConfig.PortString);
		FViscaReceiverRuntime& ReceiverRuntime = Receivers.AddDefaulted_GetRef();
		ReceiverRuntime.Config = ReceiverConfig;
		ReceiverRuntime.SubjectName = ReceiverConfig.CameraName;
		ReceiverRuntime.bListenSocketOk = false;

		if (ReceiverConfig.Transport == EViscaReceiverTransportMode::TCP)
		{
			ReceiverRuntime.Socket = FTcpSocketBuilder(
				FString::Printf(TEXT("VISCA_TCP_%s_%d"), *ReceiverRuntime.SubjectName.ToString(), Port))
				.AsReusable()
				.BoundToAddress(BindAddress)
				.BoundToPort(Port)
				.Listening(8);

			if (ReceiverRuntime.Socket)
			{
				ReceiverRuntime.Socket->SetNonBlocking(true);
				ReceiverRuntime.bListenSocketOk = true;
			}
		}
		else
		{
			ReceiverRuntime.Socket = FUdpSocketBuilder(
				FString::Printf(TEXT("VISCA_UDP_%s_%d"), *ReceiverRuntime.SubjectName.ToString(), Port))
				.AsNonBlocking()
				.AsReusable()
				.BoundToAddress(BindAddress)
				.BoundToPort(Port)
				.WithReceiveBufferSize(MaxUdpPacketSize);
			ReceiverRuntime.bListenSocketOk = (ReceiverRuntime.Socket != nullptr);
		}
	}

	RefreshSourceStatus();
}

void FViscaLiveLinkSource::ResetTcpClientConnection(FViscaReceiverRuntime& Receiver)
{
	Receiver.TcpReceiveBuffer.Reset();
	{
		FScopeLock Lock(&Receiver.TcpSendLock);
		Receiver.PendingTcpIpPackets.Reset();
	}
	if (Receiver.TcpAcceptedSocket)
	{
		Receiver.TcpAcceptedSocket->Close();
		if (SocketSubsystem)
		{
			SocketSubsystem->DestroySocket(Receiver.TcpAcceptedSocket);
		}
		Receiver.TcpAcceptedSocket = nullptr;
	}
}

void FViscaLiveLinkSource::FlushPendingTcpSends(FViscaReceiverRuntime& Receiver)
{
	if (!Receiver.TcpAcceptedSocket)
	{
		return;
	}

	TArray<TArray<uint8>> LocalPackets;
	{
		FScopeLock Lock(&Receiver.TcpSendLock);
		LocalPackets = MoveTemp(Receiver.PendingTcpIpPackets);
		Receiver.PendingTcpIpPackets.Reset();
	}

	for (TArray<uint8>& Packet : LocalPackets)
	{
		int32 Sent = 0;
		if (!Receiver.TcpAcceptedSocket->Send(Packet.GetData(), Packet.Num(), Sent))
		{
			const ESocketErrors Err = SocketSubsystem ? SocketSubsystem->GetLastErrorCode() : SE_NO_ERROR;
			if (Err != SE_EWOULDBLOCK)
			{
				ResetTcpClientConnection(Receiver);
				return;
			}
		}
	}
}

void FViscaLiveLinkSource::BuildViscaIpEnvelope(uint32 Sequence, const TArray<uint8>& ViscaPayload, TArray<uint8>& OutPacket) const
{
	const int32 L = ViscaPayload.Num();
	OutPacket.SetNumUninitialized(8 + L);
	OutPacket[0] = 0x01;
	OutPacket[1] = 0x11; // peripheral → controller (VISCA-over-IP reply)
	OutPacket[2] = static_cast<uint8>((L >> 8) & 0xFF);
	OutPacket[3] = static_cast<uint8>(L & 0xFF);
	OutPacket[4] = static_cast<uint8>((Sequence >> 24) & 0xFF);
	OutPacket[5] = static_cast<uint8>((Sequence >> 16) & 0xFF);
	OutPacket[6] = static_cast<uint8>((Sequence >> 8) & 0xFF);
	OutPacket[7] = static_cast<uint8>(Sequence & 0xFF);
	if (L > 0)
	{
		FMemory::Memcpy(OutPacket.GetData() + 8, ViscaPayload.GetData(), L);
	}
}

void FViscaLiveLinkSource::QueueViscaIpReply(FViscaReceiverRuntime& Receiver, uint32 Sequence, const TArray<uint8>& ViscaPayload)
{
	TArray<uint8> Packet;
	BuildViscaIpEnvelope(Sequence, ViscaPayload, Packet);
	FScopeLock Lock(&Receiver.TcpSendLock);
	Receiver.PendingTcpIpPackets.Add(MoveTemp(Packet));
}

void FViscaLiveLinkSource::AppendViscaNibblesU14(TArray<uint8>& Out, uint32 Value14)
{
	Value14 &= 0x3FFF;
	for (int32 i = 3; i >= 0; --i)
	{
		Out.Add(static_cast<uint8>((Value14 >> (i * 4)) & 0x0F));
	}
}

void FViscaLiveLinkSource::AppendViscaNibblesU16(TArray<uint8>& Out, uint32 Value16)
{
	Value16 &= 0xFFFF;
	for (int32 i = 3; i >= 0; --i)
	{
		Out.Add(static_cast<uint8>((Value16 >> (i * 4)) & 0x0F));
	}
}

void FViscaLiveLinkSource::AppendInquiryCompletion(const TArray<uint8>& Inq, const FViscaCameraState& State, TArray<uint8>& OutVisca) const
{
	OutVisca.Reset();
	constexpr uint8 CmpMid = 0x51; // completion, socket 1
	if (Inq.Num() < 5)
	{
		OutVisca = { 0x90, CmpMid, 0xFF };
		return;
	}

	const uint8 Cat = Inq[2];
	const uint8 Cmd = Inq[3];

	if (Cat == 0x04 && Cmd == 0x47)
	{
		const uint32 Z = FMath::Min(static_cast<uint32>(State.NormalizedZoom * 16383.0f), 0x3FFFu);
		OutVisca.Add(0x90);
		OutVisca.Add(CmpMid);
		AppendViscaNibblesU14(OutVisca, Z);
		OutVisca.Add(0xFF);
		return;
	}
	if (Cat == 0x04 && Cmd == 0x48)
	{
		const uint32 F = FMath::Min(static_cast<uint32>(State.NormalizedFocus * 65535.f), 65535u);
		OutVisca.Add(0x90);
		OutVisca.Add(CmpMid);
		AppendViscaNibblesU16(OutVisca, F);
		OutVisca.Add(0xFF);
		return;
	}
	if (Cat == 0x06 && Cmd == 0x12)
	{
		const uint32 P = static_cast<uint32>(State.PanNormalized * 65535.f) & 0xFFFFu;
		OutVisca.Add(0x90);
		OutVisca.Add(CmpMid);
		AppendViscaNibblesU16(OutVisca, P);
		OutVisca.Add(0xFF);
		return;
	}
	if (Cat == 0x06 && Cmd == 0x13)
	{
		const uint32 T = static_cast<uint32>(State.TiltNormalized * 65535.f) & 0xFFFFu;
		OutVisca.Add(0x90);
		OutVisca.Add(CmpMid);
		AppendViscaNibblesU16(OutVisca, T);
		OutVisca.Add(0xFF);
		return;
	}
	if (Cat == 0x04 && Cmd == 0x00 && Inq.Num() >= 6 && Inq[4] == 0x02)
	{
		OutVisca = { 0x90, CmpMid, 0x02, 0xFF };
		return;
	}
	if (Cat == 0x00 && Cmd == 0x02)
	{
		OutVisca = { 0x90, CmpMid, 0x00, 0x02, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF };
		return;
	}
	if (Cat == 0x7E && Cmd == 0x7E)
	{
		OutVisca = { 0x90, CmpMid, 0x00, 0x02, 0x00, 0x01, 0xFF };
		return;
	}

	OutVisca = { 0x90, CmpMid, 0xFF };
}

void FViscaLiveLinkSource::SendViscaCommandResponses(
	FViscaReceiverRuntime& Receiver,
	const TArray<uint8>& CommandPayload,
	uint32 IpSequence,
	bool bUseIpEnvelope,
	const FIPv4Endpoint& UdpEndpoint,
	const FViscaCameraState& StateForInquiryReplies)
{
	constexpr uint8 SocketNum = 1;
	const uint8 AckMid = static_cast<uint8>(0x40 | SocketNum);
	const uint8 CmpMid = static_cast<uint8>(0x50 | SocketNum);

	const TArray<uint8> Ack = { 0x90, AckMid, 0xFF };

	auto Deliver = [&](const TArray<uint8>& Visca)
	{
		if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
		{
			if (bUseIpEnvelope)
			{
				QueueViscaIpReply(Receiver, IpSequence, Visca);
			}
			else
			{
				FScopeLock Lock(&Receiver.TcpSendLock);
				Receiver.PendingTcpIpPackets.Add(TArray<uint8>(Visca));
			}
		}
		else if (Receiver.Socket)
		{
			TArray<uint8> Wire;
			if (bUseIpEnvelope)
			{
				BuildViscaIpEnvelope(IpSequence, Visca, Wire);
			}
			else
			{
				Wire = Visca;
			}
			int32 Sent = 0;
			TSharedRef<FInternetAddr> Addr = UdpEndpoint.ToInternetAddr();
			Receiver.Socket->SendTo(Wire.GetData(), Wire.Num(), Sent, *Addr);
		}
	};

	Deliver(Ack);

	if (CommandPayload.Num() >= 2 && CommandPayload[1] == 0x09)
	{
		TArray<uint8> Cmp;
		AppendInquiryCompletion(CommandPayload, StateForInquiryReplies, Cmp);
		if (Cmp.Num() > 0)
		{
			Deliver(Cmp);
		}
	}
	else
	{
		const TArray<uint8> Cmp = { 0x90, CmpMid, 0xFF };
		Deliver(Cmp);
	}
}

bool FViscaLiveLinkSource::TryConsumeTcpViscaMessage(TArray<uint8>& InOutBuffer, TArray<uint8>& OutVisca, uint32& OutSeq, bool& bOutIpWrapped)
{
	bOutIpWrapped = false;
	OutSeq = 0;
	OutVisca.Reset();

	if (InOutBuffer.Num() == 0)
	{
		return false;
	}

	// VISCA-over-IP: 0x01 0x00 command, 0x01 0x10 inquiry, etc. Never treat 0x01 0x11 (device→controller) as a header here.
	if (InOutBuffer.Num() >= 8 && InOutBuffer[0] == 0x01 && InOutBuffer[1] != 0x11)
	{
		const uint32 PlenBE = (static_cast<uint32>(InOutBuffer[2]) << 8) | InOutBuffer[3];
		const uint32 PlenLE = static_cast<uint32>(InOutBuffer[2]) | (static_cast<uint32>(InOutBuffer[3]) << 8);
		uint32 Plen = PlenBE;

		auto Fits = [&](uint32 L) { return L <= 1024u && InOutBuffer.Num() >= 8 + static_cast<int32>(L); };

		if (!Fits(Plen))
		{
			if (Fits(PlenLE))
			{
				Plen = PlenLE;
			}
			else if (PlenBE > 1024u && PlenLE <= 1024u && InOutBuffer.Num() >= 8 + static_cast<int32>(PlenLE))
			{
				Plen = PlenLE;
			}
			else if (PlenBE > 1024u)
			{
				InOutBuffer.RemoveAt(0, 1);
				return false;
			}
			else
			{
				return false;
			}
		}

		OutSeq = (static_cast<uint32>(InOutBuffer[4]) << 24) | (static_cast<uint32>(InOutBuffer[5]) << 16)
			| (static_cast<uint32>(InOutBuffer[6]) << 8) | InOutBuffer[7];

		OutVisca.Append(InOutBuffer.GetData() + 8, static_cast<int32>(Plen));
		InOutBuffer.RemoveAt(0, 8 + static_cast<int32>(Plen), EAllowShrinking::No);
		bOutIpWrapped = true;
		return true;
	}

	int32 StartIndex = INDEX_NONE;
	int32 EndIndex = INDEX_NONE;
	for (int32 Index = 0; Index < InOutBuffer.Num(); ++Index)
	{
		if (StartIndex == INDEX_NONE && (InOutBuffer[Index] & 0xF0) == 0x80)
		{
			StartIndex = Index;
		}
		if (StartIndex != INDEX_NONE && InOutBuffer[Index] == 0xFF)
		{
			EndIndex = Index;
			break;
		}
	}

	if (StartIndex == INDEX_NONE || EndIndex == INDEX_NONE || EndIndex <= StartIndex)
	{
		return false;
	}

	OutVisca.Append(InOutBuffer.GetData() + StartIndex, EndIndex - StartIndex + 1);
	InOutBuffer.RemoveAt(0, EndIndex + 1, EAllowShrinking::No);
	bOutIpWrapped = false;
	return true;
}

bool FViscaLiveLinkSource::TryUnwrapUdpViscaIp(const TArray<uint8>& Datagram, TArray<uint8>& OutVisca, uint32& OutSeq, bool& bOutWrapped)
{
	bOutWrapped = false;
	OutSeq = 0;
	OutVisca.Reset();

	if (Datagram.Num() >= 8 && Datagram[0] == 0x01 && Datagram[1] != 0x11)
	{
		const uint32 PlenBE = (static_cast<uint32>(Datagram[2]) << 8) | Datagram[3];
		const uint32 PlenLE = static_cast<uint32>(Datagram[2]) | (static_cast<uint32>(Datagram[3]) << 8);
		uint32 Plen = PlenBE;
		if (Plen > 1024u || Datagram.Num() < 8 + static_cast<int32>(Plen))
		{
			if (PlenLE <= 1024u && Datagram.Num() >= 8 + static_cast<int32>(PlenLE))
			{
				Plen = PlenLE;
			}
			else
			{
				return false;
			}
		}
		OutSeq = (static_cast<uint32>(Datagram[4]) << 24) | (static_cast<uint32>(Datagram[5]) << 16)
			| (static_cast<uint32>(Datagram[6]) << 8) | Datagram[7];
		OutVisca.Append(Datagram.GetData() + 8, static_cast<int32>(Plen));
		bOutWrapped = true;
		return true;
	}

	return false;
}

void FViscaLiveLinkSource::ShutdownSockets()
{
	for (FViscaReceiverRuntime& Receiver : Receivers)
	{
		Receiver.TcpReceiveBuffer.Reset();
		{
			FScopeLock Lock(&Receiver.TcpSendLock);
			Receiver.PendingTcpIpPackets.Reset();
		}

		if (Receiver.TcpAcceptedSocket)
		{
			Receiver.TcpAcceptedSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Receiver.TcpAcceptedSocket);
			Receiver.TcpAcceptedSocket = nullptr;
		}

		if (Receiver.Socket)
		{
			Receiver.Socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Receiver.Socket);
			Receiver.Socket = nullptr;
		}
	}
}

void FViscaLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	Client = InClient;
	SourceGuid = InSourceGuid;
	PublishConfiguredSubjects();
	RefreshSourceStatus();
}

void FViscaLiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* InSettings)
{
	if (UViscaLiveLinkSourceSettings* TypedSettings = Cast<UViscaLiveLinkSourceSettings>(InSettings))
	{
		TypedSettings->ListenInterfaceIp = Settings.ListenInterfaceIp;
		TypedSettings->Receivers = Settings.Receivers;
		// VISCA is burst/event driven. Latest keeps snapshots valid without engine timecode / buffer tuning.
		TypedSettings->Mode = ELiveLinkSourceMode::Latest;
	}

	InSettings->ConnectionString = Settings.ToString();
}

TSubclassOf<ULiveLinkSourceSettings> FViscaLiveLinkSource::GetSettingsClass() const
{
	return UViscaLiveLinkSourceSettings::StaticClass();
}

void FViscaLiveLinkSource::OnSettingsChanged(ULiveLinkSourceSettings* InSettings, const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (const UViscaLiveLinkSourceSettings* TypedSettings = Cast<UViscaLiveLinkSourceSettings>(InSettings))
	{
		FViscaLiveLinkConnectionSettings NewSettings;
		NewSettings.ListenInterfaceIp = TypedSettings->ListenInterfaceIp;
		NewSettings.Receivers = TypedSettings->Receivers;

		RestartWithSettings(NewSettings);
		InSettings->ConnectionString = Settings.ToString();
	}
}

bool FViscaLiveLinkSource::IsSourceStillValid() const
{
	return bIsRunning && Thread != nullptr;
}

bool FViscaLiveLinkSource::RequestSourceShutdown()
{
	PerformShutdownOnce();
	SourceStatus = LOCTEXT("SourceStatusDisconnected", "Disconnected");
	return true;
}

void FViscaLiveLinkSource::PerformShutdownOnce()
{
	FScopeLock Lock(&ShutdownMutex);
	if (bHasPerformedShutdown)
	{
		return;
	}
	bHasPerformedShutdown = true;

	bShuttingDown = true;
	bIsRunning = false;

	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}

	ShutdownSockets();

	if (Client)
	{
		ClearSubjects();
	}
	Client = nullptr;
}

void FViscaLiveLinkSource::RefreshSourceStatus()
{
	const int32 NumReceivers = Receivers.Num();
	if (NumReceivers == 0)
	{
		SourceStatus = LOCTEXT("StatusNoReceivers", "No receivers — add cameras in Source Settings");
		return;
	}

	int32 NumOk = 0;
	for (const FViscaReceiverRuntime& R : Receivers)
	{
		if (R.bListenSocketOk && R.Socket != nullptr)
		{
			++NumOk;
		}
	}

	if (NumOk == NumReceivers)
	{
		SourceStatus = FText::Format(LOCTEXT("StatusListeningAll", "Listening ({0} receiver(s))"), NumReceivers);
	}
	else
	{
		SourceStatus = FText::Format(
			LOCTEXT("StatusListeningPartial", "Listening {0} / {1} — check bind address and ports"),
			NumOk,
			NumReceivers);
	}
}

void FViscaLiveLinkSource::Update()
{
	RefreshSourceStatus();

	if (!Client || Receivers.Num() == 0)
	{
		return;
	}

	const double Now = FPlatformTime::Seconds();
	float Dt = 0.0f;
	if (LastPtzIntegrateTimeSecs > 0.0)
	{
		Dt = FMath::Clamp(static_cast<float>(Now - LastPtzIntegrateTimeSecs), 0.0f, 0.1f);
	}
	LastPtzIntegrateTimeSecs = Now;
	if (Dt <= 0.0f)
	{
		return;
	}

	FScopeLock Lock(&CameraStateLock);
	for (FViscaReceiverRuntime& Receiver : Receivers)
	{
		FViscaCameraState& S = Receiver.State;
		const bool bMoving = !FMath::IsNearlyZero(S.PanAxisVelocity, 1.e-4f)
			|| !FMath::IsNearlyZero(S.TiltAxisVelocity, 1.e-4f)
			|| !FMath::IsNearlyZero(S.ZoomAxisVelocity, 1.e-4f);
		if (!bMoving)
		{
			continue;
		}

		S.PanNormalized = FMath::Clamp(S.PanNormalized + S.PanAxisVelocity * PanTiltDriveUnitsPerSec * Dt, 0.0f, 1.0f);
		S.TiltNormalized = FMath::Clamp(S.TiltNormalized + S.TiltAxisVelocity * PanTiltDriveUnitsPerSec * Dt, 0.0f, 1.0f);
		S.NormalizedZoom = FMath::Clamp(S.NormalizedZoom + S.ZoomAxisVelocity * Dt, 0.0f, 1.0f);
		UpdateTransformFromState(S);
		PushFrame(Receiver);
	}
}

void FViscaLiveLinkSource::Stop()
{
	bIsRunning = false;
}

uint32 FViscaLiveLinkSource::Run()
{
	while (bIsRunning)
	{
		for (int32 Index = 0; Index < Receivers.Num(); ++Index)
		{
			FViscaReceiverRuntime& Receiver = Receivers[Index];
			if (!Receiver.Socket)
			{
				continue;
			}

			FSocket* ReadSocket = Receiver.Socket;
			if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
			{
				if (Receiver.TcpAcceptedSocket
					&& Receiver.TcpAcceptedSocket->GetConnectionState() != SCS_Connected)
				{
					ResetTcpClientConnection(Receiver);
				}

				if (!Receiver.TcpAcceptedSocket && Receiver.Socket)
				{
					Receiver.Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime);
					bool bHasPendingConnection = false;
					if (Receiver.Socket->HasPendingConnection(bHasPendingConnection) && bHasPendingConnection)
					{
						FSocket* NewClient = Receiver.Socket->Accept(TEXT("VISCAAcceptedTCP"));
						if (NewClient)
						{
							Receiver.TcpAcceptedSocket = NewClient;
							Receiver.TcpAcceptedSocket->SetNonBlocking(true);
							Receiver.TcpReceiveBuffer.Reset();
							{
								FScopeLock ClearLock(&Receiver.TcpSendLock);
								Receiver.PendingTcpIpPackets.Reset();
							}
							// Drain accept backlog so a full queue cannot block the next reconnect.
							bool bExtra = false;
							while (Receiver.Socket->HasPendingConnection(bExtra) && bExtra)
							{
								FSocket* Extra = Receiver.Socket->Accept(TEXT("VISCAAcceptedDrop"));
								if (!Extra)
								{
									break;
								}
								Extra->Close();
								SocketSubsystem->DestroySocket(Extra);
								bExtra = false;
							}
						}
					}
				}
				ReadSocket = Receiver.TcpAcceptedSocket;
			}

			if (!ReadSocket)
			{
				continue;
			}

			if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
			{
				FlushPendingTcpSends(Receiver);
			}

			uint32 PendingDataSize = 0;
			if (ReadSocket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
			{
				while (ReadSocket->HasPendingData(PendingDataSize))
				{
					TArray<uint8> PacketData;
					PacketData.SetNumUninitialized(FMath::Min(static_cast<int32>(PendingDataSize), MaxUdpPacketSize));

					TSharedRef<FInternetAddr> SenderAddr = SocketSubsystem->CreateInternetAddr();
					int32 BytesRead = 0;
					bool bReceived = false;
					if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
					{
						bReceived = ReadSocket->Recv(PacketData.GetData(), PacketData.Num(), BytesRead);
						SenderAddr->SetIp(FIPv4Address::InternalLoopback.Value);
						SenderAddr->SetPort(ParsePortString(Receiver.Config.PortString));
						if (ReadSocket == Receiver.TcpAcceptedSocket && !bReceived)
						{
							ResetTcpClientConnection(Receiver);
							break;
						}
					}
					else
					{
						bReceived = ReadSocket->RecvFrom(PacketData.GetData(), PacketData.Num(), BytesRead, *SenderAddr);
					}

					if (bReceived && BytesRead > 0)
					{
						PacketData.SetNum(BytesRead);
						FIPv4Address SenderIp = FIPv4Address::InternalLoopback;
						FIPv4Address::Parse(SenderAddr->ToString(false), SenderIp);
						const FIPv4Endpoint SenderEndpoint(SenderIp, SenderAddr->GetPort());

						if (!bShuttingDown && bIsRunning)
						{
							if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
							{
								Receiver.TcpReceiveBuffer.Append(PacketData);
								if (Receiver.TcpReceiveBuffer.Num() > 65536)
								{
									Receiver.TcpReceiveBuffer.Reset();
								}
								else
								{
									TArray<uint8> Chunk;
									uint32 IpSeq = 0;
									bool bIpWrap = false;
									while (TryConsumeTcpViscaMessage(Receiver.TcpReceiveBuffer, Chunk, IpSeq, bIpWrap))
									{
										if (!bShuttingDown && bIsRunning)
										{
											AsyncTask(ENamedThreads::GameThread,
												[this, Index, Chunk = MoveTemp(Chunk), SenderEndpoint, IpSeq, bIpWrap]() mutable
												{
													ProcessIncomingPacket(Index, Chunk, SenderEndpoint, IpSeq, bIpWrap);
												});
										}
									}
								}
							}
							else
							{
								uint32 IpSeq = 0;
								bool bIpWrap = false;
								TArray<uint8> Unwrapped;
								if (TryUnwrapUdpViscaIp(PacketData, Unwrapped, IpSeq, bIpWrap))
								{
									AsyncTask(ENamedThreads::GameThread,
										[this, Index, Unwrapped = MoveTemp(Unwrapped), SenderEndpoint, IpSeq, bIpWrap]() mutable
										{
											ProcessIncomingPacket(Index, Unwrapped, SenderEndpoint, IpSeq, bIpWrap);
										});
								}
								else
								{
									AsyncTask(ENamedThreads::GameThread,
										[this, Index, PacketData = MoveTemp(PacketData), SenderEndpoint]() mutable
										{
											ProcessIncomingPacket(Index, PacketData, SenderEndpoint, 0, false);
										});
								}
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

void FViscaLiveLinkSource::EnsureSubject(FViscaReceiverRuntime& Receiver)
{
	if (!Client)
	{
		return;
	}

	const FLiveLinkSubjectKey SubjectKey(SourceGuid, Receiver.SubjectName);
	if (!CreatedSubjects.Contains(Receiver.SubjectName))
	{
		FLiveLinkSubjectPreset Preset;
		Preset.Key = SubjectKey;
		Preset.Role = ULiveLinkCameraRole::StaticClass();
		Preset.bEnabled = true;
		if (!Client->CreateSubject(Preset))
		{
			return;
		}
		Client->SetSubjectEnabled(SubjectKey, true);

		CreatedSubjects.Add(Receiver.SubjectName);
	}

	// Push static data only once per receiver. Re-pushing clears all buffered frames in Live Link and
	// prevents a valid frame snapshot (subjects show as Invalid / Unresponsive).
	if (!Receiver.bStaticDataPushed)
	{
		FLiveLinkStaticDataStruct StaticDataStruct(FLiveLinkCameraStaticData::StaticStruct());
		FLiveLinkCameraStaticData& StaticData = *StaticDataStruct.Cast<FLiveLinkCameraStaticData>();
		StaticData.bIsFocalLengthSupported = true;
		StaticData.bIsApertureSupported = true;
		StaticData.bIsFocusDistanceSupported = true;
		StaticData.bIsLocationSupported = false;
		StaticData.bIsRotationSupported = true;
		StaticData.bIsScaleSupported = false;
		Client->PushSubjectStaticData_AnyThread(SubjectKey, ULiveLinkCameraRole::StaticClass(), MoveTemp(StaticDataStruct));
		Receiver.bStaticDataPushed = true;
	}
}

void FViscaLiveLinkSource::PushFrame(const FViscaReceiverRuntime& Receiver)
{
	if (!Client)
	{
		return;
	}

	const FLiveLinkSubjectKey SubjectKey(SourceGuid, Receiver.SubjectName);

	FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkCameraFrameData::StaticStruct());
	FLiveLinkCameraFrameData& FrameData = *FrameDataStruct.Cast<FLiveLinkCameraFrameData>();
	// Use engine time so Live Link's LastPushTime matches the frame clock (see FLiveLinkSubject::AddFrameData).
	const double WorldSeconds = FApp::GetCurrentTime();
	FrameData.WorldTime = FLiveLinkWorldTime(WorldSeconds);
	// Valid scene time keeps Timecode evaluation mode and frame-rate bookkeeping from starving snapshots.
	const FFrameTime SceneFrameTime = ViscaSceneFrameRate.AsFrameTime(WorldSeconds);
	FrameData.MetaData.SceneTime = FQualifiedFrameTime(SceneFrameTime, ViscaSceneFrameRate);
	FrameData.MetaData.StringMetaData.Add(TEXT("ViscaSubject"), Receiver.SubjectName.ToString());
	FrameData.Transform = Receiver.State.CameraTransform;
	FrameData.FocalLength = Receiver.State.NormalizedZoom;
	FrameData.FocusDistance = Receiver.State.NormalizedFocus;
	FrameData.Aperture = Receiver.State.NormalizedIris;
	Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
}

void FViscaLiveLinkSource::ProcessIncomingPacket(
	int32 ReceiverIndex,
	const TArray<uint8>& PacketData,
	const FIPv4Endpoint& SenderEndpoint,
	uint32 ViscaIpSequence,
	bool bViscaIpWrapped)
{
	if (bShuttingDown || !Receivers.IsValidIndex(ReceiverIndex) || !Client)
	{
		return;
	}

	FViscaReceiverRuntime& Receiver = Receivers[ReceiverIndex];
	EnsureSubject(Receiver);

	TArray<uint8> CommandPayload;
	if (!ParseViscaPayload(PacketData, CommandPayload))
	{
		return;
	}

	// Ignore peripheral→controller packets if they ever appear on the wire.
	if (CommandPayload.Num() >= 1 && (CommandPayload[0] & 0xF0) == 0x90)
	{
		return;
	}

	FViscaCameraState StateForReplies;
	{
		FScopeLock Lock(&CameraStateLock);
		ApplyViscaCommandToState(Receiver, CommandPayload);
		PushFrame(Receiver);
		StateForReplies = Receiver.State;
	}
	SendViscaCommandResponses(Receiver, CommandPayload, ViscaIpSequence, bViscaIpWrapped, SenderEndpoint, StateForReplies);
}

bool FViscaLiveLinkSource::ParseViscaPayload(const TArray<uint8>& PacketData, TArray<uint8>& OutViscaPayload) const
{
	int32 StartIndex = INDEX_NONE;
	int32 EndIndex = INDEX_NONE;
	for (int32 Index = 0; Index < PacketData.Num(); ++Index)
	{
		if (StartIndex == INDEX_NONE && (PacketData[Index] & 0xF0) == 0x80)
		{
			StartIndex = Index;
		}

		if (StartIndex != INDEX_NONE && PacketData[Index] == 0xFF)
		{
			EndIndex = Index;
			break;
		}
	}

	if (StartIndex == INDEX_NONE || EndIndex == INDEX_NONE || EndIndex <= StartIndex)
	{
		return false;
	}

	OutViscaPayload.Reset();
	OutViscaPayload.Append(PacketData.GetData() + StartIndex, EndIndex - StartIndex + 1);
	return OutViscaPayload.Num() >= 4;
}

void FViscaLiveLinkSource::ApplyViscaCommandToState(FViscaReceiverRuntime& InOutReceiver, const TArray<uint8>& CommandPayload)
{
	FViscaCameraState& InOutState = InOutReceiver.State;

	// Inquiries are answered in SendViscaCommandResponses; they do not drive simulated state here.
	if (CommandPayload.Num() >= 2 && CommandPayload[1] == 0x09)
	{
		return;
	}

	// Expect: 8x 01 <category> <command> ...
	if (CommandPayload.Num() < 5)
	{
		return;
	}

	const uint8 Category = CommandPayload[2];
	const uint8 Command = CommandPayload[3];
	InOutState.LastUpdateTime = FPlatformTime::Seconds();

	if (Category == 0x04 && Command == 0x07 && CommandPayload.Num() >= 6)
	{
		// Zoom standard/variable/stop — sustained velocity until stop or direct zoom.
		const uint8 ZoomParam = CommandPayload[4];
		if (ZoomParam == 0x00)
		{
			InOutState.ZoomAxisVelocity = 0.0f;
		}
		else if (ZoomParam == 0x02)
		{
			InOutState.ZoomAxisVelocity = ZoomStandardVelocity;
		}
		else if (ZoomParam == 0x03)
		{
			InOutState.ZoomAxisVelocity = -ZoomStandardVelocity;
		}
		else if ((ZoomParam & 0xF0) == 0x20)
		{
			const float Speed = static_cast<float>(ZoomParam & 0x0F);
			InOutState.ZoomAxisVelocity = ((Speed + 1.0f) / 8.0f) * ZoomVariableVelocityScale;
		}
		else if ((ZoomParam & 0xF0) == 0x30)
		{
			const float Speed = static_cast<float>(ZoomParam & 0x0F);
			InOutState.ZoomAxisVelocity = -((Speed + 1.0f) / 8.0f) * ZoomVariableVelocityScale;
		}
	}
	else if (Category == 0x7E && Command == 0x04 && CommandPayload.Num() >= 11 && CommandPayload[4] == 0x17)
	{
		// High resolution zoom tele/wide + speed.
		const bool bTele = CommandPayload[5] == 0x02;
		const int32 RawSpeed = (CommandPayload[6] << 12) | (CommandPayload[7] << 8) | (CommandPayload[8] << 4) | CommandPayload[9];
		const float SpeedNorm = FMath::Clamp(static_cast<float>(RawSpeed) / static_cast<float>(0x7FFE), 0.01f, 1.0f);
		InOutState.ZoomAxisVelocity = (bTele ? 1.0f : -1.0f) * SpeedNorm * ZoomHighResVelocityScale;
	}
	else if (Category == 0x04 && Command == 0x47 && CommandPayload.Num() >= 9)
	{
		// Zoom Direct: 0x0000 - 0x6000
		const int32 RawZoom = (CommandPayload[4] << 12) | (CommandPayload[5] << 8) | (CommandPayload[6] << 4) | CommandPayload[7];
		InOutState.NormalizedZoom = FMath::Clamp(static_cast<float>(RawZoom) / 0x6000f, 0.0f, 1.0f);
		InOutState.ZoomAxisVelocity = 0.0f;
	}
	else if (Category == 0x04 && Command == 0x08 && CommandPayload.Num() >= 6)
	{
		// Focus standard/variable/stop
		const uint8 FocusParam = CommandPayload[4];
		if (FocusParam == 0x00)
		{
			// Stop: keep current focus.
		}
		else if (FocusParam == 0x02)
		{
			InOutState.NormalizedFocus = FMath::Clamp(InOutState.NormalizedFocus + StandardStep, 0.0f, 1.0f);
		}
		else if (FocusParam == 0x03)
		{
			InOutState.NormalizedFocus = FMath::Clamp(InOutState.NormalizedFocus - StandardStep, 0.0f, 1.0f);
		}
		else if ((FocusParam & 0xF0) == 0x20)
		{
			const float Speed = static_cast<float>(FocusParam & 0x0F);
			InOutState.NormalizedFocus = FMath::Clamp(InOutState.NormalizedFocus + (Speed + 1.0f) * VariableStepScale, 0.0f, 1.0f);
		}
		else if ((FocusParam & 0xF0) == 0x30)
		{
			const float Speed = static_cast<float>(FocusParam & 0x0F);
			InOutState.NormalizedFocus = FMath::Clamp(InOutState.NormalizedFocus - (Speed + 1.0f) * VariableStepScale, 0.0f, 1.0f);
		}
	}
	else if (Category == 0x04 && Command == 0x48 && CommandPayload.Num() >= 9)
	{
		// Focus Direct: 0x0000 - 0xFFFF
		const int32 RawFocus = (CommandPayload[4] << 12) | (CommandPayload[5] << 8) | (CommandPayload[6] << 4) | CommandPayload[7];
		InOutState.NormalizedFocus = FMath::Clamp(static_cast<float>(RawFocus) / 65535.0f, 0.0f, 1.0f);
	}
	else if (Category == 0x04 && Command == 0x38 && CommandPayload.Num() >= 6)
	{
		// Focus mode (Auto/Manual/Toggle)
		const uint8 FocusMode = CommandPayload[4];
		if (FocusMode == 0x02)
		{
			InOutState.bAutoFocus = true;
		}
		else if (FocusMode == 0x03)
		{
			InOutState.bAutoFocus = false;
		}
		else if (FocusMode == 0x10)
		{
			InOutState.bAutoFocus = !InOutState.bAutoFocus;
		}
	}
	else if (Category == 0x7E && Command == 0x04 && CommandPayload.Num() >= 9 && CommandPayload[4] == 0x4B)
	{
		// Iris Up/Down step, loosely treated as normalized delta.
		const bool bIsUp = CommandPayload[5] == 0x02;
		const float Delta = static_cast<float>(FMath::Clamp(static_cast<int32>(CommandPayload[6]) * 16 + CommandPayload[7], 1, 255)) / 255.0f;
		InOutState.NormalizedIris = FMath::Clamp(InOutState.NormalizedIris + (bIsUp ? Delta : -Delta), 0.0f, 1.0f);
	}
	else if (Category == 0x05 && Command == 0x34 && CommandPayload.Num() >= 6)
	{
		// Auto iris on/off.
		const uint8 AutoIrisParam = CommandPayload[4];
		if (AutoIrisParam == 0x02)
		{
			InOutState.bAutoIris = true;
		}
		else if (AutoIrisParam == 0x03)
		{
			InOutState.bAutoIris = false;
		}
	}
	else if (Category == 0x06 && Command == 0x01 && CommandPayload.Num() >= 9)
	{
		// Pan/tilt drive: 81 01 06 01 VV WW PP TT FF — PP: 01=left 02=right 03=stop; TT: 01=up 02=down 03=stop (Sony VISCA).
		constexpr float MaxViscaPtzSpeed = 24.0f;
		const float PanSpeed = FMath::Clamp(static_cast<float>(CommandPayload[4]) / MaxViscaPtzSpeed, 0.0f, 1.0f);
		const float TiltSpeed = FMath::Clamp(static_cast<float>(CommandPayload[5]) / MaxViscaPtzSpeed, 0.0f, 1.0f);
		const uint8 PanDir = CommandPayload[6];
		const uint8 TiltDir = CommandPayload[7];

		if (PanDir == 0x01)
		{
			InOutState.PanAxisVelocity = -PanSpeed;
		}
		else if (PanDir == 0x02)
		{
			InOutState.PanAxisVelocity = PanSpeed;
		}
		else
		{
			InOutState.PanAxisVelocity = 0.0f;
		}

		if (TiltDir == 0x01)
		{
			InOutState.TiltAxisVelocity = TiltSpeed;
		}
		else if (TiltDir == 0x02)
		{
			InOutState.TiltAxisVelocity = -TiltSpeed;
		}
		else
		{
			InOutState.TiltAxisVelocity = 0.0f;
		}
	}
	else if (Category == 0x06 && Command == 0x03 && CommandPayload.Num() >= 17)
	{
		// Pan/Tilt relative nibble-packed deltas.
		InOutState.PanAxisVelocity = 0.0f;
		InOutState.TiltAxisVelocity = 0.0f;
		int32 RawPanDelta = 0;
		int32 RawTiltDelta = 0;
		for (int32 i = 0; i < 5; ++i)
		{
			RawPanDelta = (RawPanDelta << 4) | CommandPayload[6 + i];
			RawTiltDelta = (RawTiltDelta << 4) | CommandPayload[11 + i];
		}

		const float PanDelta = ((static_cast<float>(RawPanDelta) / 0xFFFFF) - 0.5f) * 0.1f;
		const float TiltDelta = ((static_cast<float>(RawTiltDelta) / 0xFFFFF) - 0.5f) * 0.1f;
		InOutState.PanNormalized = FMath::Clamp(InOutState.PanNormalized + PanDelta, 0.0f, 1.0f);
		InOutState.TiltNormalized = FMath::Clamp(InOutState.TiltNormalized + TiltDelta, 0.0f, 1.0f);
	}
	else if (Category == 0x06 && Command == 0x02 && CommandPayload.Num() >= 17)
	{
		// Pan/Tilt absolute nibble-packed values.
		InOutState.PanAxisVelocity = 0.0f;
		InOutState.TiltAxisVelocity = 0.0f;
		int32 RawPan = 0;
		int32 RawTilt = 0;
		for (int32 i = 0; i < 5; ++i)
		{
			RawPan = (RawPan << 4) | CommandPayload[6 + i];
			RawTilt = (RawTilt << 4) | CommandPayload[11 + i];
		}
		InOutState.PanNormalized = FMath::Clamp(static_cast<float>(RawPan) / 0xFFFFF, 0.0f, 1.0f);
		InOutState.TiltNormalized = FMath::Clamp(static_cast<float>(RawTilt) / 0xFFFFF, 0.0f, 1.0f);
	}
	else if (Category == 0x06 && Command == 0x04)
	{
		// HOME
		InOutState.PanNormalized = 0.5f;
		InOutState.TiltNormalized = DefaultTiltNormalizedForLevel;
		InOutState.PanAxisVelocity = 0.0f;
		InOutState.TiltAxisVelocity = 0.0f;
		InOutState.ZoomAxisVelocity = 0.0f;
	}
	else if (Category == 0x06 && Command == 0x05)
	{
		// RESET
		InOutState = FViscaCameraState();
	}
	else if (Category == 0x04 && Command == 0x3F && CommandPayload.Num() >= 7)
	{
		// Preset SET/RESET/RECALL
		const uint8 PresetAction = CommandPayload[4];
		const uint8 PresetSlot = CommandPayload[5];
		if (PresetAction == 0x01)
		{
			FViscaCameraState Snapshot = InOutState;
			Snapshot.PanAxisVelocity = 0.0f;
			Snapshot.TiltAxisVelocity = 0.0f;
			Snapshot.ZoomAxisVelocity = 0.0f;
			InOutReceiver.Presets.Add(PresetSlot, Snapshot);
		}
		else if (PresetAction == 0x00)
		{
			InOutReceiver.Presets.Remove(PresetSlot);
		}
		else if (PresetAction == 0x02)
		{
			if (const FViscaCameraState* PresetState = InOutReceiver.Presets.Find(PresetSlot))
			{
				InOutState = *PresetState;
			}
		}
	}

	UpdateTransformFromState(InOutState);
}

void FViscaLiveLinkSource::UpdateTransformFromState(FViscaCameraState& InOutState)
{
	const float PanDegrees = FMath::Lerp(-170.0f, 170.0f, InOutState.PanNormalized);
	const float TiltDegrees = FMath::Lerp(-30.0f, 90.0f, InOutState.TiltNormalized);
	InOutState.CameraTransform.SetRotation(FQuat(FRotator(TiltDegrees, PanDegrees, 0.0f)));
}

void FViscaLiveLinkSource::PublishConfiguredSubjects()
{
	for (FViscaReceiverRuntime& Receiver : Receivers)
	{
		EnsureSubject(Receiver);
		FScopeLock Lock(&CameraStateLock);
		PushFrame(Receiver);
	}
}

void FViscaLiveLinkSource::ClearSubjects()
{
	if (!Client)
	{
		return;
	}

	FScopeLock ScopeLock(&SubjectsLock);
	for (const FName& SubjectName : CreatedSubjects)
	{
		Client->RemoveSubject_AnyThread(FLiveLinkSubjectKey(SourceGuid, SubjectName));
	}
	CreatedSubjects.Empty();
}

#undef LOCTEXT_NAMESPACE
