#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "InkpotStoryMetaDataSimple.generated.h"

UCLASS()
class INKPOT_API UInkpotStoryMetaDataSimple : public UAssetUserData
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Inkpot|StoryMetaData")
	static UInkpotStoryMetaDataSimple* MakeInkpotStoryMetaDataSimple(FString SomeExtraData, FString EvenMoreExtraData);

	UFUNCTION(BlueprintPure, Category = "Inkpot|StoryMetaData")
	static void BreakInkpotStoryMetaDataSimple(UAssetUserData* target, FString &SomeExtraData, FString &EvenMoreExtraData);

	UPROPERTY(VisibleAnywhere, Category = "Inkpot|StoryMetaData")
	FString SomeExtraData;

	UPROPERTY(VisibleAnywhere, Category = "Inkpot|StoryMetaData")
	FString EvenMoreExtraData;
};
