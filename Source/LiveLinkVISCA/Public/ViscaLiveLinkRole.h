// Copyright (c) 2026 Timothy YU. All rights reserved.

#pragma once

#include "Roles/LiveLinkCameraRole.h"

#include "ViscaLiveLinkRole.generated.h"

UCLASS(BlueprintType, meta = (DisplayName = "VISCA Camera Role"))
class LIVELINKVISCA_API ULiveLinkVISCACameraRole : public ULiveLinkCameraRole
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetStaticDataStruct() const override;
	virtual UScriptStruct* GetFrameDataStruct() const override;
	virtual UScriptStruct* GetBlueprintDataStruct() const override;
	virtual bool InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const override;
	virtual FText GetDisplayName() const override;
};
