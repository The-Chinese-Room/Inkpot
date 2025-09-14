#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"
#include "InkpotGameplayTagLibrary.generated.h"

// gameplay tag prefixes
#define INK_ORIGIN_GAMEPLAYTAG_PREFIX TEXT("Ink.Origin.")
#define INK_PATH_GAMEPLAYTAG_PREFIX TEXT("Ink.Path.")
#define INK_VARIABLE_GAMEPLAYTAG_PREFIX TEXT("Ink.Variable.")

// meta Categories filters, these will not work as macros; however defined here commented out for ease of search
//#define INK_ORIGIN_GAMEPLAYTAG_FILTER "Ink.Origin"
//#define INK_PATH_GAMEPLAYTAG_FILTER "Ink.Path"
//#define INK_VARIABLE_GAMEPLAYTAG_FILTER "Ink.Variable"

UCLASS(meta=(ScriptName="InkpotGameplayTagLibrary"), MinimalAPI)
class UInkpotGameplayTagLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static INKPOT_API FString OriginTagToString(FGameplayTag InTag );
	static INKPOT_API FString PathTagToString(FGameplayTag InTag);
	static INKPOT_API FString VariableTagToString(FGameplayTag InTag );

private:
	static FString TagToString(FGameplayTag InTag, const TCHAR* prefix);
};
