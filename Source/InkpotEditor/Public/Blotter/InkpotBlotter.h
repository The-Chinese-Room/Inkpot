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

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void FilterVariables( const TArray<UBlotterVariable*> &Variables, const FString &Filter, TArray<UBlotterVariable*> &ReturnValue );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	TArray<UBlotterVariable*> GetOrigins(UInkpotStory* Story);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Blotter", meta = (DisplayName = "OnDebugRefresh"))
	void ReceiveOnDebugRefresh(UInkpotStory* Story, bool bBuild );

	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Blotter", meta = (DisplayName = "OnDebugEnd"))
	void ReceiveOnDebugEnd();

	void Refresh(UInkpotStory* InStory);

protected:
	TArray<UBlotterString*> MakeDisplayStrings( const TArray<FString> &InStrings );

	UFUNCTION()
	void OnPIEStart(bool bStandard);

	UFUNCTION()
	void OnPIEEnd(bool bStandard);

protected:
	void BindInkpotStoryBegin();

	UFUNCTION()
	void OnStoryBegin(UInkpotStory* Story);

	UFUNCTION()
	void OnDebugRefresh(UInkpotStory* Story);
};