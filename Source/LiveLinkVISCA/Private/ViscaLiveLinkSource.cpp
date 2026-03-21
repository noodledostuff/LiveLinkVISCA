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
#include "Misc/App.h"
#include "Misc/QualifiedFrameTime.h"
#include "Misc/ScopeLock.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "ViscaLiveLinkSource"

namespace
{
	constexpr int32 MaxUdpPacketSize = 65507;
	/** Scene time for Live Link frames (Timecode / display paths); world evaluation still uses WorldTime. */
	static const FFrameRate ViscaSceneFrameRate(60, 1);
	constexpr double CommandRetrySeconds = 0.2;
	constexpr int32 MaxRetryCount = 3;
	constexpr float StandardStep = 0.02f;
	constexpr float VariableStepScale = 0.005f;

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
				.Listening(1);

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

void FViscaLiveLinkSource::ShutdownSockets()
{
	for (FViscaReceiverRuntime& Receiver : Receivers)
	{
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
				if (!Receiver.TcpAcceptedSocket)
				{
					bool bHasPendingConnection = false;
					if (Receiver.Socket->HasPendingConnection(bHasPendingConnection) && bHasPendingConnection)
					{
						Receiver.TcpAcceptedSocket = Receiver.Socket->Accept(TEXT("VISCAAcceptedTCP"));
						if (Receiver.TcpAcceptedSocket)
						{
							Receiver.TcpAcceptedSocket->SetNonBlocking(true);
						}
					}
				}
				ReadSocket = Receiver.TcpAcceptedSocket;
			}

			if (!ReadSocket)
			{
				continue;
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
							AsyncTask(ENamedThreads::GameThread, [this, Index, PacketData = MoveTemp(PacketData), SenderEndpoint]() mutable
							{
								ProcessIncomingPacket(Index, PacketData, SenderEndpoint);
							});
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

void FViscaLiveLinkSource::ProcessIncomingPacket(int32 ReceiverIndex, const TArray<uint8>& PacketData, const FIPv4Endpoint& SenderEndpoint)
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

	if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
	{
		if (CommandPayload.Num() >= 2 && (CommandPayload[0] & 0xF0) == 0x90 && ((CommandPayload[1] & 0xF0) == 0x40 || (CommandPayload[1] & 0xF0) == 0x50))
		{
			Receiver.bAwaitingResponse = false;
			Receiver.RetryCount = 0;
			Receiver.PendingOutCommand.Reset();
		}
	}

	ApplyViscaCommandToState(Receiver, CommandPayload);
	PushFrame(Receiver);

	if (Receiver.Config.Transport == EViscaReceiverTransportMode::TCP)
	{
		if (Receiver.PendingOutCommand.IsEmpty())
		{
			Receiver.PendingOutCommand = CommandPayload;
		}
		MaybeSendQueuedCommand(Receiver, SenderEndpoint);
	}
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
		// Zoom standard/variable/stop
		const uint8 ZoomParam = CommandPayload[4];
		if (ZoomParam == 0x00)
		{
			// Stop: keep current zoom.
		}
		else if (ZoomParam == 0x02)
		{
			InOutState.NormalizedZoom = FMath::Clamp(InOutState.NormalizedZoom + StandardStep, 0.0f, 1.0f);
		}
		else if (ZoomParam == 0x03)
		{
			InOutState.NormalizedZoom = FMath::Clamp(InOutState.NormalizedZoom - StandardStep, 0.0f, 1.0f);
		}
		else if ((ZoomParam & 0xF0) == 0x20)
		{
			const float Speed = static_cast<float>(ZoomParam & 0x0F);
			InOutState.NormalizedZoom = FMath::Clamp(InOutState.NormalizedZoom + (Speed + 1.0f) * VariableStepScale, 0.0f, 1.0f);
		}
		else if ((ZoomParam & 0xF0) == 0x30)
		{
			const float Speed = static_cast<float>(ZoomParam & 0x0F);
			InOutState.NormalizedZoom = FMath::Clamp(InOutState.NormalizedZoom - (Speed + 1.0f) * VariableStepScale, 0.0f, 1.0f);
		}
	}
	else if (Category == 0x7E && Command == 0x04 && CommandPayload.Num() >= 11 && CommandPayload[4] == 0x17)
	{
		// High resolution zoom tele/wide + speed.
		const bool bTele = CommandPayload[5] == 0x02;
		const int32 RawSpeed = (CommandPayload[6] << 12) | (CommandPayload[7] << 8) | (CommandPayload[8] << 4) | CommandPayload[9];
		const float Delta = FMath::Clamp(static_cast<float>(RawSpeed) / 0x7FFEu, 0.0f, 1.0f) * 0.04f;
		InOutState.NormalizedZoom = FMath::Clamp(InOutState.NormalizedZoom + (bTele ? Delta : -Delta), 0.0f, 1.0f);
	}
	else if (Category == 0x04 && Command == 0x47 && CommandPayload.Num() >= 9)
	{
		// Zoom Direct: 0x0000 - 0x6000
		const int32 RawZoom = (CommandPayload[4] << 12) | (CommandPayload[5] << 8) | (CommandPayload[6] << 4) | CommandPayload[7];
		InOutState.NormalizedZoom = FMath::Clamp(static_cast<float>(RawZoom) / 0x6000f, 0.0f, 1.0f);
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
		// Pan/tilt drive command.
		const float PanSpeed = FMath::Clamp(static_cast<float>(CommandPayload[4]) / 0x18f, 0.0f, 1.0f);
		const float TiltSpeed = FMath::Clamp(static_cast<float>(CommandPayload[5]) / 0x18f, 0.0f, 1.0f);
		const uint8 PanDir = CommandPayload[6];
		const uint8 TiltDir = CommandPayload[7];
		InOutState.PanSpeedNormalized = PanSpeed;
		InOutState.TiltSpeedNormalized = TiltSpeed;

		if (PanDir == 0x01)
		{
			InOutState.PanNormalized = FMath::Clamp(InOutState.PanNormalized - PanSpeed * 0.02f, 0.0f, 1.0f);
		}
		else if (PanDir == 0x02)
		{
			InOutState.PanNormalized = FMath::Clamp(InOutState.PanNormalized + PanSpeed * 0.02f, 0.0f, 1.0f);
		}

		if (TiltDir == 0x01)
		{
			InOutState.TiltNormalized = FMath::Clamp(InOutState.TiltNormalized + TiltSpeed * 0.02f, 0.0f, 1.0f);
		}
		else if (TiltDir == 0x02)
		{
			InOutState.TiltNormalized = FMath::Clamp(InOutState.TiltNormalized - TiltSpeed * 0.02f, 0.0f, 1.0f);
		}
	}
	else if (Category == 0x06 && Command == 0x03 && CommandPayload.Num() >= 15)
	{
		// Pan/Tilt relative nibble-packed deltas.
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
	else if (Category == 0x06 && Command == 0x02 && CommandPayload.Num() >= 15)
	{
		// Pan/Tilt absolute nibble-packed values.
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
		InOutState.TiltNormalized = 0.5f;
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
			InOutReceiver.Presets.Add(PresetSlot, InOutState);
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

void FViscaLiveLinkSource::MaybeSendQueuedCommand(FViscaReceiverRuntime& InOutReceiver, const FIPv4Endpoint& SenderEndpoint)
{
	if (!InOutReceiver.Socket)
	{
		return;
	}

	// The current implementation mirrors packets as a basic request/response loop with retry.
	if (InOutReceiver.PendingOutCommand.IsEmpty())
	{
		return;
	}

	const double Now = FPlatformTime::Seconds();
	if (InOutReceiver.bAwaitingResponse && (Now - InOutReceiver.LastSendTime) < CommandRetrySeconds)
	{
		return;
	}

	if (InOutReceiver.RetryCount >= MaxRetryCount)
	{
		InOutReceiver.PendingOutCommand.Reset();
		InOutReceiver.RetryCount = 0;
		InOutReceiver.bAwaitingResponse = false;
		SourceStatus = LOCTEXT("SourceStatusRetryFailure", "Retry limit reached");
		return;
	}

	int32 BytesSent = 0;
	bool bSendOk = false;
	if (InOutReceiver.Config.Transport == EViscaReceiverTransportMode::TCP)
	{
		if (InOutReceiver.TcpAcceptedSocket)
		{
			bSendOk = InOutReceiver.TcpAcceptedSocket->Send(
				InOutReceiver.PendingOutCommand.GetData(),
				InOutReceiver.PendingOutCommand.Num(),
				BytesSent);
		}
	}
	else
	{
		TSharedRef<FInternetAddr> TargetAddr = SenderEndpoint.ToInternetAddr();
		bSendOk = InOutReceiver.Socket->SendTo(
			InOutReceiver.PendingOutCommand.GetData(),
			InOutReceiver.PendingOutCommand.Num(),
			BytesSent,
			*TargetAddr);
	}

	if (bSendOk)
	{
		InOutReceiver.LastSendTime = Now;
		InOutReceiver.RetryCount++;
		InOutReceiver.bAwaitingResponse = true;
	}
}

void FViscaLiveLinkSource::PublishConfiguredSubjects()
{
	for (FViscaReceiverRuntime& Receiver : Receivers)
	{
		EnsureSubject(Receiver);
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
