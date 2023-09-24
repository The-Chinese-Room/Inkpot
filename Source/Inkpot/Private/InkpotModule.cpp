#include "InkpotModule.h"
#include "ISettingsModule.h"
#include "Utility/InkpotSettings.h"

#define LOCTEXT_NAMESPACE "InkpotModule"

IMPLEMENT_MODULE(FInkpotModule, Inkpot)

void FInkpotModule::StartupModule()
{
	RegisterSettings();
}

void FInkpotModule::ShutdownModule()
{
}

void FInkpotModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Inkpot",
			LOCTEXT("RuntimeSettingsName", "Inkpot"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Ink Pot plugin"),
			GetMutableDefault<UInkpotSettings>()
			);
	}
}

#undef LOCTEXT_NAMESPACE 

