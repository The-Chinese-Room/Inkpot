#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "InkpotStoryMetaDataSimple.generated.h"

UCLASS()
class INKPOT_API UInkpotStoryMetaDataSimple : public UAssetUserData
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static UInkpotStoryMetaDataSimple* MakeInkpotStoryMetaDataSimple(FString SomeExtraData, FString EvenMoreExtraData);

	UFUNCTION(BlueprintPure)
	static void BreakInkpotStoryMetaDataSimple(UAssetUserData* target, FString &SomeExtraData, FString &EvenMoreExtraData);

	UPROPERTY(VisibleAnywhere)
	FString SomeExtraData;

	UPROPERTY(VisibleAnywhere)
	FString EvenMoreExtraData;
};
