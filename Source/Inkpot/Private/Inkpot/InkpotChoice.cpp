#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotStory.h"

UInkpotChoice::UInkpotChoice()
{
}

void UInkpotChoice::Initialise( TSharedPtr<Ink::FChoice> InInkChoice )
{
	InkChoice = InInkChoice;
	UInkpotLine::Initialise( InkChoice->GetText() );
}

int32 UInkpotChoice::GetIndex() const
{
	if(InkChoice.IsValid())
		return InkChoice->GetIndex();
	else
		return BadChoice;
}

bool UInkpotChoice::operator == ( int32 InIndex )
{
	return GetIndex() == InIndex;
}

const TArray<FString> &UInkpotChoice::GetTagsInternal() const
{
	if(InkChoice.IsValid())
	{
		return InkChoice->GetTags();
	}
	else
	{
		static TArray<FString> BadTags;
		return BadTags;
	}
}
