#pragma once
#include "CoreMinimal.h"
#include "InkListItem.h"

namespace Ink
{
	// If ListDefinition is acting up, look in "InkListItem" key struct first
	class INKPLUSPLUS_API FListDefinition
	{
	public:
		FListDefinition() = default;
		FListDefinition(const FString& InName, TMap<FString, int32> InItems);
		FListDefinition(const Ink::FListDefinition& InOtherListDefinition) = default;
		FListDefinition& operator=(const Ink::FListDefinition& InOtherListDefinition) = default;
		~FListDefinition() = default;

		int32 GetValueForItem(const Ink::FInkListItem& InItem);
		bool ContainsItems(const Ink::FInkListItem& InItem) const;
		bool ContainsItemWithName(const FString& InItemName) const;
		
		bool TryGetItemWithValue(int32 InValue, Ink::FInkListItem& OutItem);
		bool TryGetValueForItem(const Ink::FInkListItem& InItem, int32& OutValue);

		FString GetName() const;
		
		const TMap<Ink::FInkListItem, int32, FDefaultSetAllocator, Ink::TInkListItemKeyFunc<int32>>& GetItems() const;

		const TMap<FString, int32>& GetItemNameToValues() const;

	private:
		FString Name;
		mutable TMap<Ink::FInkListItem, int32, FDefaultSetAllocator, Ink::TInkListItemKeyFunc<int32>> Items;

		// The main representation should be simple item names rather than a RawListItem,
		// since we mainly want to access items based on their simple name, since that's
		// how they'll be most commonly requested from ink.
		TMap<FString, int32> ItemNameToValues;
	};
}