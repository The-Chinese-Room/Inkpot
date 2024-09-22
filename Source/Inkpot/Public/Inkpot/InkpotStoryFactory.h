#pragma once

#include "CoreMinimal.h"
#include "InkpotStoryFactory.generated.h"

class UInkpotStoryAsset;
class UInkpotStory;
class FInkpotStoryInternal;

UCLASS(abstract)
class INKPOT_API UInkpotStoryFactoryBase : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialise();

	virtual UInkpotStory* CreateStory(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle, UObject* InOwner) PURE_VIRTUAL(UInkpotStoryFactoryBase::CreateStory, return ABadStory; );
	TSharedPtr<FInkpotStoryInternal> CreateInternalStory(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle);

	UInkpotStory* BadStory();

	void Reset();

protected:
	UPROPERTY()
	UInkpotStory* ABadStory;
};


UCLASS()
class INKPOT_API UInkpotStoryFactory : public UInkpotStoryFactoryBase
{
	GENERATED_BODY()

public:
	UInkpotStory* CreateStory(UInkpotStoryAsset *InInkpotStoryAsset, int32 InHandle, UObject *InOwner);
};





