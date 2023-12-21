#pragma once 

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "InkpotCVars.h"
#include "InkpotSettings.generated.h"

UCLASS(Config=Inkpot, defaultconfig, meta = (DisplayName="Inkpot Settings"))
class INKPOT_API UInkpotSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	/* 
	Show Inkpot debug log or not.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime", meta=(ConsoleVariable="Inkpot.DebugLog") )
	bool bDebugLog;

	/* 
	Should the story be reloaded if the story asset has been updated at runtime.
	This will cause the story to reset.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime", meta=(ConsoleVariable="Inkpot.ReloadIfAssetChanged") )
	bool bReloadIfAssetChanged;

	/* 
	If a reload occurs should Inkpot replay the story.
	*/
	UPROPERTY(Config, EditAnywhere, Category = "Runtime", meta=(EditCondition="bReloadIfAssetChanged", ConsoleVariable="Inkpot.ReplayIfReloaded") )
	bool bReplayIfReloaded;

	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif 
};

