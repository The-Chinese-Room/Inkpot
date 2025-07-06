#include "GameplayTags/InkpotTagMenu.h"
#include "Engine/World.h"
#include "Misc/ScopedSlowTask.h"
#include "ContentBrowserMenuContexts.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "Asset/InkpotStoryAsset.h"
#include "GameplayTags/InkpotTagTable.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotValueLibrary.h"
#include "Inkpot/InkpotListLibrary.h"

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

bool UInkpotTagMenu::CopyTagsFromStoryToTable( UInkpotStoryAsset *InStoryAsset, UInkpotTagTable *TagTable)
{
	UInkpot *inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	UInkpotStory *story = inkpot->BeginStory(InStoryAsset);
	 
	TagTable->RowStruct = FGameplayTagTableRow::StaticStruct();

	TSharedPtr<Ink::FListDefinitionsOrigin> definitions = story->GetStoryInternal()->GetListDefinitions();
	if (definitions.IsValid())
	{
		int rowID = 0;

		TArray<TSharedPtr<Ink::FListDefinition>> lists = definitions->GetLists();
		for (auto &list : lists)
		{
			FString originName = list->GetName();
			const TMap<FString, int32>& items = list->GetItemNameToValues();
			for (auto& item : items)
			{
				FString sRowName = FString::Printf(TEXT("Row_%03d"), ++rowID);
				FName rowName(*sRowName);

				FGameplayTagTableRow row;
				FString sRow = FString::Printf(TEXT("%s%s.%s"), INKORIGIN_GAMEPLAYTAG_PREFIX, *originName, *item.Key);
				row.Tag = FName(*sRow);

				TagTable->AddRow(rowName, row);
			}
		}
	}

	inkpot->EndStory(story);

	return true;
}

UPackage* UInkpotTagMenu::CreateTagTableAsset(const FAssetData &InAssetData)
{
	FString AssetName = InAssetData.AssetName.ToString() ;
	AssetName.Append(TEXT("_TAGS"));
	FString AssetPath =  InAssetData.PackagePath.ToString();
	FString PackageName = FPaths::Combine(AssetPath, AssetName);

	FText Reason;
	if ( !FPackageName::IsValidObjectPath(PackageName, &Reason) )
		return nullptr;

	UPackage* Package = FPackageName::DoesPackageExist(PackageName) ? LoadPackage(nullptr, *PackageName, LOAD_None) : FindPackage(nullptr, *PackageName);

	UObject* Asset = nullptr;
	UObject* PreviousAsset = nullptr;
	bool NewAssetCreated = false;

	if ( Package )
	{
		PreviousAsset = FindObjectFast<UObject>(Package, *AssetName);
		if ( PreviousAsset )
		{
			PreviousAsset->SetFlags(RF_Transient);
			PreviousAsset->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
			NewAssetCreated = true;
		}
	}
	else
	{
		Package = CreatePackage(*PackageName);
		if ( Package )
			NewAssetCreated = true;
		else
			return nullptr;
	}

	if ( !Asset )
	{
		UClass* AssetClass = UInkpotTagTable::StaticClass();
		const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
		Asset = NewObject<UObject>(Package, AssetClass, FName(*AssetName), Flags);
	}

	if ( Asset )
	{
		if ( NewAssetCreated )
			FAssetRegistryModule::AssetCreated(Asset);

		UInkpotStoryAsset* storyAsset = Cast<UInkpotStoryAsset>( InAssetData.GetAsset() );
		UInkpotTagTable* tableAsset = Cast<UInkpotTagTable>(Asset);
		const bool bSuccess = CopyTagsFromStoryToTable( storyAsset, tableAsset );

		if ( PreviousAsset )
		{
			TMap<UObject*, UObject*> OldToNew;
			OldToNew.Emplace(PreviousAsset, Asset);
			GEngine->NotifyToolsOfObjectReplacement(OldToNew);
		}

		if ( !bSuccess )
		{
			return nullptr;
		}
	}

	// Save the file
	//if ( Package && Parameters.bSaveOnExportEnded )
	//{
	//FEditorFileUtils::PromptForCheckoutAndSave({ Package }, /*bCheckDirty=*/false, true);
	//}

	return Package;
}

void UInkpotTagMenu::CreateTagTableAssets(const TArray<FAssetData> &InSourceAssets )
{
	TArray<UPackage*> PackagesToSave;
	for( const FAssetData &assetData : InSourceAssets )
	{
		UPackage* package = CreateTagTableAsset( assetData );
		PackagesToSave.Add(package);
	}

	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, true);
}

#undef LOCTEXT_NAMESPACE

