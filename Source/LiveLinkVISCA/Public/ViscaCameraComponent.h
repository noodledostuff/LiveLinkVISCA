#pragma once

#include "Components/ActorComponent.h"
#include "LiveLinkTypes.h"
#include "ViscaCameraComponent.generated.h"

class UCameraComponent;
class UCineCameraComponent;

UCLASS(ClassGroup = (VISCA), meta = (BlueprintSpawnableComponent))
class LIVELINKVISCA_API UViscaCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UViscaCameraComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "LiveLink")
	FLiveLinkSubjectName LiveLinkSubject;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	bool bTreatIncomingValuesAsNormalized = true;

private:
	float ApplyRange(float InValue, float InMin, float InMax) const;
	void ApplyToCamera(UCameraComponent* CameraComponent, UCineCameraComponent* CineCameraComponent, const struct FLiveLinkCameraStaticData* StaticData, const struct FLiveLinkCameraFrameData* FrameData) const;
};
