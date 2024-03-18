#include "Ink/Choice.h"
#include "Ink/Path.h"
#include "Ink/CallStack.h"

FString Ink::FChoice::GetText() const
{
	return Text;
}

void Ink::FChoice::SetText(const FString& InText)
{
	Text = InText;
}

FString Ink::FChoice::GetSourcePath() const
{
	return SourcePath;
}

void Ink::FChoice::SetSourcePath(const FString& InSourcePath)
{
	SourcePath = InSourcePath;
}

TSharedPtr<Ink::FPath> Ink::FChoice::GetTargetPath() const
{
	return TargetPath;
}

void Ink::FChoice::SetTargetPath(TSharedPtr<FPath> InTargetPath)
{
	TargetPath = InTargetPath;
}

int32 Ink::FChoice::GetIndex() const
{
	return Index;
}

void Ink::FChoice::SetIndex(int32 InIndex)
{
	Index = InIndex;
}

int32 Ink::FChoice::GetOriginalThreadIndex() const
{
	return OriginalThreadIndex;
}

void Ink::FChoice::SetOriginalThreadIndex(int32 InOriginalThreadIndex)
{
	OriginalThreadIndex = InOriginalThreadIndex;
}

bool Ink::FChoice::IsInvisibleDefault() const
{
	return bIsInvisibleDefault;
}

void Ink::FChoice::SetIsInvisibleDefault(bool InIsInvisibleDefault)
{
	bIsInvisibleDefault = InIsInvisibleDefault;
}

FString Ink::FChoice::GetPathStringOnChoice() const
{
	return TargetPath->ToString();
}

void Ink::FChoice::SetPathStringOnChoice(const FString& InPathStringOnChoice)
{
	SetTargetPath(MakeShared<FPath>(InPathStringOnChoice));
}

TSharedPtr<Ink::FThread> Ink::FChoice::GetThreadAtGeneration() const
{
	return ThreadAtGeneration;
}

void Ink::FChoice::SetThreadAtGeneration(TSharedPtr<Ink::FThread> InThreadAtGeneration)
{
	ThreadAtGeneration = InThreadAtGeneration;
}

const TArray<FString> &Ink::FChoice::GetTags() const
{
	return Tags;
}

void Ink::FChoice::SetTags(const TArray<FString>& InTags)
{
	Tags = InTags;
}
