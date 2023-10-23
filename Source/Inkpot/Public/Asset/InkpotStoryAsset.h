#pragma once

#include "Coreminimal.h"
#include "Engine/DataAsset.h"
#include "InkpotStoryAsset.generated.h"

UCLASS()
class INKPOT_API UInkpotStoryAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UInkpotStoryAsset();

	void SetSource( const FString &InSource );
	const FString &GetSource() const;

	void SetCompiledJSON( const FString &InJSON );
	const FString& GetCompiledJSON() const;

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags( TArray<FAssetRegistryTag>& OutTags ) const override;
#endif
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FArchive& Ar) override;
	class UAssetImportData* GetAssetImportData();
	void UpdateAssetInfo( const FString &InFilename );
#endif

private:
	UPROPERTY( VisibleAnywhere, Category="Inkpot|StoryAsset" )
	FString Source;

	UPROPERTY( VisibleAnywhere, Category="Inkpot|StoryAsset" )
	FString JSON;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* AssetImportData;
#endif
};
