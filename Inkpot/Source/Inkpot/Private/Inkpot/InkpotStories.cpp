#include "Inkpot/InkpotStories.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Asset/InkpotStoryAsset.h"
#include "Ink/Choice.h"
#include "Utility/InkpotLog.h"

static FString BadInkJSON { TEXT("{\"inkVersion\":20,\"root\":[[\"^This is BAD Ink. If you see this your Ink did not import correctly.\",\"\n\",\"end\",[\"done\",{\"#n\":\"g-0\"}],null],\"done\",null],\"listDefs\":{}}") };

UInkpotStories::UInkpotStories()
: Super()
{
	BadStory = CreateDefaultSubobject<UInkpotStory>(TEXT("BadInkStory"));
}

void UInkpotStories::Initialise()
{
	BadStory->Initialise( MakeShared<FInkpotStoryInternal>( BadInkJSON , FInkpotStoryInternal::BadStoryHandle ) );
}

UInkpotStory* UInkpotStories::BeginStory( UInkpotStoryAsset* InStory )
{
	if(!InStory)
	{
		INKPOT_ERROR("No story asset.");
		return BadStory;
	}

	const FString &json = InStory->GetCompiledJSON();
	TSharedRef<FInkpotStoryInternal> storyInternal = MakeShared<FInkpotStoryInternal>( json, NextStoryHandle );
	if(!storyInternal->IsValidStory())
	{
		INKPOT_ERROR("Story asset is not a valid Ink story.");
		return BadStory;
	}

	UInkpotStory *storyNew = NewObject<UInkpotStory>(this);
	storyNew->Initialise( storyInternal );
	Stories.Emplace( NextStoryHandle, storyNew );
	NextStoryHandle++;

	return storyNew;
}

void UInkpotStories::EndStory(UInkpotStory *InStory)
{
	if(!InStory)
		return;
	EndStory( InStory->GetID() );
}

void UInkpotStories::EndStory(int32 InStoryHandle)
{
	if(!Stories.Contains(InStoryHandle))
		return;
	Stories.Remove( InStoryHandle );
}

UInkpotStory* UInkpotStories::GetStory( int32 InStoryHandle ) const
{
	if(!Stories.Contains(InStoryHandle))
	{
		INKPOT_ERROR("Story ID does not exist.");
		return BadStory;
	}
	return Stories[InStoryHandle];
}

