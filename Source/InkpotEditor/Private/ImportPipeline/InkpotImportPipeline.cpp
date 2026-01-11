#include "ImportPipeline/InkpotImportPipeline.h"
#include "Asset/InkpotStoryAsset.h"


UInkpotImportPipeline::UInkpotImportPipeline(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UInkpotStoryAsset* UInkpotImportPipeline::CreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFullFilePath, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
	FEditorScriptExecutionGuard ScriptGuard;

	UInkpotStoryAsset *newAsset = NewObject<UInkpotStoryAsset>(InParent, InClass, InName, InFlags);
	newAsset->UpdateAssetInfo(InFullFilePath);

	Import( InFullFilePath, newAsset);

	if (!bFinalised)
	{
		bOutOperationCanceled = true;
		return nullptr;
	}
	else
	{
		return newAsset;
	}
}

void UInkpotImportPipeline::Finalise()
{
	bFinalised = true;
}

void UInkpotImportPipeline::LogMsg(FString Message)
{
	LogMsgs( TArray{ Message } );
}

void UInkpotImportPipeline::LogMsgs(TArray<FString> Messages)
{
	FMessageLog InkCompilerLog("InkpotImport");
	InkCompilerLog.Open( EMessageSeverity::Info );
	if (Messages.Num() != 0)
	{
		for (FString &message : Messages )
		{
			InkCompilerLog.Info( FText::FromString(message) );
		}
	}
}
