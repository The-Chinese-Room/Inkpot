#include "GameplayTags/InkpotTagMenu.h"
#include "Engine/World.h"
#include "Misc/ScopedSlowTask.h"
#include "ContentBrowserMenuContexts.h"
#include "Asset/InkpotStoryAsset.h"
#include "GameplayTags/InkpotTagUtility.h"

#define LOCTEXT_NAMESPACE "InkpotTagMenu"

void UInkpotTagMenu::RegisterMenuExtensions(IModuleInterface *InOwner)
{
	FToolMenuOwnerScoped OwnerScoped(InOwner);
	
	if (UToolMenu* WorldAssetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu"))
	{
		FToolMenuSection& Section = WorldAssetMenu->AddDynamicSection("Inkpot", FNewToolMenuDelegate::CreateLambda(
			[InOwner](UToolMenu* ToolMenu)
			{
				if (!GEditor || GEditor->GetPIEWorldContext() || !ToolMenu)
					return;

				if (UContentBrowserAssetContextMenuContext* AssetMenuContext = ToolMenu->Context.FindContext<UContentBrowserAssetContextMenuContext>())
				{
					CreateOrUpdateInkpotTagAssetFromMenu(ToolMenu, AssetMenuContext->SelectedAssets);
				}
			}
		), FToolMenuInsert(NAME_None, EToolMenuInsertType::Default));
	}
}

void UInkpotTagMenu::UnregisterMenuExtensions(IModuleInterface *InOwner)
{
	UToolMenus::UnregisterOwner(InOwner);
}

FToolMenuSection& UInkpotTagMenu::CreateInkpotSection(UToolMenu* Menu)
{
	const FName LevelSectionName = TEXT("Inkpot");
	FToolMenuSection* SectionPtr = Menu->FindSection(LevelSectionName);
	if (!SectionPtr)
		SectionPtr = &(Menu->AddSection(LevelSectionName, LOCTEXT("InkpotLabel", "Inkpot")));

	FToolMenuSection& Section = *SectionPtr;
	return Section;
}

void UInkpotTagMenu::CreateOrUpdateInkpotTagAssetFromMenu(UToolMenu* Menu, TArray<FAssetData>& InAssets)
{
	TArray<FAssetData> TempInkpotStoryAssets;
	for (const FAssetData& Asset : InAssets)
	{
		if (Asset.IsInstanceOf<UInkpotStoryAsset>())
			TempInkpotStoryAssets.Add(Asset);
	}

	if (TempInkpotStoryAssets.IsEmpty() )
		return;

	FToolMenuSection& Section = CreateInkpotSection(Menu);

	if (!TempInkpotStoryAssets.IsEmpty())
	{
		FToolUIAction UIAction;
		UIAction.ExecuteAction.BindLambda([inkpotStoryAssets = MoveTemp(TempInkpotStoryAssets)](const FToolMenuContext& MenuContext)
		{
			CreateTagTableAssets(inkpotStoryAssets);
		});

		Section.AddMenuEntry(
			"CreateInkpotTagsFromMenu",
			LOCTEXT("CreateInkpotTagsFromMenu", "Create Inkpot Tags from Inkpot Story Assets"),
			TAttribute<FText>(),
			FSlateIcon(),
			UIAction);
	}
}

UPackage* UInkpotTagMenu::CreateTagTableAsset(const FAssetData &InAssetData)
{
	FString AssetName = InAssetData.AssetName.ToString() ;
	AssetName.Append(TEXT("_TAGS"));
	FString AssetPath =  InAssetData.PackagePath.ToString();
	UInkpotStoryAsset* storyAsset = Cast<UInkpotStoryAsset>( InAssetData.GetAsset() );
	
	return UInkpotTagUtility::CreateTagTableAsset(AssetName, AssetPath, storyAsset);
}

void UInkpotTagMenu::CreateTagTableAssets(const TArray<FAssetData> &InSourceAssets )
{
	for( const FAssetData &assetData : InSourceAssets )
		UPackage* package = CreateTagTableAsset( assetData );
}

#undef LOCTEXT_NAMESPACE

