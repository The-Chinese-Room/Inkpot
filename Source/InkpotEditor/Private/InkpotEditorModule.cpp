#include "InkpotEditorModule.h"
#include "AssetToolsModule.h"
#include "Asset/InkpotStoryAssetActions.h"

#define LOCTEXT_NAMESPACE "FInkEditorModule"

void FInkpotEditorModule::StartupModule()
{
	InitAssetCategory();

}

void FInkpotEditorModule::ShutdownModule()
{
}

void FInkpotEditorModule::InitAssetCategory()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedRef<IAssetTypeActions> Action = MakeShared<FAssetTypeActions_InkpotStoryAsset>();
	AssetTools.RegisterAssetTypeActions( Action );

	InkpotAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Inkpot")), LOCTEXT("Inkpot", "Inkpot"));
}

EAssetTypeCategories::Type FInkpotEditorModule::GetAssetCategory() const
{
	return InkpotAssetCategory;
}

IMPLEMENT_MODULE(FInkpotEditorModule, InkpotEditor)
#undef LOCTEXT_NAMESPACE

/*
DEFINE_LOG_CATEGORY( LogInkpotEditor )
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogInkEditor, Log, TEXT("Ink Editor Code Loaded"));

	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	{
		TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_StoryAsset);
		AssetTools.RegisterAssetTypeActions(Action);
	}

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	{
		FMessageLogInitializationOptions InitOptions;
		InitOptions.bAllowClear = true;
		InitOptions.bShowFilters = true;
		MessageLogModule.RegisterLogListing("InkCompiler", LOCTEXT("InkCompiler", "Ink Compiler"), InitOptions);
	}
*/

