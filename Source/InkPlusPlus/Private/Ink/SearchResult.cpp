#include "Ink/SearchResult.h"
#include "Ink/Object.h"
#include "Ink/Container.h"


Ink::FSearchResult::FSearchResult()
	: bIsApproximate(false)
{
}

TSharedPtr<Ink::FObject> Ink::FSearchResult::GetCorrectObject() const
{
	return bIsApproximate ? nullptr : Object;
}

TSharedPtr<Ink::FContainer> Ink::FSearchResult::GetObjectAsContainer() const
{
	return Ink::FObject::DynamicCastTo<Ink::FContainer>(Object);
}

bool Ink::FSearchResult::GetIsApproximate() const
{
	return bIsApproximate;
}

void Ink::FSearchResult::SetIsApproximate(bool InIsApproximate)
{
	bIsApproximate = InIsApproximate;
}

TSharedPtr<Ink::FObject> Ink::FSearchResult::GetObject() const
{
	return Object;
}

void Ink::FSearchResult::SetObject(TSharedPtr<Ink::FObject> InObject)
{
	Object = InObject;
}
