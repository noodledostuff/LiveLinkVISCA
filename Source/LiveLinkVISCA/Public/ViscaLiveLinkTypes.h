// Copyright (c) 2026 Timothy YU. All rights reserved.

#pragma once

#include "Roles/LiveLinkCameraTypes.h"

#include "ViscaLiveLinkTypes.generated.h"

USTRUCT(BlueprintType)
struct LIVELINKVISCA_API FLiveLinkVISCACameraStaticData : public FLiveLinkCameraStaticData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bHasExtendedViscaState = true;
};

USTRUCT(BlueprintType)
struct LIVELINKVISCA_API FLiveLinkVISCACameraFrameData : public FLiveLinkCameraFrameData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bPowerOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bMenuOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bColorBar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAutoFocus = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAutoIris = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAgc = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAutoShutter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bBacklightCompensation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bSpotlightCompensation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bDetailEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bKneeEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bVariableNdMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAutoNd = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bNdFiltered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bTallyRed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bTallyGreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bRecording = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	bool bAudioLevelAuto = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	int32 NdPreset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	int32 WhiteBalanceMode = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	int32 KneeMode = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA")
	int32 AudioLevelChannel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedNd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedAeLevel = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedDetailLevel = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedWhiteTemperature = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedRGain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedBGain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink|VISCA", Interp)
	float NormalizedAudioInputLevel = 0.5f;
};

USTRUCT(BlueprintType)
struct LIVELINKVISCA_API FLiveLinkVISCACameraBlueprintData : public FLiveLinkBaseBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	FLiveLinkVISCACameraStaticData StaticData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LiveLink")
	FLiveLinkVISCACameraFrameData FrameData;
};
