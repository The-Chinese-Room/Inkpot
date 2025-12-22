#include "Asset/InkpotStoryAssetFactory.h"
#include "Asset/InkpotStoryAsset.h"
#include "Misc/FileHelper.h"
#include "Compiler/InkCompiler.h"
#include "Logging/MessageLog.h"
#include "EditorFramework/AssetImportData.h"
#include "GameplayTags/InkpotTagUtility.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Settings/InkpotPreferences.h"
#include "Settings/InkpotSettings.h"
#include "ImportPipeline/InkpotImportPipeline.h"
#include "ImportPipeline/InkpotImportPipelineLibrary.h"

#define LOCTEXT_NAMESPACE "InkpotStoryAssetFactory"

UInkpotStoryAssetFactory::UInkpotStoryAssetFactory()
{
	Formats.Add(FString(TEXT("ink;")) + LOCTEXT("FormatTxt", "Ink Story File").ToString());
	SupportedClass = UInkpotStoryAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bAutomatedReimport = true;
}

bool UInkpotStoryAssetFactory::FactoryCanImport(const FString& Filename)
{
	return Filename.EndsWith("ink");
}

FSoftClassPath UInkpotStoryAssetFactory::GetCustomPiplineClass()  const
{
	const UInkpotSettings* settings = GetDefault<UInkpotSettings>();
	return settings->CustomImportClass;
}

bool UInkpotStoryAssetFactory::IsUsingCustomPipeline() const
{
	return GetCustomPiplineClass().IsValid();
}

UObject* UInkpotStoryAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFullFilePath, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
	const FString fileExtension = FPaths::GetExtension(InFullFilePath);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *fileExtension);

	UInkpotStoryAsset* newAsset = nullptr;
	if( IsUsingCustomPipeline() )
		newAsset = ExecuteCustomImportPipeline(InClass, InParent, InName, InFlags, InFullFilePath, InParms, InWarn, bOutOperationCanceled);
	else 
		newAsset = ExecuteStandardImportPipeline(InClass, InParent, InName, InFlags, InFullFilePath, InParms, InWarn, bOutOperationCanceled);

	if (!newAsset)
		bOutOperationCanceled = true;
	else
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, newAsset);

	return newAsset;
}

UInkpotStoryAsset* UInkpotStoryAssetFactory::ExecuteStandardImportPipeline(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFullFilePath, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
	UInkpotStoryAsset* newAsset = nullptr;

	FString inkStory;
	FString inkJSON;
	if (LoadAndCompileStory(InFullFilePath, inkStory, inkJSON))
	{
		newAsset = NewObject<UInkpotStoryAsset>(InParent, InClass, InName, InFlags);
		newAsset->SetSource(inkStory);
		newAsset->SetCompiledJSON(inkJSON);
		newAsset->UpdateAssetInfo(InFullFilePath);
		GenerateTAGs(InParent, newAsset);
	}

	return newAsset;
}

UInkpotStoryAsset* UInkpotStoryAssetFactory::ExecuteCustomImportPipeline(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFullFilePath, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
	FSoftClassPath importPipelineClassName = GetCustomPiplineClass();
	UClass* importPipelineClass = importPipelineClassName.IsValid() ? LoadObject<UClass>(NULL, *importPipelineClassName.ToString()) : nullptr;
	if (!importPipelineClass)
		return nullptr;

	UInkpotImportPipeline* importPipeline = NewObject<UInkpotImportPipeline>( InParent, importPipelineClass );
	if (!importPipeline)
		return nullptr;

	UInkpotStoryAsset* newAsset = importPipeline->CreateFile(InClass, InParent, InName, InFlags, InFullFilePath, InParms, InWarn, bOutOperationCanceled);

	return newAsset;
}

void UInkpotStoryAssetFactory::GenerateTAGs( UObject* InParent, UInkpotStoryAsset *InStoryAsset ) const
{
	bool bSuccess;
	UInkpotImportPipelineLibrary::GenerateTAGs(InStoryAsset, bSuccess);
}

bool UInkpotStoryAssetFactory::LoadAndCompileStory( const FString& InFilename, FString &OutStory, FString &OutCompiledStory) const
{
	if ( !FFileHelper::LoadFileToString( OutStory, *InFilename ) )
		return false;
	bool bSuccess;
	UInkpotImportPipelineLibrary::CompileInkFile(InFilename, bSuccess, OutCompiledStory);
	return bSuccess;
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

