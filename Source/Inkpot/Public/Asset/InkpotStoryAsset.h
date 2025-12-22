#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InkpotStoryAsset.generated.h"

UCLASS(BlueprintType)
class INKPOT_API UInkpotStoryAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UInkpotStoryAsset();

	/* SetSource
	* Set the source contents of the Asset, this is not used by the runtime and can be left blank.
	*/
	UFUNCTION( BlueprintCallable )
	void SetSource( const FString &Source );

	/* GetSource
	* returns the source contents of the Asset.
	*/
	UFUNCTION(BlueprintPure)
	const FString &GetSource() const;

	/* SetCompiledJSON
	* Sets the JSON content of the Asset.
	* This is required for the story to be loadable by the Ink runtime.
	*/
	UFUNCTION( BlueprintCallable )
	void SetCompiledJSON( const FString &JSON );

	/* GetCompiledJSON
	* Gets the JSON content of the Asset.
	*/
	UFUNCTION(BlueprintPure)
	const FString& GetCompiledJSON() const;

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags( FAssetRegistryTagsContext Context ) const override;
#endif
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FStructuredArchiveRecord Record) override;
	class UAssetImportData* GetAssetImportData();
	void UpdateAssetInfo( const FString &Filename );
#endif

private:
	UPROPERTY( VisibleAnywhere, Category="Inkpot|StoryAsset" )
	FString Source;

	UPROPERTY( VisibleAnywhere, Category="Inkpot|StoryAsset" )
	FString JSON;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	TObjectPtr<class UAssetImportData> AssetImportData;
#endif
};
