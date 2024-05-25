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

	UFUNCTION(BlueprintCallable, Category=Inkpot )
	UInkpotStory* BeginStory( UInkpotStoryAsset* InkpotStoryAsset );

	UFUNCTION(BlueprintCallable, Category=Inkpot )
	void EndStory( UInkpotStory* InkpotStory );

	UPROPERTY(BlueprintAssignable, Category="Inkpot", meta=(DisplayName="On Story Begin") )
	FOnStoryBegin EventOnStoryBegin; 

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
};
