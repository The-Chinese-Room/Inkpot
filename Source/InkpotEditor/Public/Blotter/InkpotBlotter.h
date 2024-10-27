#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "BlotterVariable.h"
#include "InkpotBlotter.generated.h"


UCLASS(Abstract, meta = (ShowWorldContextPin), config = Editor)
class INKPOTEDITOR_API UInkpotBlotter : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	//UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	//FString GetCurrentFlowName(UInkpotStory* Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterString*> GetAliveFlowNames(UInkpotStory* Story);

	//UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	//FString currentText = GetCurrentText(UInkpotStory * Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const TArray<UBlotterString*> GetCurrentTags(UInkpotStory* Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterString*> GlobalTags(UInkpotStory* Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterString*> GetChoices(UInkpotStory* Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterVariable*> GetVariables(UInkpotStory* Story);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterVariable*> GetOrigins(UInkpotStory* Story);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Blotter", meta = (DisplayName = "OnDebugRefresh"))
	void ReceiveOnDebugRefresh(UInkpotStory* Story);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Blotter", meta = (DisplayName = "OnDebugEnd"))
	void ReceiveOnDebugEnd();

	void EnableDebugRefresh(bool bDoOnDebugRefresh);

protected:
	TArray<UBlotterString*> MakeDisplayStrings( const TArray<FString> &InStrings );
	void OnPIEEnd(UGameInstance* InGameInstance);

protected:
	void BindInkpotStoryBegin();

	UFUNCTION()
	void OnStoryBegin(UInkpotStory* Story);

	UFUNCTION()
	void OnDebugRefresh(UInkpotStory* Story);

private:
	bool bDoOnDebugRefresh {true};
};