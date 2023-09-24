#include "Ink/ListDefinition.h"
#include "Ink/InkList.h"

Ink::FListDefinition::FListDefinition(const FString& InName, TMap<FString, int32> InItems)
	: Name(InName),
	ItemNameToValues(InItems)
{
}

int32 Ink::FListDefinition::GetValueForItem(const Ink::FInkListItem& InItem)
{
	const int32* value = ItemNameToValues.Find(InItem.ItemName);
	if (value != nullptr)
		return *value;
	return 0;
}

bool Ink::FListDefinition::ContainsItems(const Ink::FInkListItem& InItem) const
{
	if (InItem.ItemName != Name)
		return false;

	return ItemNameToValues.Contains(InItem.ItemName);
}

bool Ink::FListDefinition::ContainsItemWithName(const FString& InItemName) const
{
	return ItemNameToValues.Contains(InItemName);
}

bool Ink::FListDefinition::TryGetItemWithValue(int32 InValue, Ink::FInkListItem& OutItem)
{
	for (const TPair<FString, int32>& namedItem : ItemNameToValues)
	{
		if (namedItem.Value == InValue)
		{
			OutItem = Ink::FInkListItem(Name, namedItem.Key);
			return true;
		}
	}

	OutItem = Ink::FInkListItem::Null();
	return false;
}

bool Ink::FListDefinition::TryGetValueForItem(const Ink::FInkListItem& InItem, int32& OutValue)
{
	const int32* value = ItemNameToValues.Find(InItem.ItemName);
	if (value != nullptr)
	{
		OutValue = *value;
		return true;
	}
	OutValue = -1;
	return false;
}

FString Ink::FListDefinition::GetName() const
{
	return Name;
}

const TMap<Ink::FInkListItem, int32, FDefaultSetAllocator, Ink::TInkListItemKeyFunc<int32>>& Ink::FListDefinition::GetItems() const
{
	if (Items.Num() == 0)
	{
		for (const TPair<FString, int32>& itemNameAndValue : ItemNameToValues)
		{
			Ink::FInkListItem item(Name, itemNameAndValue.Key);
			Items.Add(item, itemNameAndValue.Value);
		}
	}
	return Items;
}

const TMap<FString, int32>& Ink::FListDefinition::GetItemNameToValues() const
{
	return ItemNameToValues;
}
