#include "Settings/InkpotCVars.h"

bool FInkpotCVars::bDebugLog{true};
FAutoConsoleVariableRef CVarDebugLog(
	TEXT("Inkpot.DebugLog"),
	FInkpotCVars::bDebugLog,
	TEXT("Show Inkpot debug log or not."),
	ECVF_Default);

bool FInkpotCVars::bReloadIfAssetChanged{false};
FAutoConsoleVariableRef CVarReloadIfAssetChanged(
	TEXT("Inkpot.ReloadIfAssetChanged"),
	FInkpotCVars::bReloadIfAssetChanged,
	TEXT("Should the story be reloaded if the story asset has been updated at runtime."),
	ECVF_Default);

bool FInkpotCVars::bReplayIfReloaded{false};
FAutoConsoleVariableRef CVarReplayIfReloaded(
	TEXT("Inkpot.ReplayIfReloaded"),
	FInkpotCVars::bReplayIfReloaded,
	TEXT("Should Inkpot replay the story on reload."),
	ECVF_Default);

	