#include "Ink/Pointer.h"
#include "Ink/Container.h"
#include "Ink/Path.h"


Ink::FPointer::FPointer()
	: Index(-1),
	Container(nullptr)
{
}

Ink::FPointer::FPointer(int32 InIndex)
	: Index(InIndex),
	Container(nullptr)
{
}

Ink::FPointer::FPointer(TSharedPtr<FContainer> InContainer, int32 InIndex)
	: Index(InIndex)
	, Container(InContainer)
{
}

bool Ink::FPointer::IsNull() const
{
	return !Container.IsValid();
}

TSharedPtr<Ink::FPath> Ink::FPointer::GetPath() const
{
	if (IsNull())
		return nullptr;

	if (Index >= 0)
	{
		const TSharedPtr<Ink::FComponent> component = MakeShared<Ink::FComponent>(Index);
		return Container->GetPath()->GetPathByAppendingComponent(component);
	}
	else
	{
		return Container->GetPath();
	}
}

TSharedPtr<Ink::FObject> Ink::FPointer::Resolve() const
{
	if (Index < 0) return Container;
	if (IsNull()) return nullptr;
	if (Container->GetContent()->Num() == 0) return Container;
	if (Index >= Container->GetContent()->Num()) return nullptr;
	return (*Container->GetContent())[Index];
}

int32 Ink::FPointer::GetIndex() const
{
	return Index;
}

void Ink::FPointer::SetIndex(int32 InIndex)
{
	Index = InIndex;
}

void Ink::FPointer::IncrementIndex()
{
	++Index;
}

TSharedPtr<Ink::FContainer> Ink::FPointer::GetContainer() const
{
	return Container;
}

void Ink::FPointer::SetContainer(TSharedPtr<Ink::FContainer> InContainer)
{
	Container = InContainer;
}

FString Ink::FPointer::ToString() const
{
	if (IsNull())
		return FString(TEXT("Ink Pointer (null)"));

	return FString::Printf(TEXT("Ink Pointer -> %s -- index %d "), *Container->GetPath()->ToString(), Index);
}

Ink::FPointer Ink::FPointer::StartOf(TSharedPtr<Ink::FContainer> InContainer)
{
	return Ink::FPointer(InContainer, 0);
}

Ink::FPointer Ink::FPointer::Null()
{
	return Ink::FPointer(-1);
}

