#include "Asset/InkpotStoryAssetActions.h"
#include "InkpotEditorModule.h"
#include "Asset/InkpotStoryAsset.h"
#include "EditorFramework/AssetImportData.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"
	
FText FAssetTypeActions_InkpotStoryAsset::GetName() const
{
	return LOCTEXT( "AssetTypeActions_InkpotStoryAsset", "Inkpot Story" );
}

FColor FAssetTypeActions_InkpotStoryAsset::GetTypeColor() const
{
	return FColor( 175, 0, 128 );
}

UClass* FAssetTypeActions_InkpotStoryAsset::GetSupportedClass() const
{
	return UInkpotStoryAsset::StaticClass();
}

bool FAssetTypeActions_InkpotStoryAsset::IsImportedAsset() const
{
	return true;
}

uint32 FAssetTypeActions_InkpotStoryAsset::GetCategories()
{
	FInkpotEditorModule& module = FModuleManager::LoadModuleChecked<FInkpotEditorModule>("InkpotEditor");
	return module.GetAssetCategory();
}

void FAssetTypeActions_InkpotStoryAsset::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		UInkpotStoryAsset *InkpotStoryAsset = Cast<UInkpotStoryAsset>(Asset);
		if (!InkpotStoryAsset)
			continue;
		UAssetImportData* importData = InkpotStoryAsset->GetAssetImportData();
		if (!importData)
			continue;
		importData->ExtractFilenames(OutSourceFilePaths);
	}
}

#undef LOCTEXT_NAMESPACE
