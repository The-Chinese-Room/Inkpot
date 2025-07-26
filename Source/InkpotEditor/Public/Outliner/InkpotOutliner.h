#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Inkpot/InkpotStory.h"
#include "InkpotOutliner.generated.h"

class UBlotterContainer;

UCLASS(Abstract, meta = (ShowWorldContextPin), config = Editor)
class INKPOTEDITOR_API UInkpotOutliner : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Blotter", meta = (DisplayName = "OnStoryDisplay"))
	void ReceiveOnStoryDisplay(UBlotterContainer *StoryRoot);

protected:
	void BindInkpotStoryBegin();
	UBlotterContainer* BuildContainers(UInkpotStory* Story);
	void BuildContainers(UBlotterContainer *View, const FString &RootName, TSharedPtr<Ink::FContainer> Source, int32 Level );

	UFUNCTION()
	void OnStoryBegin(UInkpotStory* Story);
};