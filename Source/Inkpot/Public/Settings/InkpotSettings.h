#pragma once 

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InkpotSettings.generated.h"

// Runtime settings for Inkpot
UCLASS(Config=Inkpot, defaultconfig, meta = (DisplayName="Inkpot Settings"))
class INKPOT_API UInkpotSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/*
	Factory for creating an Inkpot story
	*/
	UPROPERTY(config, noclear, EditAnywhere, Category = "Setup", meta = (MetaClass = "/Script/Inkpot.InkpotStoryFactoryBase"))
	FSoftClassPath StoryFactoryClass {"/Script/Inkpot.InkpotStoryFactory"};

	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

};

