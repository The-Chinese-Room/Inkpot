#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotStoryHistory.h"
#include "InkpotStories.generated.h"

class UInkpotStoryAsset;
class UInkpotStoryFactoryBase;

UCLASS()
class INKPOT_API UInkpotStories : public UObject
{
	GENERATED_BODY()

public:
	UInkpotStories();

	void Initialise();
	void InitialiseStoryFactory();
	void Reset();

	UInkpotStory* BeginStory( UInkpotStoryAsset* InkpotStoryAsset );
	void EndStory( UInkpotStory* InkpotStory );

	UInkpotStory* Reload( UInkpotStoryAsset* InkpotStoryAsset );
	void Replay( UInkpotStory* InkpotStory, bool bInResetState = true );

	UInkpotStory* GetStory( TSoftObjectPtr<UInkpotStoryAsset> InkpotStoryAssetPath );

	TArray<UInkpotStory*> GetStories();

private:
	UInkpotStory* GetStory( int32 InStoryHandle ) const;
	void EndStory(int32 InStoryHandle);


private:
	int32 NextStoryHandle{0};

	UPROPERTY(Transient)
	TMap<int32, UInkpotStory*> Stories;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStoryAsset*> StoryAssets;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStoryHistory*> StoryHistories;

	UPROPERTY(Transient)
	UInkpotStoryFactoryBase *StoryFactory;
};
