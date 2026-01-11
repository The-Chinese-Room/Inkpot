#pragma once

#include "Coreminimal.h"
#include "EditorUtilityObject.h"
#include "IEditorUtilityExtension.h"
#include "Logging/MessageLog.h"
#include "InkpotImportPipeline.generated.h"

class UInkpotStoryAsset;

UCLASS(MinimalAPI, hideCategories = (Object), Blueprintable, Abstract )
class UInkpotImportPipeline : public UEditorUtilityObject, public IEditorUtilityExtension
{
	GENERATED_BODY()
public:
	UInkpotImportPipeline(const FObjectInitializer& ObjectInitializer);

	UInkpotStoryAsset* CreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFullFilePath, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled);

	/* Import
	* Event for processing of the ink import pipeline.
	* Passes in full file path of the source ink file with the newly created but empty story asset.
	* Intent is that after processing the source we get compiled JSON which is then set on the story asset.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Inkpot|Import")
	void Import(const FString& SourceFile, UInkpotStoryAsset *NewStoryAsset);

	/* Finalise 
	* Marks the import process as having succeeded.
	* If this is not called the import process will fail gracefully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Import")
	void Finalise();

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Import")
	void LogMsg(FString Message );

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Import")
	void LogMsgs(TArray<FString> Messages);

private:
	UPROPERTY()
	bool bFinalised{ false };
};

