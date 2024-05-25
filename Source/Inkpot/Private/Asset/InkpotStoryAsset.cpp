#include "Asset/InkpotStoryAsset.h"
#include "EditorFramework/AssetImportData.h"

UInkpotStoryAsset::UInkpotStoryAsset()
: Super()
{
}

void UInkpotStoryAsset::SetSource( const FString &InSource )
{
	Source = InSource;
}

const FString &UInkpotStoryAsset::GetSource() const
{
	return Source;
}

void UInkpotStoryAsset::SetCompiledJSON( const FString &InJSON )
{
	JSON = InJSON;
}

const FString& UInkpotStoryAsset::GetCompiledJSON() const
{
	return JSON;
}

#if WITH_EDITORONLY_DATA
UAssetImportData* UInkpotStoryAsset::GetAssetImportData()
{
	return AssetImportData;
}
#endif

#if WITH_EDITORONLY_DATA
void UInkpotStoryAsset::UpdateAssetInfo( const FString &InFilename )
{
	if (!AssetImportData)
		return;
	AssetImportData->Update(InFilename);
}
#endif

#if WITH_EDITOR
void UInkpotStoryAsset::PostInitProperties()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
	Super::PostInitProperties();
}
#endif

#if WITH_EDITOR
void UInkpotStoryAsset::GetAssetRegistryTags( FAssetRegistryTagsContext InContext ) const
{
	if (AssetImportData)
	{
		InContext.AddTag( FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden) );
	}
	Super::GetAssetRegistryTags( InContext );
}
#endif

#if WITH_EDITORONLY_DATA
void UInkpotStoryAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() && Ar.UEVer() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON && !AssetImportData)
	{
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
}
#endif

