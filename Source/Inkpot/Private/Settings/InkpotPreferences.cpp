#include "Settings/InkpotPreferences.h"
#include "Settings/InkpotCVars.h"
#include "Inkpot/Inkpot.h"

FName UInkpotPreferences::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText UInkpotPreferences::GetSectionText() const
{
	return NSLOCTEXT("InkpotPlugin", "InkpotSettingsSection", "Inkpot");
}
#endif

void UInkpotPreferences::PostInitProperties()
{
	Super::PostInitProperties();

	FInkpotCVars::bDebugLog = bDebugLog;
	FInkpotCVars::bReloadIfAssetChanged = bReloadIfAssetChanged;
	FInkpotCVars::bReplayIfReloaded = bReplayIfReloaded;
}

#if WITH_EDITOR
void UInkpotPreferences::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName propertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotPreferences, bDebugLog))
	{
		FInkpotCVars::bDebugLog = bDebugLog;
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotPreferences, bReloadIfAssetChanged))
	{
		FInkpotCVars::bReloadIfAssetChanged = bReloadIfAssetChanged;
	}
	else if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotPreferences, bReplayIfReloaded))
	{
		FInkpotCVars::bReplayIfReloaded = bReplayIfReloaded;
	}
}
#endif
