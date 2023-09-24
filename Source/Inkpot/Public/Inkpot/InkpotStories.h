#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStory.h"
#include "InkpotStories.generated.h"

class UInkpotStoryAsset;

UCLASS()
class INKPOT_API UInkpotStories : public UObject
{
	GENERATED_BODY()

public:
	UInkpotStories();

	void Initialise();

	UInkpotStory* BeginStory( UInkpotStoryAsset* InkpotStoryAsset );
	void EndStory( UInkpotStory* InkpotStory );

private:
	UInkpotStory* GetStory( int32 InStoryHandle ) const;
	void EndStory(int32 InStoryHandle);

private:
	int32 NextStoryHandle{0};

	UPROPERTY()
	UInkpotStory *BadStory;

	UPROPERTY(Transient)
	TMap<int32, UInkpotStory*> Stories;
};
