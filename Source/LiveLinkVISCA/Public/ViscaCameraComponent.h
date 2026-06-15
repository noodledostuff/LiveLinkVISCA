// Copyright (c) 2026 Timothy YU. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "LiveLinkTypes.h"
#include "ViscaCameraComponent.generated.h"

class UCameraComponent;
class UCineCameraComponent;
#if WITH_EDITOR
struct FPropertyChangedEvent;
#endif

/**
 * Applies Live Link Camera-role data from a VISCA source to the owner's camera (Cine Camera recommended).
 * Evaluation and editor ticking options mirror Live Link Component Controller for consistent Sequencer and editor behavior.
 */
UCLASS(
	ClassGroup = (LiveLink),
	meta = (BlueprintSpawnableComponent, DisplayName = "Live Link VISCA Camera"))
class LIVELINKVISCA_API UViscaCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UViscaCameraComponent();

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	/** Live Link subject to read (must use the Camera role). */
	UPROPERTY(EditAnywhere, Category = "Live Link", meta = (DisplayName = "Subject Name"))
	FLiveLinkSubjectName LiveLinkSubject;

	/** When false, Live Link is not evaluated and the camera is not driven (paused). */
	UPROPERTY(
		EditAnywhere,
		Category = "Live Link",
		meta = (DisplayName = "Evaluate Live Link", ToolTip = "If false, does not evaluate Live Link (effectively pauses driving)."))
	bool bEvaluateLiveLink = true;

	/** When true, Sequencer spawnables (tagged SequencerActor) will not evaluate Live Link. */
	UPROPERTY(
		EditAnywhere,
		Category = "Live Link",
		meta = (DisplayName = "Disable Evaluate Live Link when Spawnable",
			ToolTip = "If true, does not evaluate Live Link when the owner is a Level Sequence spawnable."))
	bool bDisableEvaluateLiveLinkWhenSpawnable = true;

	/** When true, ticks in the editor so the viewport reflects incoming Live Link data. */
	UPROPERTY(
		EditAnywhere,
		Category = "Live Link",
		AdvancedDisplay,
		meta = (DisplayName = "Update in Editor", ToolTip = "When enabled, ticks in the editor (level viewport), not only during Play In Editor."))
	bool bUpdateInEditor = true;

	/** When true, ticks in Blueprint editor preview worlds. Requires Update in Editor. */
	UPROPERTY(
		EditAnywhere,
		Category = "Live Link",
		AdvancedDisplay,
		meta = (DisplayName = "Update in Preview Editor",
			EditCondition = "bUpdateInEditor",
			ToolTip = "If true, ticks when the world is an editor preview (for example Blueprint Class editor)."))
	bool bUpdateInPreviewEditor = false;

	/** When true, focal length, aperture, and focus distance are treated as normalized 0–1 and mapped to the Cine Camera lens range. */
	UPROPERTY(
		EditAnywhere,
		Category = "Camera Mapping",
		meta = (DisplayName = "Map Lens Values from 0–1"))
	bool bTreatIncomingValuesAsNormalized = true;

private:
	float ApplyRange(float InValue, float InMin, float InMax) const;
	void ApplyViscaExtendedData(
		UCameraComponent* CameraComponent,
		UCineCameraComponent* CineCameraComponent,
		const struct FLiveLinkVISCACameraFrameData* FrameData) const;
	void ApplyToCamera(
		UCameraComponent* CameraComponent,
		UCineCameraComponent* CineCameraComponent,
		const struct FLiveLinkCameraStaticData* StaticData,
		const struct FLiveLinkCameraFrameData* FrameData) const;
};
