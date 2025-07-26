#include "GameplayTags/InkpotTagUtility.h"
#include "Engine/World.h"
#include "Misc/ScopedSlowTask.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "Asset/InkpotStoryAsset.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotValueLibrary.h"
#include "Inkpot/InkpotListLibrary.h"

bool UInkpotTagUtility::CopyTagsFromStoryToTable( UInkpotStoryAsset *InStoryAsset, UDataTable *TagTable)
{
	UInkpot *inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	UInkpotStory *story = inkpot->BeginStory(InStoryAsset);
	 
	TagTable->RowStruct = FGameplayTagTableRow::StaticStruct();
	int rowID = 0;

	bool result = false;
	result |= CopyOriginTagsToTable(story, TagTable, rowID);
	result |= CopyPathTagsToTable(story, TagTable,rowID);
	result |= CopyVariableTagsToTable(story, TagTable,rowID);

	inkpot->EndStory(story);

	return result;
}

bool UInkpotTagUtility::CopyOriginTagsToTable(UInkpotStory* InStory, UDataTable* OutTagTable, int &OutRowID)
{
	TSharedPtr<Ink::FListDefinitionsOrigin> definitions = InStory->GetStoryInternal()->GetListDefinitions();
	if (!definitions.IsValid())
		return false;

	TArray<TSharedPtr<Ink::FListDefinition>> lists = definitions->GetLists();
	for (auto &list : lists)
	{
		FString originName = list->GetName();
		const TMap<FString, int32>& items = list->GetItemNameToValues();
		for (auto& item : items)
		{
			FString sRowName = FString::Printf(TEXT("Row_%03d"), ++OutRowID);
			FName rowName(*sRowName);

			FGameplayTagTableRow row;
			FString sRow = FString::Printf(TEXT("%s%s.%s"), INK_ORIGIN_GAMEPLAYTAG_PREFIX, *originName, *item.Key);
			row.Tag = FName(*sRow);

			OutTagTable->AddRow(rowName, row);
		}
	}
	return true;
}

bool UInkpotTagUtility::CopyPathTagsToTable(UInkpotStory* InStory, UDataTable* OutTagTable, int &OutRowID)
{
	TSharedPtr<FInkpotStoryInternal>  story = InStory->GetStoryInternal();
	return CopyPathTagsToTable(story->GetMainContentContainer(),"", OutTagTable, OutRowID);
}

bool UInkpotTagUtility::CopyPathTagsToTable(TSharedPtr<Ink::FContainer> InSource, const FString &InRootName, UDataTable* OutTagTable, int &OutRowID)
{
	if(!InSource)
		return false;

	FString rootName;
		rootName = InRootName;
	
	FString containerName = InSource->GetName();
	if (rootName.IsEmpty())
		rootName = containerName;
	else if (containerName.Len() > 0)
		rootName += TEXT(".") + containerName;

	if (!rootName.IsEmpty() && !containerName.Equals(TEXT("global decl")) )
	{
		FString sRowName = FString::Printf(TEXT("Row_%03d"), ++OutRowID);
		FName rowName(*sRowName);

		FGameplayTagTableRow row;
		FString sRow = FString::Printf(TEXT("%s%s"), INK_PATH_GAMEPLAYTAG_PREFIX, *rootName);
		row.Tag = FName(*sRow);

		OutTagTable->AddRow(rowName, row);
	}
	
	TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> namedContentPtr = InSource->GetNamedContent();
	for( auto pair : *namedContentPtr )
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>( pair.Value );
		if(!container)
			continue;
		CopyPathTagsToTable( container, rootName, OutTagTable,OutRowID );
	}

	return true;
}

bool UInkpotTagUtility::CopyVariableTagsToTable(UInkpotStory* InStory, UDataTable* OutTagTable, int &OutRowID)
{
	TArray<FString> keys;
	int num = InStory->GetVariableKeys(keys);
	if (num == 0)
		return true; // perfectly ok for a story to have no variables 

	for (auto &key : keys )
	{
		FString sRowName = FString::Printf(TEXT("Row_%03d"), ++OutRowID);
		FName rowName(*sRowName);

		FGameplayTagTableRow row;
		FString sRow = FString::Printf(TEXT("%s%s"), INK_VARIABLE_GAMEPLAYTAG_PREFIX, *key);
		row.Tag = FName(*sRow);

		OutTagTable->AddRow(rowName, row);
	}
	
	return true;
}

UPackage* UInkpotTagUtility::CreateTagTableAsset(const FString &InName, const FString &InPath, UInkpotStoryAsset *InAsset)  
{
	FString AssetName = InName;
	AssetName.Append(TEXT("_TAGS"));
	FString AssetPath = InPath;
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
		UClass* AssetClass = UDataTable::StaticClass();
		const EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;
		Asset = NewObject<UObject>(Package, AssetClass, FName(*AssetName), Flags);
	}

	if ( Asset )
	{
		if ( NewAssetCreated )
			FAssetRegistryModule::AssetCreated(Asset);

		UDataTable* tableAsset = Cast<UDataTable>(Asset);
		const bool bSuccess = CopyTagsFromStoryToTable( InAsset, tableAsset );

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

	Package->MarkPackageDirty();
	return Package;
}

