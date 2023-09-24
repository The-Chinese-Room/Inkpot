#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotStory.h"

UInkpotChoice::UInkpotChoice()
: Index( BadChoice ) 
{
}

void UInkpotChoice::Initialise( int32 InIndex, const FString &InString)
{
	Index = InIndex;
	String = InString;
}

int32 UInkpotChoice::GetIndex() const
{
	return Index;
}

const FString& UInkpotChoice::GetString() const
{
	return String;
}

UInkpotStory* UInkpotChoice::GetStory() const
{
	UObject *outer = GetOuter();
	return Cast<UInkpotStory>( outer );
}

bool UInkpotChoice::operator == ( int32 InIndex )
{
	return Index == InIndex;
}
