#pragma once

#include "AssetTypeActions_Base.h"

class INKPOTEDITOR_API FAssetTypeActions_InkpotStoryAsset : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual bool IsImportedAsset() const override;
	virtual uint32 GetCategories() override;
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
};
