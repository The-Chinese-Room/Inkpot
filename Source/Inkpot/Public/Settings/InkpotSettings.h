#pragma once 

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Inkpot/InkpotStoryFactory.h"
#include "InkpotCVars.h"
#include "InkpotSettings.generated.h"

UCLASS(Config=Inkpot, defaultconfig, meta = (DisplayName="Inkpot Settings"))
class INKPOT_API UInkpotSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/* 
	Show Inkpot debug log or not.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime")
	bool bDebugLog{ true };

	/* 
	Should the story be reloaded if the story asset has been updated at runtime.
	This will cause the story to reset.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime")
	bool bReloadIfAssetChanged{ false };

	/* 
	If a reload occurs should Inkpot replay the story.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime")
	bool bReplayIfReloaded{ false };

	/*
	Factory for creating an Inkpot story
	*/
	UPROPERTY(config, noclear, EditAnywhere, Category = "Setup", meta = (MetaClass = "/Script/Inkpot.InkpotStoryFactoryBase"))
	FSoftClassPath StoryFactoryClass {"/Script/Inkpot.InkpotStoryFactory"};

	void PostInitProperties();

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif 
};

