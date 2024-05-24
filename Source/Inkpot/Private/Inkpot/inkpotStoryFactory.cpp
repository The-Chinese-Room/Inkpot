#include "Inkpot/InkpotStoryFactory.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpot/InkpotStory.h"
#include "Asset/InkpotStoryAsset.h"
#include "Utility/InkpotLog.h"

static FString BadInkJSON{ TEXT("{\"inkVersion\":20,\"root\":[[\"^This is BAD Ink. If you see this your Ink did not import correctly.\",\"\n\",\"end\",[\"done\",{\"#n\":\"g-0\"}],null],\"done\",null],\"listDefs\":{}}") };

void UInkpotStoryFactoryBase::Initialise()
{
	ABadStory = NewObject<UInkpotStory>(this);
	ABadStory->Initialise( MakeShared<FInkpotStoryInternal>(BadInkJSON, FInkpotStoryInternal::BadStoryHandle) );
}

TSharedPtr<FInkpotStoryInternal> UInkpotStoryFactoryBase::CreateInternalStory(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle)
{
	TSharedPtr<FInkpotStoryInternal> storyInternal;
	if (InInkpotStoryAsset)
	{
		const FString& json = InInkpotStoryAsset->GetCompiledJSON();
		storyInternal = MakeShared<FInkpotStoryInternal>(json, InHandle);
	}
	else
	{
		INKPOT_ERROR("No story asset.");
	}
	return storyInternal;
}

UInkpotStory* UInkpotStoryFactoryBase::BadStory()
{
	return ABadStory;
}

UInkpotStory* UInkpotStoryFactory::CreateStory(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle, UObject *InOwner )
{ 
	TSharedPtr<FInkpotStoryInternal> storyInternal = CreateInternalStory( InInkpotStoryAsset, InHandle );
	if (!storyInternal->IsValidStory())
	{
		INKPOT_ERROR("Story asset is not a valid Ink story.");
		return ABadStory;
	}
	else
	{
		UInkpotStory* storyNew = NewObject<UInkpotStory>(InOwner);
		storyNew->Initialise(storyInternal);
		return storyNew;
	}
}
