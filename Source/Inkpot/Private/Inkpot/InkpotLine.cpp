#include "Inkpot/InkpotLine.h"
#include "Inkpot/InkpotStory.h"

UInkpotLine::UInkpotLine()
{
}

void UInkpotLine::Initialise(const FString &InString)
{
	String = InString;
	// TODO : Localisation support here
	Text = FText::FromString(String);
}

const FString& UInkpotLine::GetString() const
{
	return String;
}

UInkpotStory* UInkpotLine::GetStory() const
{
	UObject *outer = GetOuter();
	return Cast<UInkpotStory>( outer );
}

const FText& UInkpotLine::GetText() const
{
	return Text;
}
