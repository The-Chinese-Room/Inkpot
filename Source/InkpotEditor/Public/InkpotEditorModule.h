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
	void InitMenuOptions();

	UFUNCTION()
	void AddMenu( FMenuBarBuilder& MenuBuilder );

	UFUNCTION()
	void FillMenu( FMenuBuilder& MenuBuilder );

	UFUNCTION()
	static void OpenBlotter();

	UFUNCTION()
	static void OpenOutliner();

	static void OpenEditorUtilityWidget( const FSoftObjectPath &WidgetPath );

private:
	EAssetTypeCategories::Type InkpotAssetCategory;
};
