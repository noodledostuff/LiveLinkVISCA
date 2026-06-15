// Copyright (c) 2026 Timothy YU. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceSettings.h"
#include "UObject/ObjectMacros.h"
#if WITH_EDITOR
struct FPropertyChangedEvent;
#endif
#include "ViscaLiveLinkSettings.generated.h"

/** VISCA over IP transport mode. */
UENUM(BlueprintType)
enum class EViscaReceiverTransportMode : uint8
{
	/** Stateless datagram listener; no session or command replies. */
	UDP UMETA(DisplayName = "UDP (Listen)"),
	/** Connection-oriented; supports VISCA-over-IP ACK and reply traffic. */
	TCP UMETA(DisplayName = "TCP (Session + Replies)")
};

USTRUCT(BlueprintType)
struct FViscaReceiverConfig
{
	GENERATED_BODY()

	/** Name of the Live Link subject published for this receiver (shown in Live Link UI). */
	UPROPERTY(EditAnywhere, Category = "VISCA Source", meta = (DisplayName = "Subject Name"))
	FName CameraName;

	/** TCP/UDP port this receiver binds to (default VISCA over IP: 52381). */
	UPROPERTY(EditAnywhere, Category = "VISCA Source", meta = (DisplayName = "Listen Port"))
	FString PortString = TEXT("52381");

	UPROPERTY(EditAnywhere, Category = "VISCA Source", meta = (DisplayName = "Transport"))
	EViscaReceiverTransportMode Transport = EViscaReceiverTransportMode::UDP;
};

struct LIVELINKVISCA_API FViscaLiveLinkConnectionSettings
{
public:
	static FViscaLiveLinkConnectionSettings FromString(const FString& InConnectionString);
	FString ToString() const;

	/** Bind address: use 0.0.0.0 for all interfaces, or a specific host IPv4. */
	FString ListenInterfaceIp = TEXT("0.0.0.0");

	TArray<FViscaReceiverConfig> Receivers;
};

UCLASS(meta = (DisplayName = "VISCA over IP Source Settings"))
class LIVELINKVISCA_API UViscaLiveLinkSourceSettings : public ULiveLinkSourceSettings
{
	GENERATED_BODY()

public:
	/** Local IPv4 to bind, or 0.0.0.0 for all interfaces. */
	UPROPERTY(EditAnywhere, Category = "VISCA Source", meta = (GetOptions = "GetListenInterfaceOptions", DisplayName = "Bind Address"))
	FString ListenInterfaceIp = TEXT("0.0.0.0");

	/** One Live Link subject per entry; each must use a unique port. */
	UPROPERTY(EditAnywhere, Category = "VISCA Source", meta = (DisplayName = "Receivers"))
	TArray<FViscaReceiverConfig> Receivers;

	UFUNCTION()
	TArray<FString> GetListenInterfaceOptions() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
