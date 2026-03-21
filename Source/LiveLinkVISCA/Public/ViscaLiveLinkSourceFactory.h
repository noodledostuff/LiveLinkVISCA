#pragma once

#include "LiveLinkSourceFactory.h"
#include "ViscaLiveLinkSourceFactory.generated.h"

struct FViscaLiveLinkConnectionSettings;
class SViscaLiveLinkSourceEditorWidget;

UCLASS()
class LIVELINKVISCA_API UViscaLiveLinkSourceFactory : public ULiveLinkSourceFactory
{
	GENERATED_BODY()

public:
	virtual FText GetSourceDisplayName() const override;
	virtual FText GetSourceTooltip() const override;
	virtual EMenuType GetMenuType() const override { return EMenuType::MenuEntry; }
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;
};
