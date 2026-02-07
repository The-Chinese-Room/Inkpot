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
	 * MapCurrentTagsWithDelimiter
	 * gets tags containing the delimiter from the current list of tags.
	 * if tags with the delimiter are found, returns them as a map, with the left  side
	 * of the delimiter as the key and the right side as the value.
	 *
	 * @param Story - The story to query.
	 * @param TagDelimiter - The delimiter to split on. Default is ": ".
	 * @returns A map of key:value pairs assembled from ink tags containing the delimiter.
	 */
	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	static INKPOT_API TMap<FName, FString> MapCurrentTagsWithDelimiter(UInkpotStory* Story, const FString& TagDelimiter);


	/* GetStoryAssetUserData
	* Get the asset user data instance (of class) from the story data asset used to create the story
	*/
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story", meta = (DeterminesOutputType = "Class"))
	static UAssetUserData* GetStoryAssetUserData(UInkpotStory* Story, UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UAssetUserData> Class);
};