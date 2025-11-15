#include "Inkpot/InkpotStories.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpot/InkpotStoryFactory.h"
#include "Settings/InkpotSettings.h"
#include "Asset/InkpotStoryAsset.h"
#include "Ink/Choice.h"
#include "Utility/InkpotLog.h"

UInkpotStories::UInkpotStories()
: Super()
{
}

void UInkpotStories::Initialise()
{
	InitialiseStoryFactory();
	Reset();
}

void UInkpotStories::InitialiseStoryFactory()
{
	const UInkpotSettings *settings = GetDefault<UInkpotSettings>();
	FSoftClassPath storyFactoryClassName = settings->StoryFactoryClass;
	UClass* storyFactoryClass = (storyFactoryClassName.IsValid() ? LoadObject<UClass>(NULL, *storyFactoryClassName.ToString() ) : UInkpotStoryFactory::StaticClass() );
	if (!storyFactoryClass)
	{
		INKPOT_ERROR("Inkpot story factory class is not set");
		return;
	}
	StoryFactory = NewObject<UInkpotStoryFactoryBase>(this, storyFactoryClass);
	StoryFactory->Initialise();
}


void UInkpotStories::Reset()
{
	StoryFactory->Reset();
	NextStoryHandle = 0;
	Stories.Empty();
	StoryAssets.Empty();
	StoryHistories.Empty();
}

UInkpotStory* UInkpotStories::BeginStory( UInkpotStoryAsset* InInkpotStoryAsset )
{
	if(!IsValid(InInkpotStoryAsset))
	{
		INKPOT_ERROR("Invalid Story Asset, could not begin story!");
		return StoryFactory->BadStory();
	}

	int32 handle;
	const int32 *keyptr = StoryAssets.FindKey( InInkpotStoryAsset );
	if(keyptr)
		handle = *keyptr;
	else
		handle = NextStoryHandle++;

	UInkpotStory* storyNew = StoryFactory->CreateStory(InInkpotStoryAsset, handle, this);

	if (storyNew->GetID() != FInkpotStoryInternal::BadStoryHandle)
	{
		Stories.Emplace(handle, storyNew);

		StoryAssets.Emplace(handle, InInkpotStoryAsset);

		UInkpotStoryHistory* history = NewObject<UInkpotStoryHistory>(storyNew);
		history->Initialise(storyNew);
		StoryHistories.Emplace(handle, history);
	}

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
		return StoryFactory->BadStory();
	}
	return Stories[InStoryHandle];
}

UInkpotStory* UInkpotStories::GetStory( TSoftObjectPtr<UInkpotStoryAsset>  InStoryAssetPath )
{
	FString inStoryAssetPath = InStoryAssetPath.ToString();
	for (auto pair : StoryAssets)
	{
		UInkpotStoryAsset* asset = pair.Value;
		FString assetPath = asset->GetPathName();
		if (inStoryAssetPath.Equals(assetPath))
		{
			int32 storyID = pair.Key;
			return GetStory(storyID);
		}
	}
	INKPOT_ERROR("Story '%s' is not running.", *inStoryAssetPath);
	return StoryFactory->BadStory();
}

UInkpotStory* UInkpotStories::Reload( UInkpotStoryAsset* InInkpotStoryAsset )
{
	const int32 *keyptr = StoryAssets.FindKey( InInkpotStoryAsset );
	if(!keyptr)
		return nullptr;
	int32 handle = *keyptr;

	TObjectPtr<UInkpotStory>* storyInProgressPtr = Stories.Find( handle );
	if(!storyInProgressPtr)
		return nullptr;

	TSharedPtr<FInkpotStoryInternal> storyInternal = StoryFactory->CreateInternalStory( InInkpotStoryAsset, handle );
	if(!storyInternal->IsValidStory())
		return nullptr;

	UInkpotStory *story = storyInProgressPtr->Get();
	story->ResetContent( storyInternal );
	return story;
}

void UInkpotStories::Replay( UInkpotStory* InStory, bool bInResetState )
{
	if(!InStory)
		return;

	int index = InStory->GetID();
	TObjectPtr<UInkpotStoryHistory>* history = StoryHistories.Find( index );
	if(!history)
		return;

	history->Get()->Replay( bInResetState );
}

TArray<UInkpotStory*> UInkpotStories::GetStories()
{
	TArray<UInkpotStory*> stories;
	Stories.GenerateValueArray(stories);;
	return stories;
}
