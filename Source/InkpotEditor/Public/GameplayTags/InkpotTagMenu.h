#pragma once

#include "Coreminimal.h"
#include "InkpotTagMenu.generated.h"

class UInkpotStoryAsset;
class UInkpotTagTable;
	

UCLASS()
class UInkpotTagMenu : public UObject 
{
GENERATED_BODY()

public:
	static void RegisterMenuExtensions(IModuleInterface *Owner);
	static void UnregisterMenuExtensions(IModuleInterface *Owner);
	static FToolMenuSection& CreateInkpotSection(UToolMenu* Menu);
	static void CreateOrUpdateInkpotTagAssetFromMenu(UToolMenu* Menu, TArray<FAssetData>& InAssets);

private:
	static void CreateTagTableAssets(const TArray<FAssetData>& SourceAssets);
	static bool CopyTagsFromStoryToTable(UInkpotStoryAsset* StoryAsset, UInkpotTagTable* TagTable);
	static UPackage* CreateTagTableAsset(const FAssetData& AssetData);
};
