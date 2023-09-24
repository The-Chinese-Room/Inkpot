#include "Ink/VariableReference.h"
#include "Ink/Container.h"
#include "Ink/Path.h"
#include "Ink/SearchResult.h"


Ink::FVariableReference::FVariableReference()
	: Name(FString()),
	PathForCount(nullptr)
{
}

Ink::FVariableReference::FVariableReference(const FString& InName)
	: Name(InName),
	PathForCount(nullptr)
{
}

FString Ink::FVariableReference::ToString()
{
	if (!Name.IsEmpty())
	{
		return FString::Printf(TEXT("var(%s)"), *Name);
	}
	else
	{
		const FString pathString = GetPathStringForCount();
		return FString::Printf(TEXT("read_count(%s)"), *pathString);
	}
}

FString Ink::FVariableReference::GetName() const
{
	return Name;
}

void Ink::FVariableReference::SetName(const FString& InName)
{
	Name = InName;
}

FString Ink::FVariableReference::GetPathStringForCount()
{
	if (!PathForCount.IsValid())
		return FString();

	return CompactPathString(PathForCount);
}

void Ink::FVariableReference::SetPathStringForCount(const FString& InPathString)
{
	if (InPathString.IsEmpty())
		PathForCount = nullptr;
	else
		PathForCount = MakeShared<Ink::FPath>(InPathString);
}

TSharedPtr<Ink::FContainer> Ink::FVariableReference::GetContainerForCount()
{
	return ResolvePath(PathForCount).GetObjectAsContainer();
}
