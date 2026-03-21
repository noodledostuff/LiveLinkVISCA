#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceSettings.h"
#include "UObject/ObjectMacros.h"
#if WITH_EDITOR
struct FPropertyChangedEvent;
#endif
#include "ViscaLiveLinkSettings.generated.h"

/** VISCA-over-IP transport: UDP is listen-only; TCP uses connection + ACK/reply semantics. */
UENUM(BlueprintType)
enum class EViscaReceiverTransportMode : uint8
{
	UDP UMETA(DisplayName = "UDP (listen only)"),
	TCP UMETA(DisplayName = "TCP (with ACK)")
};

USTRUCT(BlueprintType)
struct FViscaReceiverConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "VISCA")
	FName CameraName;

	/** Enter port as text (e.g. 52381). */
	UPROPERTY(EditAnywhere, Category = "VISCA", meta = (DisplayName = "Port"))
	FString PortString = TEXT("52381");

	UPROPERTY(EditAnywhere, Category = "VISCA", meta = (DisplayName = "Transport"))
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

UCLASS()
class LIVELINKVISCA_API UViscaLiveLinkSourceSettings : public ULiveLinkSourceSettings
{
	GENERATED_BODY()

public:
	/** Dropdown: All interfaces (0.0.0.0), loopback, and local adapter IPv4s. */
	UPROPERTY(EditAnywhere, Category = "VISCA", meta = (GetOptions = "GetListenInterfaceOptions"))
	FString ListenInterfaceIp = TEXT("0.0.0.0");

	UPROPERTY(EditAnywhere, Category = "VISCA")
	TArray<FViscaReceiverConfig> Receivers;

	UFUNCTION()
	TArray<FString> GetListenInterfaceOptions() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
