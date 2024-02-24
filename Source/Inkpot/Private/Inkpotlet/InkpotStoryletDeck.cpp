#include "Inkpotlet/InkpotStoryletDeck.h"

FInkpotletDeck::FInkpotletDeck(FName InName, TSharedPtr<Ink::FContainer> InValidator)
	: FInkpotlet(InName, InValidator)
{
	Content.Empty();
}
