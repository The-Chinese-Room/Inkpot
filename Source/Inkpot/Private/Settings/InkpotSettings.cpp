#include "Settings/InkpotSettings.h"

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
