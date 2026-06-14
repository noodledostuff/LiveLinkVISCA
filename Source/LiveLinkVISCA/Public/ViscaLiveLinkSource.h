#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeCounter.h"
#include "HAL/ThreadSafeBool.h"
#include "ILiveLinkSource.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "ViscaLiveLinkSettings.h"

class FSocket;
class FRunnableThread;
class ILiveLinkClient;
class ISocketSubsystem;
class ULiveLinkSourceSettings;
struct FPropertyChangedEvent;

class LIVELINKVISCA_API FViscaLiveLinkSource
	: public ILiveLinkSource
	, public FRunnable
	, public TSharedFromThis<FViscaLiveLinkSource, ESPMode::ThreadSafe>
{
public:
	explicit FViscaLiveLinkSource(const FViscaLiveLinkConnectionSettings& InSettings);
	virtual ~FViscaLiveLinkSource() override;

	// ILiveLinkSource
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
	virtual void InitializeSettings(ULiveLinkSourceSettings* InSettings) override;
	virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override;
	virtual void OnSettingsChanged(ULiveLinkSourceSettings* InSettings, const FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void Update() override;
	virtual bool IsSourceStillValid() const override;
	virtual bool RequestSourceShutdown() override;
	virtual FText GetSourceType() const override { return SourceType; }
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }

	// FRunnable
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override {}

	void Start();

private:
	struct FViscaCameraState
	{
		float NormalizedZoom = 0.0f;
		float NormalizedFocus = 0.0f;
		float NormalizedIris = 0.0f;
		float PanNormalized = 0.5f;
		/** Maps to degrees via Lerp(-30, 90, x); 0.25 == 0° (level). */
		float TiltNormalized = 0.25f;
		/** Signed pan/tilt drive rates (VISCA speed 0–1); integrated in Update() for continuous motion. */
		float PanAxisVelocity = 0.0f;
		float TiltAxisVelocity = 0.0f;
		/** Signed zoom rate in normalized zoom units per second. */
		float ZoomAxisVelocity = 0.0f;
		/** Signed focus rate in normalized focus units per second. */
		float FocusAxisVelocity = 0.0f;
		FTransform CameraTransform = FTransform::Identity;
		double LastUpdateTime = 0.0;
		bool bAutoFocus = false;
		bool bAutoIris = false;
		bool bPowerOn = true;
		bool bMenuOpen = false;
		bool bColorBar = false;
		bool bAgc = false;
		bool bAutoShutter = false;
		bool bBacklightCompensation = false;
		bool bSpotlightCompensation = false;
		bool bDetailEnabled = false;
		bool bKneeEnabled = false;
		bool bVariableNdMode = false;
		bool bAutoNd = false;
		bool bNdFiltered = false;
		bool bTallyRed = false;
		bool bTallyGreen = false;
		bool bRecording = false;
		bool bAudioLevelAuto = false;
		uint8 NdPreset = 0;
		uint8 WhiteBalanceMode = 0x04;
		uint8 KneeMode = 0x00;
		uint8 AudioLevelChannel = 1;
		float NormalizedNd = 0.0f;
		float NormalizedAeLevel = 0.5f;
		float NormalizedDetailLevel = 0.5f;
		float NormalizedWhiteTemperature = 0.5f;
		float NormalizedRGain = 0.5f;
		float NormalizedBGain = 0.5f;
		float NormalizedAudioInputLevel = 0.5f;
	};

	struct FViscaReceiverRuntime
	{
		FViscaReceiverConfig Config;
		FSocket* Socket = nullptr;
		FSocket* TcpAcceptedSocket = nullptr;
		/** True when the listen/bind socket was created successfully. */
		bool bListenSocketOk = false;
		FName SubjectName;
		bool bStaticDataPushed = false;
		bool bViscaStaticDataPushed = false;
		FViscaCameraState State;
		TMap<uint8, FViscaCameraState> Presets;

		/** TCP only: reassemble stream into VISCA-over-IP frames (8-byte header + payload). */
		TArray<uint8> TcpReceiveBuffer;
		/** TCP only: full IP packets to send; drained on the receiver thread (same as Recv). */
		TArray<TArray<uint8>> PendingTcpIpPackets;
		FCriticalSection TcpSendLock;
	};

private:
	void RestartWithSettings(const FViscaLiveLinkConnectionSettings& InSettings);
	void RebuildSockets();
	void ShutdownSockets();
	void PublishConfiguredSubjects();
	void ClearSubjects();
	void PerformShutdownOnce();
	void RefreshSourceStatus();
	void EnsureSubject(FViscaReceiverRuntime& Receiver);
	void EnsureViscaSubject(FViscaReceiverRuntime& Receiver);
	void PushFrame(const FViscaReceiverRuntime& Receiver);
	void PushViscaFrame(const FViscaReceiverRuntime& Receiver);
	void ProcessIncomingPacket(
		int32 ReceiverRevisionAtDispatch,
		int32 ReceiverIndex,
		const TArray<uint8>& PacketData,
		const FIPv4Endpoint& SenderEndpoint,
		uint32 ViscaIpSequence,
		bool bViscaIpWrapped);
	bool ParseViscaPayload(const TArray<uint8>& PacketData, TArray<uint8>& OutViscaPayload) const;
	void ApplyViscaCommandToState(FViscaReceiverRuntime& InOutReceiver, const TArray<uint8>& CommandPayload);
	void UpdateTransformFromState(FViscaCameraState& InOutState);
	void QueueViscaIpReply(FViscaReceiverRuntime& Receiver, uint32 Sequence, const TArray<uint8>& ViscaPayload);
	void BuildViscaIpEnvelope(uint32 Sequence, const TArray<uint8>& ViscaPayload, TArray<uint8>& OutPacket) const;
	void SendViscaCommandResponses(
		FViscaReceiverRuntime& Receiver,
		const TArray<uint8>& CommandPayload,
		uint32 IpSequence,
		bool bUseIpEnvelope,
		const FIPv4Endpoint& UdpEndpoint,
		const FViscaCameraState& StateForInquiryReplies);
	void AppendInquiryCompletion(const TArray<uint8>& Inquiry, const FViscaCameraState& State, TArray<uint8>& OutVisca) const;
	static void AppendViscaNibblesU14(TArray<uint8>& Out, uint32 Value14);
	static void AppendViscaNibblesU16(TArray<uint8>& Out, uint32 Value16);
	void FlushPendingTcpSends(FViscaReceiverRuntime& Receiver);
	void ResetTcpClientConnection(FViscaReceiverRuntime& Receiver);
	static bool TryConsumeTcpViscaMessage(TArray<uint8>& InOutBuffer, TArray<uint8>& OutVisca, uint32& OutSeq, bool& bOutIpWrapped);
	static bool TryUnwrapUdpViscaIp(const TArray<uint8>& Datagram, TArray<uint8>& OutVisca, uint32& OutSeq, bool& bOutWrapped);

private:
	ILiveLinkClient* Client = nullptr;
	FGuid SourceGuid;

	FViscaLiveLinkConnectionSettings Settings;
	TArray<FViscaReceiverRuntime> Receivers;
	TArray<FName> CreatedSubjects;
	mutable FCriticalSection SubjectsLock;
	mutable FCriticalSection ShutdownMutex;
	FThreadSafeCounter ReceiverRevision;
	bool bHasPerformedShutdown = false;

	ISocketSubsystem* SocketSubsystem = nullptr;
	FRunnableThread* Thread = nullptr;
	FThreadSafeBool bIsRunning = false;
	FThreadSafeBool bShuttingDown = false;
	FTimespan WaitTime = FTimespan::FromMilliseconds(50.0);

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;

	/** Protects camera state + frame push paths used from the receiver thread and game-thread Update(). */
	mutable FCriticalSection CameraStateLock;
	double LastPtzIntegrateTimeSecs = 0.0;
};
