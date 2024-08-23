#include "Settings/InkpotSettings.h"
#include "Settings/InkpotCVars.h"
#include "Inkpot/Inkpot.h"

FName UInkpotSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText UInkpotSettings::GetSectionText() const
{
	return NSLOCTEXT("InkpotPlugin", "InkpotSettingsSection", "Inkpot");
}
#endif

void UInkpotSettings::PostInitProperties()
{
	Super::PostInitProperties();

	FInkpotCVars::bDebugLog = bDebugLog;
	FInkpotCVars::bReloadIfAssetChanged = bReloadIfAssetChanged;
	FInkpotCVars::bReplayIfReloaded = bReplayIfReloaded;
}

#if WITH_EDITOR
void UInkpotSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName propertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotSettings, bDebugLog))
	{
		FInkpotCVars::bDebugLog = bDebugLog;
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotSettings, bReloadIfAssetChanged))
	{
		FInkpotCVars::bReloadIfAssetChanged = bReloadIfAssetChanged;
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotSettings, bReplayIfReloaded))
	{
		FInkpotCVars::bReplayIfReloaded = bReplayIfReloaded;
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotSettings, StoryFactoryClass))
	{
		UInkpot *inkpot = GEngine->GetEngineSubsystem<UInkpot>();
		inkpot->InitializeStoryFactory();
	}
}
#endif
