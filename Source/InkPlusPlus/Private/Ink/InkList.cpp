#include "Ink/InkList.h"

#include "Ink/ListDefinition.h"
#include "Ink/ListDefinitionsOrigin.h"
#include "Ink/Story.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FInkList::FInkList()
{
}

Ink::FInkList::FInkList(const Ink::FInkList& InOtherInkList)
: FInkListBaseMap(InOtherInkList)
{
	TSharedPtr<TArray<FString>> otherOriginNames = InOtherInkList.GetOriginNames();
	if(otherOriginNames.IsValid())
		_originNames = MakeShared<TArray<FString>>(*otherOriginNames);

	if (InOtherInkList.Origins.IsValid())
		Origins = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>(*InOtherInkList.Origins);
}

Ink::FInkList::FInkList(const FString& InSingleOriginListName, Ink::FStory* InOriginStory)
{
	SetInitialOriginName(InSingleOriginListName);

	TSharedPtr<Ink::FListDefinition> definition = nullptr;
	TSharedPtr<FListDefinitionsOrigin> originStoryDefinition = InOriginStory->GetListDefinitions();
	if (originStoryDefinition->TryListGetDefinition(InSingleOriginListName, definition))
	{
		Origins = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>();
		Origins->Add(definition);
	}
	else
		UE_LOG(InkPlusPlus, Error, TEXT("InkList origin could not be found in story when constructing new list: %s"), *InSingleOriginListName);
}

Ink::FInkList::FInkList(const Ink::FInkListItem& InKey, int32 InValue)
{
	Add(InKey, InValue);
}

Ink::FInkList::FInkList(const TPair<Ink::FInkListItem, int32>& InSingleElement)
{
	Add(InSingleElement.Key, InSingleElement.Value);
}

void Ink::FInkList::AddItem(const Ink::FInkListItem& InItem)
{
	if (InItem.OriginName.IsEmpty())
	{
		AddItem(InItem.ItemName);
		return;
	}

	for (const TSharedPtr<FListDefinition>& origin : *Origins)
	{
		if (origin->GetName() == InItem.OriginName)
		{
			int32 intVal;
			if (origin->TryGetValueForItem(InItem, intVal))
			{
				const Ink::FInkListItem itemSmartPtr(InItem);
				Add(itemSmartPtr, intVal);
				return;
			}
			else
			{
				checkf(false, TEXT("Could not add the item %s to this list because it doesn't exist in the original list definition in ink."), *InItem.ToString());
			}
		}
	}

	checkf(false, TEXT("Failed to add item to list because the item was from a new list definition that wasn't previously known to this list. Only items from previously known lists can be used, so that the int value can be found."));
}

void Ink::FInkList::AddItem(const FString& InItemName)
{
	TSharedPtr<FListDefinition> foundListDefinition = nullptr;

	for (const TSharedPtr<FListDefinition>& origin : *Origins)
	{
		if (origin->ContainsItemWithName(InItemName))
		{
			if (foundListDefinition.IsValid())
				checkf(false, TEXT("Could not add the item %s to this list because it could come from either %s or %s"), *InItemName, *origin->GetName(), *foundListDefinition->GetName());
			
			foundListDefinition = origin;
		}
	}

	if (!foundListDefinition.IsValid())
		checkf(false, TEXT("Could not add the item %s to this list because it isn't known to any list definitions previously associated with this list."), *InItemName);

	const Ink::FInkListItem itemSmartPtr(foundListDefinition->GetName(), InItemName);
	const int32 itemValue = foundListDefinition->GetValueForItem(itemSmartPtr);
	Add(itemSmartPtr, itemValue);
}

bool Ink::FInkList::TryAddItem(const FString& InItemName)
{
	TSharedPtr<FListDefinition> foundListDefinition = nullptr;

	for (const TSharedPtr<FListDefinition>& origin : *Origins)
	{
		if (origin->ContainsItemWithName(InItemName))
		{
			if (foundListDefinition.IsValid())
			{
				UE_LOG(InkPlusPlus, Warning, TEXT("Could not add the item %s to this list because it could come from either %s or %s"),
					*InItemName, *origin->GetName(), *foundListDefinition->GetName());
				return false;
			}

			foundListDefinition = origin;
		}
	}

	if (!foundListDefinition.IsValid())
	{
		UE_LOG(InkPlusPlus, Warning, TEXT("Could not add the item %s to this list because it isn't known to any list definitions previously associated with this list."),
		*InItemName);
		return false;
	}

	const Ink::FInkListItem itemSmartPtr(foundListDefinition->GetName(), InItemName);
	const int32 itemValue = foundListDefinition->GetValueForItem(itemSmartPtr);
	Add(itemSmartPtr, itemValue);

	return true;
}

bool Ink::FInkList::ContainsItemNamed(const FString& InItemName) const
{
	for (const TPair<Ink::FInkListItem, int32>& pair : *this)
	{
		if (pair.Key.ItemName == InItemName)
			return true;
	}
	
	return false;
}

TSharedPtr<Ink::FListDefinition> Ink::FInkList::OriginOfMaxItem() const
{
	if (!Origins.IsValid())
		return nullptr;
	
	const FString maxOriginName = MaxItem().Key.OriginName;
	for (const TSharedPtr<Ink::FListDefinition>& origin : *Origins)
	{
		if (origin->GetName() == maxOriginName)
			return origin;
	}

	return nullptr;
}

void Ink::FInkList::SetInitialOriginName(const FString& InInitialOriginName)
{
	_originNames = MakeShared<TArray<FString>>();
	_originNames->Add(InInitialOriginName);
}

void Ink::FInkList::SetInitialOriginNames(TSharedPtr<TArray<FString>> InInitialOriginNames)
{
	if (InInitialOriginNames.IsValid())
		_originNames = MakeShared<TArray<FString>>(*InInitialOriginNames);
	else
		_originNames.Reset();
}

TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>>>& Ink::FInkList::GetOrigins()
{
	return Origins;
}

TPair<Ink::FInkListItem, int32> Ink::FInkList::MaxItem() const
{
	TPair<Ink::FInkListItem, int32> max;
	for (const TPair<Ink::FInkListItem, int32>& pair : *this)
	{
		if (max.Key.IsNull() || pair.Value > max.Value)
			max = pair;
	}
	return max;
}

TPair<Ink::FInkListItem, int32> Ink::FInkList::MinItem() const
{
	TPair<Ink::FInkListItem, int32> min;
	for (const TPair<Ink::FInkListItem, int32>& pair : *this)
	{
		if (min.Key.IsNull() || pair.Value < min.Value)
			min = pair;
	}
	return min;
}

Ink::FInkList Ink::FInkList::Inverse() const
{
	FInkList list;
	if (Origins.IsValid())
	{
		for (const TSharedPtr<Ink::FListDefinition>& origin : *Origins)
		{
			for (const TPair<Ink::FInkListItem, int32>& itemPair : origin->GetItems())
			{
				if (!TMap::Contains(itemPair.Key))
					list.Add(itemPair.Key, itemPair.Value);
			}
		}
	}
	return list;
}

Ink::FInkList Ink::FInkList::All() const
{
	FInkList list;
	if (Origins.IsValid())
	{
		for (const TSharedPtr<Ink::FListDefinition>& origin : *Origins)
		{
			for (const TPair<Ink::FInkListItem, int32>& itemPair : origin->GetItems())
			{
				list.Add(itemPair.Key, itemPair.Value);
			}
		}
	}
	return list;
}

Ink::FInkList Ink::FInkList::Union(const Ink::FInkList& InOtherList) const
{
	FInkList list = *this;
	for (const TPair<FInkListItem, int32>& itemPair : InOtherList)
	{
		list.Add(itemPair.Key, itemPair.Value);
	}
	return list;
}

Ink::FInkList Ink::FInkList::Intersect(const Ink::FInkList& InOtherList) const
{
	FInkList list;
	for (const TPair<FInkListItem, int32>& itemPair : *this)
	{
		if (InOtherList.Contains(itemPair.Key))
			list.Add(itemPair.Key, itemPair.Value);
	}
	return list;
}

bool Ink::FInkList::HasIntersection(const Ink::FInkList& InOtherList) const
{
	for (const TPair<FInkListItem, int32>& itemPair : *this)
	{
		if (InOtherList.Contains(itemPair.Key))
			return true;
	}
	return false;
}

Ink::FInkList Ink::FInkList::Without(const Ink::FInkList& InListToRemove) const
{
	FInkList list = *this;
	for (const TPair<FInkListItem, int32>& itemPair : InListToRemove)
	{
		list.Remove(itemPair.Key);
	}
	return list;
}

bool Ink::FInkList::ContainsList(const Ink::FInkList& InOtherList) const
{
	if( (InOtherList.Num() == 0 ) || Num() == 0 ) 
		return false;

	for (const TPair<FInkListItem, int32>& itemPair : InOtherList)
	{
		if (!Contains(itemPair.Key))
			return false;
	}
	return true;
}

bool Ink::FInkList::ContainsItem(const FString& InListItemName) const
{
	for (const TPair<FInkListItem, int32>& itemPair : *this)
	{
		if (itemPair.Key.ItemName.Equals(InListItemName))
			return true;
	}
	return false;
}

bool Ink::FInkList::GreaterThan(const Ink::FInkList& InOtherList) const
{
	if (TMap::Num() == 0)		return false;
	if (InOtherList.Num() == 0)	return true;

	// All Greater
	return MinItem().Value > InOtherList.MaxItem().Value;
}

bool Ink::FInkList::GreaterThanOrEquals(const Ink::FInkList& InOtherList) const
{
	if (TMap::Num() == 0)		return false;
	if (InOtherList.Num() == 0)	return true;

	return MinItem().Value >= InOtherList.MinItem().Value
		&& MaxItem().Value >= InOtherList.MaxItem().Value;
}

bool Ink::FInkList::LessThan(const Ink::FInkList& InOtherList) const
{
	if (InOtherList.Num() == 0)	return false;
	if (TMap::Num() == 0)		return true;

	return MaxItem().Value < InOtherList.MinItem().Value;
}

bool Ink::FInkList::LessThanOrEquals(const Ink::FInkList& InOtherList) const
{
	if (InOtherList.Num() == 0)	return false;
	if (TMap::Num() == 0)		return true;

	return MaxItem().Value <= InOtherList.MaxItem().Value
		&& MinItem().Value <= InOtherList.MinItem().Value;
}

Ink::FInkList Ink::FInkList::MaxAsList() const
{
	if (TMap::Num() > 0)
		return FInkList(MaxItem());
	else
		return FInkList();
}

Ink::FInkList Ink::FInkList::MinAsList() const
{
	if (TMap::Num() > 0)
		return FInkList(MinItem());
	else
		return FInkList();
}

Ink::FInkList Ink::FInkList::ListWithSubRange(const Ink::FValueType& InMinBound, const Ink::FValueType& InMaxBound) const
{
	if (this->Num() == 0)
		return FInkList();

	TArray<TPair<Ink::FInkListItem, int32>> ordered = GetOrderedItems();

	int32 minValue = 0;
	int32 maxValue = INT32_MAX;

	if (InMinBound.HasSubtype<int32>())
	{
		minValue = InMinBound.GetSubtype<int32>();
	}
	else
	{
		if (InMinBound.HasSubtype<Ink::FInkList>())
		{
			Ink::FInkList minAsList = InMinBound.GetSubtype<Ink::FInkList>();
			if (minAsList.Num() > 0)
				minValue = minAsList.MinItem().Value;
		}
	}
	
	if (InMaxBound.HasSubtype<int32>())
	{
		maxValue = InMaxBound.GetSubtype<int32>();
	}
	else
	{
		if (InMaxBound.HasSubtype<Ink::FInkList>())
		{
			Ink::FInkList maxAsList = InMaxBound.GetSubtype<Ink::FInkList>();
			if (maxAsList.Num() > 0)
				maxValue = maxAsList.MaxItem().Value;
		}
	}

	FInkList subList;
	subList.SetInitialOriginNames(GetOriginNames());
	for (const TPair<Ink::FInkListItem, int32>& itemPair : ordered)
	{
		if (itemPair.Value >= minValue && itemPair.Value <= maxValue)
			subList.Add(itemPair.Key, itemPair.Value);
	}

	return subList;
}

bool Ink::FInkList::Equals(const Ink::FInkList& InOtherList) const
{
	if (InOtherList.Num() != TMap::Num())
		return false;

	for (const TPair<FInkListItem, int32>& itemPair : *this)
	{
		if (!InOtherList.Contains(itemPair.Key))
			return false;
	}

	return true;
}

int32 Ink::FInkList::GetHashCode() const
{
	int32 ownHash = 0;
	for (const TPair<FInkListItem, int32>& itemPair : *this)
	{
		ownHash += itemPair.Key.GetTypeHash();
	}
	return ownHash;
}

FString Ink::FInkList::ToString() const
{
	TArray<TPair<Ink::FInkListItem, int32>> ordered = GetOrderedItems();
	FString string;
	for (int32 i = 0; i < ordered.Num(); ++i)
	{
		if (i > 0)
			string += TEXT(", ");

		Ink::FInkListItem item = ordered[i].Key;
		string += item.ItemName;
	}

	return string;
}

TArray<TPair<Ink::FInkListItem, int32>> Ink::FInkList::GetOrderedItems() const
{
	TArray<TPair<Ink::FInkListItem, int32>> orderedList = MapToArray(*this);
	orderedList.Sort([&](TPair<Ink::FInkListItem, int32> InA, TPair<Ink::FInkListItem, int32> InB)->bool
	{
		if (InA.Value < InB.Value)
		{
			return true;
		}
		else if (InA.Value == InB.Value)
		{
			return InA.Key.OriginName.Compare(InB.Key.OriginName) < 0; // Ensure consistent ordering of mixed lists.
		}
		else
		{
			return false;
		}
	});
	return orderedList;
}

TSharedPtr<TArray<FString>> Ink::FInkList::GetOriginNames() const
{
	if (this->Num() > 0) {
		if (!_originNames.IsValid() && this->Num() > 0)
			_originNames = MakeShared<TArray<FString>>();
		else
			_originNames->Reset();

		for (const TTuple<Ink::FInkListItem, int32>& itemAndValue : *this)
			_originNames->Add(itemAndValue.Key.OriginName);
	}

	return _originNames;
}

Ink::FInkList Ink::FInkList::FromString(const FString& InMyListItem, const Ink::FStory& InOriginStory)
{
	const TSharedPtr<Ink::FValueList> listValue = InOriginStory.GetListDefinitions()->FindSingleItemListWithName(InMyListItem);
	if (listValue.IsValid())
	{
		return FInkList(listValue->GetValue());
	}
	else
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Could not find the InkListItem from the string '%s' to create an InkList because it doesn't exist in the original list definition in ink."), *InMyListItem);
		return FInkList();
	}
}

TArray<TPair<Ink::FInkListItem, int32>> Ink::FInkList::MapToArray(const Ink::FInkList& InList)
{
	TArray<TPair<Ink::FInkListItem, int32>> pairList;

	for (const TPair<Ink::FInkListItem, int32>& itemPair : InList)
	{
		pairList.Add(itemPair);
	}

	return pairList;
}