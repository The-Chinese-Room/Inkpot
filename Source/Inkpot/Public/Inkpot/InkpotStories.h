#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotStoryHistory.h"
#include "InkpotStories.generated.h"

class UInkpotStoryAsset;

UCLASS()
class INKPOT_API UInkpotStories : public UObject
{
	GENERATED_BODY()

public:
	UInkpotStories();

	void Initialise();
	void Reset();

	UInkpotStory* BeginStory( UInkpotStoryAsset* InkpotStoryAsset );
	void EndStory( UInkpotStory* InkpotStory );

	UInkpotStory* Reload( UInkpotStoryAsset* InkpotStoryAsset );
	void Replay( UInkpotStory* InkpotStory, bool bInResetState = true );

private:
	UInkpotStory* GetStory( int32 InStoryHandle ) const;
	void EndStory(int32 InStoryHandle);

	TSharedPtr<FInkpotStoryInternal> CreateStoryInternal(UInkpotStoryAsset* InInkpotStoryAsset, int32 InHandle );

private:
	int32 NextStoryHandle{0};

	UPROPERTY()
	UInkpotStory *BadStory;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStory*> Stories;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStoryAsset*> StoryAssets;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStoryHistory*> StoryHistories;
};
