#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStories.h"
#include "Utility/InkpotSettings.h"
#include "Utility/InkpotLog.h"

UInkpot::UInkpot()
: Super()
{
	Stories = CreateDefaultSubobject<UInkpotStories>(TEXT("InkpotStories"));
	if( UInkpotSettings *settings = GetMutableDefault<UInkpotSettings>() )
	{
	}
}

void UInkpot::Initialize(FSubsystemCollectionBase&)
{
	Stories->Initialise();
}

UInkpotStory* UInkpot::BeginStory( UInkpotStoryAsset* InStory )
{
	UInkpotStory* story = Stories->BeginStory( InStory );
	if(EventOnStoryBegin.IsBound())
		EventOnStoryBegin.Broadcast( story );
	return story;
}

void UInkpot::EndStory(UInkpotStory *InStory)
{
	if(EventOnStoryEnd.IsBound())
		EventOnStoryEnd.Broadcast( InStory );
	Stories->EndStory( InStory );
}

