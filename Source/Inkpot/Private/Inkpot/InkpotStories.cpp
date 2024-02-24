#include "Inkpot/InkpotStories.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpotlet/InkpotStoryletDeckSet.h"
#include "Asset/InkpotStoryAsset.h"
#include "Ink/Choice.h"
#include "Utility/InkpotLog.h"
#include "Utility/InkpotUtility.h"

static FString BadInkJSON { TEXT("{\"inkVersion\":20,\"root\":[[\"^This is BAD Ink. If you see this your Ink did not import correctly.\",\"\n\",\"end\",[\"done\",{\"#n\":\"g-0\"}],null],\"done\",null],\"listDefs\":{}}") };

UInkpotStories::UInkpotStories()
: Super()
{
	BadStory = CreateDefaultSubobject<UInkpotStory>(TEXT("BadInkStory"));
}

void UInkpotStories::Initialise()
{
	BadStory->Initialise( MakeShared<FInkpotStoryInternal>( BadInkJSON , FInkpotStoryInternal::BadStoryHandle ) );
	Reset();
}

void UInkpotStories::Reset()
{
	NextStoryHandle = 0;
	Stories.Empty();
	StoryAssets.Empty();
	StoryHistories.Empty();
}

TSharedPtr<FInkpotStoryInternal> UInkpotStories::CreateStoryInternal(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle )
{
	if(!InInkpotStoryAsset)
	{
		INKPOT_ERROR("No story asset.");
		return nullptr;
	}

	const FString &json = InInkpotStoryAsset->GetCompiledJSON();
	return MakeShared<FInkpotStoryInternal>( json, InHandle );
}

UInkpotStory* UInkpotStories::BeginStory( UInkpotStoryAsset* InInkpotStoryAsset )
{
	int32 handle;
	const int32 *keyptr = StoryAssets.FindKey( InInkpotStoryAsset );
	if(keyptr)
		handle = *keyptr;
	else
		handle = NextStoryHandle++;

	TSharedPtr<FInkpotStoryInternal> storyInternal = CreateStoryInternal( InInkpotStoryAsset, handle );
	if(!storyInternal->IsValidStory())
	{
		INKPOT_ERROR("Story asset is not a valid Ink story.");
		return BadStory;
	}

	UInkpotStory* storyNew = nullptr;
	TArray<FString> tags = storyInternal->GlobalTags();
	if (InkpotUtil::GetTag(tags, "InkpotletSet"))
		storyNew = NewObject<UInkpotletSet>(this);
	else
		storyNew = NewObject<UInkpotStory>(this);

	storyNew->Initialise( storyInternal );

	Stories.Emplace( handle, storyNew );

	StoryAssets.Emplace( handle, InInkpotStoryAsset );

	UInkpotStoryHistory *history = NewObject<UInkpotStoryHistory>( storyNew );
	history->Initialise( storyNew );
	StoryHistories.Emplace( handle, history );

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
	StoryAssets.Remove( InStoryHandle );
	StoryHistories.Remove( InStoryHandle );
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

UInkpotStory* UInkpotStories::Reload( UInkpotStoryAsset* InInkpotStoryAsset )
{
	const int32 *keyptr = StoryAssets.FindKey( InInkpotStoryAsset );
	if(!keyptr)
		return nullptr;
	int32 handle = *keyptr;

	UInkpotStory **storyInProgressPtr = Stories.Find( handle );
	if(!storyInProgressPtr)
		return nullptr;

	TSharedPtr<FInkpotStoryInternal> storyInternal = CreateStoryInternal( InInkpotStoryAsset, handle );
	if(!storyInternal->IsValidStory())
		return nullptr;

	UInkpotStory *story = *storyInProgressPtr;
	story->ResetContent( storyInternal );
	return story;
}

void UInkpotStories::Replay( UInkpotStory* InStory, bool bInResetState )
{
	if(!InStory)
		return;

	int index = InStory->GetID();
	UInkpotStoryHistory **history = StoryHistories.Find( index );
	if(!history)
		return;

	(*history)->Replay( bInResetState );
}
