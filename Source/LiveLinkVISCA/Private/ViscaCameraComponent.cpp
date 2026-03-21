#include "ViscaCameraComponent.h"

#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "Features/IModularFeatures.h"
#include "ILiveLinkClient.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"

UViscaCameraComponent::UViscaCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UViscaCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UViscaCameraComponent::ApplyRange(float InValue, float InMin, float InMax) const
{
	if (!bTreatIncomingValuesAsNormalized)
	{
		return FMath::Clamp(InValue, InMin, InMax);
	}

	return FMath::Lerp(InMin, InMax, FMath::Clamp(InValue, 0.0f, 1.0f));
}

void UViscaCameraComponent::ApplyToCamera(
	UCameraComponent* CameraComponent,
	UCineCameraComponent* CineCameraComponent,
	const FLiveLinkCameraStaticData* StaticData,
	const FLiveLinkCameraFrameData* FrameData) const
{
	if (!CameraComponent || !StaticData || !FrameData)
	{
		return;
	}

	if (StaticData->bIsRotationSupported)
	{
		GetOwner()->SetActorRotation(FrameData->Transform.GetRotation());
	}

	if (CineCameraComponent)
	{
		if (StaticData->bIsFocalLengthSupported)
		{
			CineCameraComponent->SetCurrentFocalLength(
				ApplyRange(
					FrameData->FocalLength,
					CineCameraComponent->LensSettings.MinFocalLength,
					CineCameraComponent->LensSettings.MaxFocalLength));
		}

		if (StaticData->bIsApertureSupported)
		{
			CineCameraComponent->CurrentAperture = ApplyRange(
				FrameData->Aperture,
				CineCameraComponent->LensSettings.MinFStop,
				CineCameraComponent->LensSettings.MaxFStop);
		}

		if (StaticData->bIsFocusDistanceSupported)
		{
			CineCameraComponent->FocusSettings.ManualFocusDistance = ApplyRange(
				FrameData->FocusDistance,
				CineCameraComponent->LensSettings.MinimumFocusDistance,
				100000.0f);
		}
	}
}

void UViscaCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LiveLinkSubject.Name.IsNone())
	{
		return;
	}

	if (!IModularFeatures::Get().IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName))
	{
		return;
	}

	ILiveLinkClient& LiveLinkClient = IModularFeatures::Get().GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
	FLiveLinkSubjectFrameData SubjectFrameData;
	if (!LiveLinkClient.EvaluateFrame_AnyThread(LiveLinkSubject, ULiveLinkCameraRole::StaticClass(), SubjectFrameData))
	{
		return;
	}

	const FLiveLinkCameraStaticData* StaticData = SubjectFrameData.StaticData.Cast<FLiveLinkCameraStaticData>();
	const FLiveLinkCameraFrameData* FrameData = SubjectFrameData.FrameData.Cast<FLiveLinkCameraFrameData>();
	if (!StaticData || !FrameData)
	{
		return;
	}

	UCameraComponent* CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	UCineCameraComponent* CineCameraComponent = Cast<UCineCameraComponent>(CameraComponent);
	ApplyToCamera(CameraComponent, CineCameraComponent, StaticData, FrameData);
}
