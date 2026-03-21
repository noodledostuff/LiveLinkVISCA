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
	return MakeShared<FViscaLiveLinkSource>(Settings);
}

#undef LOCTEXT_NAMESPACE
