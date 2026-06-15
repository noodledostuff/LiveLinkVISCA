// Copyright (c) 2026 Timothy YU. All rights reserved.

#include "ViscaLiveLinkSourceFactory.h"

#include "ViscaLiveLinkSettings.h"
#include "ViscaLiveLinkSource.h"

#define LOCTEXT_NAMESPACE "ViscaLiveLinkSourceFactory"

FText UViscaLiveLinkSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "VISCA over IP");
}

FText UViscaLiveLinkSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Adds a Live Link source that listens for VISCA over IP and publishes Camera-role subjects.");
}

TSharedPtr<ILiveLinkSource> UViscaLiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const
{
	const FViscaLiveLinkConnectionSettings Settings = FViscaLiveLinkConnectionSettings::FromString(ConnectionString);
	TSharedRef<FViscaLiveLinkSource, ESPMode::ThreadSafe> Source = MakeShared<FViscaLiveLinkSource, ESPMode::ThreadSafe>(Settings);
	Source->Start();
	return Source;
}

#undef LOCTEXT_NAMESPACE
