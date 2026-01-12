#pragma once

#include "CoreMinimal.h"
#include "Asset/InkpotStoryMetaData.h"
#include "InkpotStoryMetaDataSimple.generated.h"

UCLASS()
class INKPOT_API UInkpotStoryMetaDataSimple : public UInkpotStoryMetaData
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	static UInkpotStoryMetaDataSimple* MakeInkpotStoryMetaDataSimple(FString SomeExtraData);

	UFUNCTION(BlueprintPure)
	static void BreakInkpotStoryMetaDataSimple(UInkpotStoryMetaData* target, FString &SomeExtraData);

	UPROPERTY(VisibleAnywhere)
	FString SomeExtraData;
};
