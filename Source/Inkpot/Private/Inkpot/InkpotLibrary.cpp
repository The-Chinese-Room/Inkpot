#include "Inkpot/InkpotLibrary.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Asset/InkpotStoryAsset.h"
#include "Utility/InkpotLog.h"

FString UInkpotLibrary::GetTagWithPrefixAndStrip(UInkpotStory* InStory, const FString& InTagPrefix)
{
	FString rval;
	if (IsValid(InStory))
	{
		const TArray<FString> &tags = InStory->GetCurrentTags();
		for (const FString& tag : tags)
		{
			if (tag.StartsWith(InTagPrefix))
			{
				rval = tag.Right( tag.Len() - InTagPrefix.Len() );
				break;
			}
		}
	}
	else
	{
		INKPOT_ERROR("Story is not set");
	}
	return rval;
}

UInkpotStoryMetaData* UInkpotLibrary::GetMetaData(UInkpotStory* InStory)
{
	if (!IsValid(InStory))
	{
		INKPOT_ERROR("Story is invalid");
		return nullptr;
	}

	UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	UInkpotStoryAsset* asset = inkpot->GetStoryAsset(InStory);;
	if (!IsValid(asset))
	{
		INKPOT_ERROR("Story asset is invalid");
		return nullptr;
	}

	return asset->GetMetaData();
}
