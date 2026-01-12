#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InkpotLibrary.generated.h"

class UInkpotStory;
class UInkpotStoryMetaData;

UCLASS(meta = (BlueprintThreadSafe, ScriptName = "InkpotLibrary"))
class UInkpotLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * GetTagWithPrefix
	 * gets the first tag that starts with the specified prefix from the current list of tags.  
	 * if found, strips the tag and returns the rest
	 *
	 * @param Story - The story to query.
	 * @param TagPrefix - The prefix to match.
	 * @returns A string made from the first matching tag but excluding the prefix.
	 */
	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	static INKPOT_API FString GetTagWithPrefixAndStrip(UInkpotStory *Story, const FString& TagPrefix);


	/**
	 * GetMetaData
	 * gets the metadata associated with the story asset for this story.
	 *
	 * @param Story - The story to query.
	 * @param TagPrefix - The prefix to match.
	 * @returns A string made from the first matching tag but excluding the prefix.
	 */
	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	static INKPOT_API UInkpotStoryMetaData* GetMetaData(UInkpotStory* Story);
};