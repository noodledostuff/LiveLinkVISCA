#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
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

class LIVELINKVISCA_API FViscaLiveLinkSource : public ILiveLinkSource, public FRunnable
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

private:
	struct FViscaCameraState
	{
		float NormalizedZoom = 0.0f;
		float NormalizedFocus = 0.0f;
		float NormalizedIris = 0.0f;
		float PanNormalized = 0.5f;
		float TiltNormalized = 0.5f;
		float PanSpeedNormalized = 0.0f;
		float TiltSpeedNormalized = 0.0f;
		FTransform CameraTransform = FTransform::Identity;
		double LastUpdateTime = 0.0;
		bool bAutoFocus = false;
		bool bAutoIris = false;
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
		FViscaCameraState State;
		TArray<uint8> PendingOutCommand;
		int32 RetryCount = 0;
		double LastSendTime = 0.0;
		bool bAwaitingResponse = false;
		TMap<uint8, FViscaCameraState> Presets;
	};

private:
	void Start();
	void RestartWithSettings(const FViscaLiveLinkConnectionSettings& InSettings);
	void RebuildSockets();
	void ShutdownSockets();
	void PublishConfiguredSubjects();
	void ClearSubjects();
	void PerformShutdownOnce();
	void RefreshSourceStatus();
	void EnsureSubject(FViscaReceiverRuntime& Receiver);
	void PushFrame(const FViscaReceiverRuntime& Receiver);
	void ProcessIncomingPacket(int32 ReceiverIndex, const TArray<uint8>& PacketData, const FIPv4Endpoint& SenderEndpoint);
	bool ParseViscaPayload(const TArray<uint8>& PacketData, TArray<uint8>& OutViscaPayload) const;
	void ApplyViscaCommandToState(FViscaReceiverRuntime& InOutReceiver, const TArray<uint8>& CommandPayload);
	void UpdateTransformFromState(FViscaCameraState& InOutState);
	void MaybeSendQueuedCommand(FViscaReceiverRuntime& InOutReceiver, const FIPv4Endpoint& SenderEndpoint);

private:
	ILiveLinkClient* Client = nullptr;
	FGuid SourceGuid;

	FViscaLiveLinkConnectionSettings Settings;
	TArray<FViscaReceiverRuntime> Receivers;
	TArray<FName> CreatedSubjects;
	mutable FCriticalSection SubjectsLock;
	mutable FCriticalSection ShutdownMutex;
	bool bHasPerformedShutdown = false;

	ISocketSubsystem* SocketSubsystem = nullptr;
	FRunnableThread* Thread = nullptr;
	FThreadSafeBool bIsRunning = false;
	FThreadSafeBool bShuttingDown = false;
	FTimespan WaitTime = FTimespan::FromMilliseconds(50.0);

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;
};
