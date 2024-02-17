#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotStory.h"

UInkpotChoice::UInkpotChoice()
: Index( BadChoice ) 
{
}

void UInkpotChoice::Initialise( int32 InIndex, const FString &InString)
{
	UInkpotLine::Initialise( InString );
	Index = InIndex;
}

int32 UInkpotChoice::GetIndex() const
{
	return Index;
}

bool UInkpotChoice::operator == ( int32 InIndex )
{
	return Index == InIndex;
}
