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
	SetDirty(true);
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

const TArray<FString> &UInkpotLine::GetTags() const
{
	return GetTagsInternal();
}

const TArray<FString> &UInkpotLine::GetTagsInternal() const
{
	UInkpotStory* story = GetStory();
	return story->GetCurrentTags();
}

bool UInkpotLine::IsDirty()
{
	return bIsDirty;
}

void UInkpotLine::SetDirty(bool bInIsDirty)
{
	bIsDirty = bInIsDirty;
}
