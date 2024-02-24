#pragma once

#include "Coreminimal.h"
#include "InkpotStorylet.h"

struct FInkpotletDeck : public FInkpotlet
{
	FInkpotletDeck(FName Name, TSharedPtr<Ink::FContainer> Validator);
	TArray<FInkpotlet> Content;
};
