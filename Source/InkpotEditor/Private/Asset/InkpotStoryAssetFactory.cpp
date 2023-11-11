#include "Asset/InkpotStoryAssetFactory.h"
#include "Asset/InkpotStoryAsset.h"
#include "Misc/FileHelper.h"
#include "Compiler/InkCompiler.h"
#include "Logging/MessageLog.h"
#include "EditorFramework/AssetImportData.h"

#define LOCTEXT_NAMESPACE "InkpotStoryAssetFactory"

UInkpotStoryAssetFactory::UInkpotStoryAssetFactory()
{
	Formats.Add( FString(TEXT("ink;")) + LOCTEXT("FormatTxt", "Ink Story File").ToString() );
	SupportedClass = UInkpotStoryAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bAutomatedReimport = true;
}

bool UInkpotStoryAssetFactory::FactoryCanImport( const FString& Filename )
{
	return Filename.EndsWith("ink");
}

UObject* UInkpotStoryAssetFactory::FactoryCreateFile( UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled )
{
	const FString fileExtension = FPaths::GetExtension(InFilename);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *fileExtension);

	UInkpotStoryAsset *newObject = nullptr;
	FString inkStory;
	FString inkJSON;
	if( LoadAndCompileStory( InFilename, inkStory, inkJSON ) )
	{
		newObject = NewObject<UInkpotStoryAsset>( InParent, InClass, InName, InFlags );
		newObject->SetSource( inkStory );
		newObject->SetCompiledJSON( inkJSON );
		newObject->UpdateAssetInfo( InFilename );
	}

	if(!newObject)
		bOutOperationCanceled = true;

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport( this, newObject );

	return newObject;
}

bool UInkpotStoryAssetFactory::LoadAndCompileStory( const FString& InFilename, FString &OutStory, FString &OutCompiledStory) const
{
	if ( !FFileHelper::LoadFileToString( OutStory, *InFilename ) )
		return false;

	const FString InkScratchFilePath = InkCompiler::GetScratchDirectory() + TEXT("TempInkFile.ink");
		
	TArray<FString> errors;
	TArray<FString> warnings;
	InkCompiler::FlushScratchDirectory();

	if (InkCompiler::CompileInkFile(InFilename, InkScratchFilePath, OutCompiledStory, errors, warnings))
	{
		FMessageLog InkCompilerLog("InkCompiler");

		InkCompilerLog.Open(EMessageSeverity::Error);
		if (warnings.Num() != 0)
		{
			for (int i = 0; i < warnings.Num(); i++)
			{
				InkCompilerLog.Warning(FText::FromString(warnings[i]));
			}
		}

		if ( errors.Num() == 0 )
			return true;

		for ( int i = 0; i < errors.Num(); i++ )
			InkCompilerLog.Error( FText::FromString( errors[ i ] ) );
	}

	return false;
}

bool UInkpotStoryAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UInkpotStoryAsset* InkpotStoryAsset = Cast<UInkpotStoryAsset>(Obj);
	if (!InkpotStoryAsset)
		return false;
	UAssetImportData *importData = InkpotStoryAsset->GetAssetImportData();
	if (!importData)
		return false;
	importData->ExtractFilenames(OutFilenames);
	return true;
}

void UInkpotStoryAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UInkpotStoryAsset* InkpotStoryAsset  = Cast<UInkpotStoryAsset>(Obj);
	if (!InkpotStoryAsset)
		return;
	UAssetImportData *importData = InkpotStoryAsset->GetAssetImportData();
	if (!importData)
		return;
	if (!ensure(NewReimportPaths.Num() == 1))
		return;
	importData->UpdateFilenameOnly(NewReimportPaths[0]);
}

EReimportResult::Type UInkpotStoryAssetFactory::Reimport(UObject* Obj)
{
	UInkpotStoryAsset* InkpotStoryAsset  = Cast<UInkpotStoryAsset>(Obj);
	if (InkpotStoryAsset  != nullptr)
	{
		UAssetImportData* importData = InkpotStoryAsset->GetAssetImportData();
		if ( importData )
		{
			bool bOperationCancelled = false;
			if (ImportObject(InkpotStoryAsset ->GetClass(), InkpotStoryAsset ->GetOuter(), *InkpotStoryAsset->GetName(), RF_Public | RF_Standalone, importData->GetFirstFilename(), nullptr, bOperationCancelled) != nullptr)
			{
				return EReimportResult::Succeeded;
			}
		}
	}

	return EReimportResult::Failed;
}

int32 UInkpotStoryAssetFactory::GetPriority() const
{
	return ImportPriority;
}

#undef LOCTEXT_NAMESPACE
