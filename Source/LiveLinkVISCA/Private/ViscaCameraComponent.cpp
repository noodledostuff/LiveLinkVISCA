#include "ViscaCameraComponent.h"

#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "Engine/World.h"
#include "Features/IModularFeatures.h"
#include "ILiveLinkClient.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

UViscaCameraComponent::UViscaCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
}

void UViscaCameraComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	bTickInEditor = bUpdateInEditor;
#endif
}

void UViscaCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void UViscaCameraComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property
		&& PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UViscaCameraComponent, bUpdateInEditor))
	{
		bTickInEditor = bUpdateInEditor;
	}
}
#endif

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

	if (const UWorld* World = GetWorld())
	{
		if (World->WorldType == EWorldType::EditorPreview && !bUpdateInPreviewEditor)
		{
			return;
		}
	}

	if (!bEvaluateLiveLink)
	{
		return;
	}

	if (bDisableEvaluateLiveLinkWhenSpawnable)
	{
		if (const AActor* Owner = GetOwner())
		{
			static const FName SequencerActorTag(TEXT("SequencerActor"));
			if (Owner->ActorHasTag(SequencerActorTag))
			{
				return;
			}
		}
	}

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
