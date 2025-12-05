#pragma once

#include "Coreminimal.h"
#include "Inkpot/InkpotStory.h"
#include "InkpotTagUtility.generated.h"

class UDataTable;
class UInkpotStoryAsset;

UCLASS()
class UInkpotTagUtility : public UObject 
{
GENERATED_BODY()

public:
	static UPackage* CreateTagTable(const FString &Name, const FString &Path, UInkpotStoryAsset *InAsset);  

private:
	static UPackage* CreateTagTableAsset(const FString& Name, const FString& Path, UInkpotStoryAsset* InAsset);
	static void AddTableAssetToGameplayTagTableList(UPackage *Package);

	static bool CopyTagsFromStoryToTable(UInkpotStoryAsset* StoryAsset, UDataTable* TagTable);
	
	static bool CopyOriginTagsToTable(UInkpotStory* StoryAsset, UDataTable* TagTable, int &RowID);
	
	static bool CopyPathTagsToTable(UInkpotStory* StoryAsset, UDataTable* TagTable, int &RowID);
	static bool CopyPathTagsToTable(TSharedPtr<Ink::FContainer> InSource, const FString &InRootName, UDataTable* InTagTable, int &OutRowID);

	static bool CopyVariableTagsToTable(UInkpotStory* StoryAsset, UDataTable* TagTable, int &RowID);
};
