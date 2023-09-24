#include "InkPlusPlus.h"

#define LOCTEXT_NAMESPACE "FInkPlusPlusModule"
#include "Utility/InkPlusPlusLog.h"

void FInkPlusPlusModule::StartupModule()
{
}

void FInkPlusPlusModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInkPlusPlusModule, InkPlusPlus)