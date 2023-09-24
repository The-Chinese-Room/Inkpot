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

