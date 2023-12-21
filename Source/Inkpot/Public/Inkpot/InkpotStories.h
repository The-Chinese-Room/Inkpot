#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStory.h"
#include "InkpotStories.generated.h"

class UInkpotStoryAsset;

USTRUCT()
struct FInkpotStoryHistory
{
	GENERATED_BODY()

	UPROPERTY()
	int RandSeed; 

	UPROPERTY()
	TArray<int32> Choices; 

	UPROPERTY(Transient)
	bool bInReplay; 
};

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

	UFUNCTION()
	void OnStoryChoice( UInkpotStory* InStory, UInkpotChoice* InChoice );

private:
	int32 NextStoryHandle{0};

	UPROPERTY()
	UInkpotStory *BadStory;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStory*> Stories;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStoryAsset*> StoryAssets;

	UPROPERTY(Transient)
	TMap<int32, FInkpotStoryHistory> StoryHistories;
};
