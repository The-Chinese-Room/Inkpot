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
#include "GameplayTags/InkpotTagMenu.h"

#define LOCTEXT_NAMESPACE "FInkEditorModule"

void FInkpotEditorModule::StartupModule()
{
	InitAssetCategory();
	InitMenuOptions();
	UInkpotTagMenu::RegisterMenuExtensions(this);
}

void FInkpotEditorModule::ShutdownModule()
{
	UInkpotTagMenu::UnregisterMenuExtensions(this);
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
	MenuBuilder.BeginSection(FName(TEXT("Inkpot")), LOCTEXT("InkpotLocMenuKey", "Inkpot") );

	;
	MenuBuilder.AddMenuEntry(
		LOCTEXT( "InkpotLocBlotterKey", "Blotter" ),
		LOCTEXT( "InkpotLocBlotterTip", "Opens the blotter for debugging the current story." ),
		FSlateIcon(),
		FUIAction( FExecuteAction::CreateStatic(&FInkpotEditorModule::OpenBlotter) )
	);

	;
	MenuBuilder.AddMenuEntry(
		LOCTEXT( "InkpotLocOutlinerKey", "Outliner" ),
		LOCTEXT( "InkpotLocOutlinerTip", "Opens the outliner for current story overview." ),
		FSlateIcon(),
		FUIAction( FExecuteAction::CreateStatic(&FInkpotEditorModule::OpenOutliner) )
	);

}

void FInkpotEditorModule::OpenEditorUtilityWidget( const FSoftObjectPath &InWidgetPath )
{
	UObject* widgetAssetLoaded = InWidgetPath.TryLoad();
	if (widgetAssetLoaded == nullptr) 
	{
		INKPOT_WARN("Missing Expected widget class, cannot open EditorUtilityWidget.");
		return;
	}
	UEditorUtilityWidgetBlueprint* widget = Cast<UEditorUtilityWidgetBlueprint>(widgetAssetLoaded);
	if (widget == nullptr) 
	{
		INKPOT_WARN("Widget is not an UEditorUtilityWidgetBlueprint, cannot open EditorUtilityWidget.");
		return;
	}
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EditorUtilitySubsystem->SpawnAndRegisterTab(widget);
}


void FInkpotEditorModule::OpenBlotter()
{
	FSoftObjectPath path( TEXT("/Inkpot/Inkpot/InkpotDebug.InkpotDebug") ); 
	OpenEditorUtilityWidget( path );
}

void FInkpotEditorModule::OpenOutliner()
{
	FSoftObjectPath path( TEXT("/Inkpot/Inkpot/InkpotOutliner.InkpotOutliner") ); 
	OpenEditorUtilityWidget( path );
}


IMPLEMENT_MODULE(FInkpotEditorModule, InkpotEditor)
#undef LOCTEXT_NAMESPACE
