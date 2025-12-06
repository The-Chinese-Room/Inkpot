#pragma once 

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InkpotPreferences.generated.h"

// Editor preferences for Inkpot
UCLASS(Config=EditorSettings, meta = (DisplayName="Inkpot Preferences"))
class INKPOT_API UInkpotPreferences : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/* 
	Generate gameplay tag table automatically on import or not.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Import")
	bool bAutogenerateGameplayTags{ false };

	/*
	Automatically update the GameplayTagTableList setting of teh Project GameplayTags section.
	(Tag files need to be added to this list before they can be used by any of the gamplay tag functionality of Inpot )
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Import", meta = (EditCondition = "bAutogenerateGameplayTags") )
	bool bAutoUpdateGameplayTagsList{ false };
	
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

	void PostInitProperties();

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif 
	
};
