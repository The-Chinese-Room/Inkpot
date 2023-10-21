#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"
#include "AssetRegistry/AssetRegistryModule.h"

class FInkpotEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	EAssetTypeCategories::Type GetAssetCategory() const;

private:
	void InitAssetCategory();

private:
	EAssetTypeCategories::Type InkpotAssetCategory;
};
