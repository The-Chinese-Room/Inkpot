#include "Ink/Container.h"
#include "Ink/Path.h"
#include "Ink/SearchResult.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FContainer::FContainer()
	: bVisitsShouldBeCounted(false)
	, bTurnIndexShouldBeCounted(false)
	, bCountingAtStartOnly(false)
	, PathToFirstLeafContent(nullptr)
	, InternalPathToFirstLeafContent(nullptr)
	, Content(MakeShared<TArray<TSharedPtr<Ink::FObject>>>())
	, NamedContent(MakeShared<TMap<FString, TSharedPtr<Ink::FObject>>>())
{
}

Ink::FContainer::FContainer(const FContainer& InOtherContainer)
{
	bVisitsShouldBeCounted = InOtherContainer.bTurnIndexShouldBeCounted;
	bTurnIndexShouldBeCounted = InOtherContainer.bTurnIndexShouldBeCounted;
	bCountingAtStartOnly = InOtherContainer.bCountingAtStartOnly;
	Name = InOtherContainer.Name;

	PathToFirstLeafContent = InOtherContainer.PathToFirstLeafContent;
	InternalPathToFirstLeafContent = InOtherContainer.InternalPathToFirstLeafContent;
	Content = InOtherContainer.Content;
	NamedContent = InOtherContainer.NamedContent;
}

Ink::FContainer& Ink::FContainer::operator=(const FContainer& InOtherContainer)
{
	bVisitsShouldBeCounted = InOtherContainer.bTurnIndexShouldBeCounted;
	bTurnIndexShouldBeCounted = InOtherContainer.bTurnIndexShouldBeCounted;
	bCountingAtStartOnly = InOtherContainer.bCountingAtStartOnly;
	Name = InOtherContainer.Name;
	
	PathToFirstLeafContent = InOtherContainer.PathToFirstLeafContent;
	InternalPathToFirstLeafContent = InOtherContainer.InternalPathToFirstLeafContent;
	Content = InOtherContainer.Content;
	NamedContent = InOtherContainer.NamedContent;
	
	return *this;
}

Ink::FContainer::~FContainer()
{
}

TSharedPtr<TArray<TSharedPtr<Ink::FObject>>> Ink::FContainer::GetContent() const
{
	return Content;
}

void Ink::FContainer::SetContent(TArray<TSharedPtr<Ink::FObject>> InContent)
{
	AddContent(InContent);
}

TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>>  Ink::FContainer::GetNamedContent() const
{
	return NamedContent;
}

void Ink::FContainer::SetNamedContent(TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> InNamedContent)
{
	NamedContent = InNamedContent;
}

TMap<FString, TSharedPtr<Ink::FObject>> Ink::FContainer::GetNamedOnlyContent() const
{
	TMap<FString, TSharedPtr<Ink::FObject>> namedOnlyContent;
	for (const TPair<FString, TSharedPtr<Ink::FObject>>& contentPair : *NamedContent)
	{
		if (contentPair.Value.IsValid())
		{
			TSharedPtr<Ink::FObject> contentValue = contentPair.Value;
			namedOnlyContent.Add(contentPair.Key, contentValue);
		}
	}

	for (const TSharedPtr<Ink::FObject>& content : *Content)
	{
		TSharedPtr<Ink::FContainer> namedContent = DynamicCastTo<Ink::FContainer>(content);
		if (namedContent.IsValid() && namedContent->HasValidName())
		{
			namedOnlyContent.Remove(namedContent->GetName());
		}
	}

	return namedOnlyContent;
}

void Ink::FContainer::SetNamedOnlyContent(const TMap<FString, TSharedPtr<Ink::FObject>>& InNamedOnlyContent)
{
	TMap<FString, TSharedPtr<Ink::FObject>> existingNamedOnly = GetNamedOnlyContent();
	for (const TPair<FString, TSharedPtr<Ink::FObject>>& contentPair : existingNamedOnly)
	{
		NamedContent->Remove(contentPair.Key);
	}

	for (const TPair<FString, TSharedPtr<Ink::FObject>>& contentPair : InNamedOnlyContent)
	{
		TSharedPtr<Ink::FContainer> namedContent = DynamicCastTo<Ink::FContainer>(contentPair.Value);
		if (!namedContent.IsValid())
		{
			UE_LOG(InkPlusPlus, Error, TEXT("failed to cast FObject into INamedContent!"))
			continue;
		}

		AddToNamedContentOnly(contentPair.Value);
	}
}

bool Ink::FContainer::GetVisitsShouldBeCounted() const
{
	return bVisitsShouldBeCounted;
}

bool Ink::FContainer::GetTurnIndexShouldBeCounted() const
{
	return bTurnIndexShouldBeCounted;
}

void Ink::FContainer::SetTurnIndexShouldBeCounted(bool InShouldTurnIndexBeCounted)
{
	bTurnIndexShouldBeCounted = InShouldTurnIndexBeCounted;
}

bool Ink::FContainer::GetCountingAtStartOnly() const
{
	return bCountingAtStartOnly;
}

void Ink::FContainer::SetCountingAtStartOnly(bool InShouldCountAtStartOnly)
{
	bCountingAtStartOnly = InShouldCountAtStartOnly;
}

uint8 Ink::FContainer::GetCountFlags() const
{
	uint8 flags = 0;
	if (bVisitsShouldBeCounted) flags |= static_cast<uint8>(Ink::ECountFlags::Visits);
	if (bTurnIndexShouldBeCounted) flags |= static_cast<uint8>(Ink::ECountFlags::Turns);
	if (bCountingAtStartOnly) flags |= static_cast<uint8>(Ink::ECountFlags::CountStartOnly);

	
	// If we're only storing CountStartOnly, it serves no purpose,
	// since it's dependent on the other two to be used at all.
	// (e.g. for setting the fact that *if* a gather or choice's
	// content is counted, then is should only be counter at the start)
	// So this is just an optimisation for storage.
	if (flags == static_cast<uint8>(Ink::ECountFlags::CountStartOnly))
		flags = 0;

	return flags;
}

void Ink::FContainer::SetCountFlags(ECountFlags InFlags)
{
	if ((InFlags & Ink::ECountFlags::Visits) != Ink::ECountFlags::None) bVisitsShouldBeCounted = true;
	if ((InFlags & Ink::ECountFlags::Turns) != Ink::ECountFlags::None) bTurnIndexShouldBeCounted = true;
	if ((InFlags & Ink::ECountFlags::CountStartOnly) != Ink::ECountFlags::None) bCountingAtStartOnly = true;
}

TSharedPtr<Ink::FPath> Ink::FContainer::GetPathToFirstLeafContent()
{
	if (!PathToFirstLeafContent.IsValid())
		PathToFirstLeafContent = GetPath()->GetPathByAppendingPath(InternalPathToFirstLeafContent);
	
	return PathToFirstLeafContent;
}

Ink::FSearchResult Ink::FContainer::GetContentAtPath(TSharedPtr<Ink::FPath> InPath, int32 InPartialPathStart /*= 0*/, int32 InPartialPathLength /*= -1*/)
{
	if (InPartialPathLength == -1)
		InPartialPathLength = InPath->GetLength();

	Ink::FSearchResult result;
	result.SetIsApproximate(false);

	TSharedPtr<Ink::FContainer> currentContainer = DynamicCastTo<Ink::FContainer>(this->AsShared());
	TSharedPtr<Ink::FObject> currentObject = this->AsShared();

	for (int32 i = InPartialPathStart; i < InPartialPathLength; ++i)
	{
		TSharedPtr<FComponent> component = InPath->GetComponent(i);

		// Path component was wrong type
		if (!currentContainer.IsValid())
		{
			result.SetIsApproximate(true);
			break;
		}

		TSharedPtr<Ink::FObject> foundObject = currentContainer->GetContentWithPathComponent(*component);

		if (foundObject == nullptr)
		{
			result.SetIsApproximate(true);
			break;
		}

		currentObject = foundObject;
		currentContainer = DynamicCastTo<Ink::FContainer>(foundObject);
	}

	result.SetObject(currentObject);
	return result;
}

TSharedPtr<Ink::FObject> Ink::FContainer::GetContentWithPathComponent(const Ink::FComponent& InComponent)
{
	if (InComponent.IsIndex())
	{
		const int32 componentIndex = InComponent.GetIndex();
		if (componentIndex >= 0 && componentIndex < Content->Num())
		{
			const TSharedPtr<Ink::FObject> content = (*Content)[componentIndex];
			return content;
		}
		else
		{
			return nullptr;
		}
	}

	else if (InComponent.IsParent())
	{
		return Parent;
	}

	else
	{
		auto findResult = NamedContent->Find(InComponent.GetName());
		if (findResult != nullptr)
		{
			return *findResult;
		}
		else
		{
			return nullptr;
		}
	}
}

void Ink::FContainer::AddContent(const TArray<TSharedPtr<Ink::FObject>>& ContentList)
{
	for (const TSharedPtr<Ink::FObject>& content : ContentList)
	{
		AddContent(content);
	}
}

void Ink::FContainer::AddContent(TSharedPtr<Ink::FObject> InObject)
{
	Content->Add(InObject);
	checkf(!InObject->HasParent(), TEXT("content is already in %s!"), *InObject->GetParent()->ToString())

	InObject->SetParent(this->AsShared());
	TryAddNamedContent(InObject);
}

void Ink::FContainer::TryAddNamedContent(TSharedPtr<Ink::FObject> InObject)
{
	if (InObject->HasValidName())
	{
		AddToNamedContentOnly(InObject);
	}
}

void Ink::FContainer::AddToNamedContentOnly(TSharedPtr<Ink::FObject> InNamedContent)
{
	InNamedContent->SetParent(this->AsShared());
	const FString contentName = InNamedContent->GetName();
	NamedContent->Add(contentName, InNamedContent);
}

TSharedPtr<Ink::FPath> Ink::FContainer::GetInternalPathToFirstLeafContent()
{
	TArray<TSharedPtr<Ink::FComponent>> components;
	TSharedPtr<FContainer> container = DynamicCastTo<FContainer>(this->AsShared());

	while (container != nullptr)
	{
		if (container->Content->Num() > 0)
		{
			components.Add(MakeShared<FComponent>(0));
			container = DynamicCastTo<Ink::FContainer>(container->Content->operator[](0));
		}
		else
		{
			break;
		}
	}
	
	return MakeShared<Ink::FPath>(components);
}
