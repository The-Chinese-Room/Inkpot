#include "InkpotEditorModule.h"
#include "Utility/InkpotLog.h"
#include "Asset/InkpotStoryAssetActions.h"
#include "AssetToolsModule.h"
#include "Framework/Commands/UIAction.h"
#include "LevelEditor.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FInkEditorModule"

void FInkpotEditorModule::StartupModule()
{
	InitAssetCategory();
	InitMenuOptions();
}

void FInkpotEditorModule::ShutdownModule()
{
}

void FInkpotEditorModule::InitAssetCategory()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedRef<IAssetTypeActions> Action = MakeShared<FAssetTypeActions_InkpotStoryAsset>();
	AssetTools.RegisterAssetTypeActions( Action );

	InkpotAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Inkpot")), LOCTEXT("Inkpot", "Inkpot"));
}

EAssetTypeCategories::Type FInkpotEditorModule::GetAssetCategory() const
{
	return InkpotAssetCategory;
}

void FInkpotEditorModule::InitMenuOptions()
{
	FLevelEditorModule& LevelEditorModule =	FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> MenuExtender = MakeShareable( new FExtender() );
	MenuExtender->AddMenuExtension(
		"Source Control",
		EExtensionHook::Before,
		nullptr,
		FMenuExtensionDelegate::CreateRaw( this, &FInkpotEditorModule::FillMenu )
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender( MenuExtender );
}

void FInkpotEditorModule::AddMenu(FMenuBarBuilder& MenuBuilder)
{
  MenuBuilder.AddPullDownMenu(
    LOCTEXT("InkpotLocMenuKey", "Inkpot"),
    LOCTEXT("InkpotLocMenuTip", "Opens menu for Inkpot"),
    FNewMenuDelegate::CreateRaw(this, &FInkpotEditorModule::FillMenu),
    FName(TEXT("Inkpot")),
    FName(TEXT("Inkpot")));
}

void FInkpotEditorModule::FillMenu(FMenuBuilder& MenuBuilder) 
{
	FUIAction Action( FExecuteAction::CreateStatic(&FInkpotEditorModule::OpenBlotter) );

	MenuBuilder.BeginSection(FName(TEXT("Inkpot")), LOCTEXT("InkpotLocMenuKey", "Inkpot") );

	MenuBuilder.AddMenuEntry(
		LOCTEXT( "InkpotLocBlotterKey", "Blotter" ),
		LOCTEXT( "InkpotLocBlotterTip", "Opens the blotter for debugging the current story." ),
		FSlateIcon(),
		Action
	);
}

void FInkpotEditorModule::OpenBlotter()
{
	const FSoftObjectPath widgetAssetPath(TEXT("/Inkpot/Inkpot/InkpotDebug.InkpotDebug"));
	UObject* widgetAssetLoaded = widgetAssetPath.TryLoad();
	if (widgetAssetLoaded == nullptr) 
	{
		INKPOT_WARN("Missing Expected widget class, cannot open blotter.");
		return;
	}
	UEditorUtilityWidgetBlueprint* widget = Cast<UEditorUtilityWidgetBlueprint>(widgetAssetLoaded);
	if (widget == nullptr) 
	{
		INKPOT_WARN("Blotter widget is not an UEditorUtilityWidgetBlueprint, cannot open blotter.");
		return;
	}
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EditorUtilitySubsystem->SpawnAndRegisterTab(widget);
}

IMPLEMENT_MODULE(FInkpotEditorModule, InkpotEditor)
#undef LOCTEXT_NAMESPACE
