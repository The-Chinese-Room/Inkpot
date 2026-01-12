#include "Settings/InkpotSettings.h"
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


#if WITH_EDITOR
void UInkpotSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName propertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	if (propertyName == GET_MEMBER_NAME_CHECKED(UInkpotSettings, StoryFactoryClass))
	{
		UInkpot *inkpot = GEngine->GetEngineSubsystem<UInkpot>();
		inkpot->InitializeStoryFactory();
	}
}
#endif
