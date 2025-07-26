#include "Inkpot/InkpotLibrary.h"
#include "Inkpot/InkpotStory.h"
#include "Utility/InkpotLog.h"

FString UInkpotLibrary::GetTagWithPrefixAndStrip(UInkpotStory* Story, const FString& TagPrefix)
{
	FString rval;
	if (Story)
	{
		const TArray<FString> &tags = Story->GetCurrentTags();
		for (const FString& tag : tags)
		{
			if (tag.StartsWith(TagPrefix))
			{
				rval = tag.Right( tag.Len() - TagPrefix.Len() );
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
