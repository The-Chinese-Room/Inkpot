#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Inkpot.generated.h"

class UInkpotStory;
class UInkpotStories;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryBegin, UInkpotStory*, Story );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryEnd,   UInkpotStory*, Story );

UCLASS()
class INKPOT_API UInkpot : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UInkpot();

	virtual void Initialize(FSubsystemCollectionBase&) override;
	void InitializeStoryFactory();

	void Register();

	/**
	 * Begin a story
	 * Many stories can run concurrently, generally there will only be one.
	 *
	 * @param InkpotStoryAsset - The UAsset created from importing an Ink script. 
	 * @return The runtime story.
	 */
	UFUNCTION(BlueprintCallable, Category=Inkpot )
	UInkpotStory* BeginStory( UInkpotStoryAsset* InkpotStoryAsset );

	/**
	 * Get a story
	 * Story will need to have been started with BeginStory.
	 *
	 * @param InkpotStoryAsset - Soft object reference to the story asset. 
	 * @return The runtime story.
	 */
	UFUNCTION(BlueprintCallable, Category = Inkpot)
	UInkpotStory* GetStory( TSoftObjectPtr<UInkpotStoryAsset> InkpotStoryAsset );

	/**
	 * End a story
	 * Story will need to have been started with BeginStory.
	 *
	 * @param InkpotStory - The runtime story to stop. 
	 */
	UFUNCTION(BlueprintCallable, Category=Inkpot )
	void EndStory( UInkpotStory* InkpotStory );

	/**
	 * Delegate for story begin.
	 * Called when BeginStory successfully starts a story.
	 */
	UPROPERTY(BlueprintAssignable, Category="Inkpot", meta=(DisplayName="On Story Begin") )
	FOnStoryBegin EventOnStoryBegin; 

	/**
	 * Delegate for end.
	 * Called when EndStory successfully stops a story.
	 */
	UPROPERTY(BlueprintAssignable, Category="Inkpot", meta=(DisplayName="On Story End") )
	FOnStoryBegin EventOnStoryEnd; 

private:
	void BindPostImport();
#if WITH_EDITOR
	void OnAssetPostImport(UFactory* InFactory, UObject* ActorAdded);
#endif 
	void OnStartGameInstance( UGameInstance *InInstance );

private:
	UPROPERTY()
	UInkpotStories *Stories;

	UPROPERTY()
	TWeakObjectPtr<UGameInstance> GameInstance{ nullptr };
};
