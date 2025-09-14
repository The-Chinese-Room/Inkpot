#include "Inkpot/InkpotGameplayTagLibrary.h"

FString UInkpotGameplayTagLibrary::TagToString(FGameplayTag InTag, const TCHAR *prefix )
{
	FString sTag = InTag.ToString();
	sTag.RemoveFromStart(prefix);;
	return sTag;
}

FString UInkpotGameplayTagLibrary::OriginTagToString(FGameplayTag InTag )
{
	return TagToString( InTag, INK_ORIGIN_GAMEPLAYTAG_PREFIX );
}

FString UInkpotGameplayTagLibrary::PathTagToString(FGameplayTag InTag )
{
	return TagToString( InTag, INK_PATH_GAMEPLAYTAG_PREFIX );
}

FString UInkpotGameplayTagLibrary::VariableTagToString(FGameplayTag InTag )
{
	return TagToString( InTag, INK_PATH_GAMEPLAYTAG_PREFIX );
}
