// Copyright (c) 2026 Timothy YU. All rights reserved.

#include "ViscaLiveLinkRole.h"

#include "ViscaLiveLinkTypes.h"

#define LOCTEXT_NAMESPACE "LiveLinkVISCARole"

UScriptStruct* ULiveLinkVISCACameraRole::GetStaticDataStruct() const
{
	return FLiveLinkVISCACameraStaticData::StaticStruct();
}

UScriptStruct* ULiveLinkVISCACameraRole::GetFrameDataStruct() const
{
	return FLiveLinkVISCACameraFrameData::StaticStruct();
}

UScriptStruct* ULiveLinkVISCACameraRole::GetBlueprintDataStruct() const
{
	return FLiveLinkVISCACameraBlueprintData::StaticStruct();
}

bool ULiveLinkVISCACameraRole::InitializeBlueprintData(
	const FLiveLinkSubjectFrameData& InSourceData,
	FLiveLinkBlueprintDataStruct& OutBlueprintData) const
{
	FLiveLinkVISCACameraBlueprintData* BlueprintData = OutBlueprintData.Cast<FLiveLinkVISCACameraBlueprintData>();
	const FLiveLinkVISCACameraStaticData* StaticData = InSourceData.StaticData.Cast<FLiveLinkVISCACameraStaticData>();
	const FLiveLinkVISCACameraFrameData* FrameData = InSourceData.FrameData.Cast<FLiveLinkVISCACameraFrameData>();
	if (!BlueprintData || !StaticData || !FrameData)
	{
		return false;
	}

	GetStaticDataStruct()->CopyScriptStruct(&BlueprintData->StaticData, StaticData);
	GetFrameDataStruct()->CopyScriptStruct(&BlueprintData->FrameData, FrameData);
	return true;
}

FText ULiveLinkVISCACameraRole::GetDisplayName() const
{
	return LOCTEXT("VISCACameraRole", "VISCA Camera");
}

#undef LOCTEXT_NAMESPACE
