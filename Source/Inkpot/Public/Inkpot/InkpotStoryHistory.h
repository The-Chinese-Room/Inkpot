#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStory.h"
#include "InkpotStoryHistory.generated.h"

UENUM()
enum class EInkpotStoryEventType : uint8 
{
	None,
	Continue,
	Choice,
	SwitchFlow,
	ChoosePath
};


USTRUCT()
struct FInkpotStoryHistoryEvent
{
	GENERATED_BODY()

	FInkpotStoryHistoryEvent();
	FInkpotStoryHistoryEvent(EInkpotStoryEventType InType, const FString &InData );
	FInkpotStoryHistoryEvent(EInkpotStoryEventType InType, int InChoice );

	UPROPERTY()
	EInkpotStoryEventType Type;

	UPROPERTY()
	int Choice; 

	UPROPERTY()
	FString Data; 
};


UCLASS()
class UInkpotStoryHistory : public UObject 
{
	GENERATED_BODY()

public:
	void Initialise( UInkpotStory* Story );
	void Replay( bool bInResetState );

	UFUNCTION()
	void OnStoryContinue( UInkpotStory* Story );

	UFUNCTION()
	void OnStoryChoice( UInkpotStory* Story, UInkpotChoice* Choice );

	UFUNCTION()
	void OnStorySwitchFlow( UInkpotStory* Story, const FString& Flow );

	UFUNCTION()
	void OnStoryChoosePath( UInkpotStory* Story, const FString& Path );

private:
	UPROPERTY()
	int RandSeed; 

	UPROPERTY()
	TArray<FInkpotStoryHistoryEvent> Events; 

	UPROPERTY(Transient)
	bool bInReplay; 
};

