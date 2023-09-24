#pragma once
#include "CoreMinimal.h"

namespace Ink
{
	struct INKPLUSPLUS_API FInkListItem
	{
		// The name of the list where the item was originally defined.
		FString OriginName;

		// The main name of the item as defined in ink.
		FString ItemName;

		FInkListItem() {}

		// Create an item with the given original list definition name, and the name of this item.
		FInkListItem(const FString& InOriginName, const FString& InItemName);

		// Create an item from a dot-separted string of the form "listDefinitionName.listItemName".
		FInkListItem(const FString& InFullName);

		bool operator==(const Ink::FInkListItem& InOtherInkListItem) const;

		bool IsNull() const;

		// Get the full dot-separated name of the item, in the form "listDefinitionName.itemName".
		FString GetFullName() const;

		// Get the full dot-separated name of the item, in the form "listDefinitionName.itemName".
		// Calls fullName internally.
		FString ToString() const;
		
		// Get the hashcode for an item.
		uint32 GetTypeHash() const;
		
		static FInkListItem Null();

		FORCEINLINE friend void operator<<(FStructuredArchive::FSlot Slot, TPair<Ink::FInkListItem, int32>& pair)
		{
			unimplemented();
		}
		
		FORCEINLINE friend FArchive& operator<<(FArchive& Ar, TPair<Ink::FInkListItem, int32>& pair)
		{
			unimplemented();
			return Ar;
		}
	};

	
	FORCEINLINE uint32 GetTypeHash(const FInkListItem& InListItem)
	{
		return InListItem.GetTypeHash();
	}

	
	// Custom TMap Key for Ink List Item struct
	template <typename ValueType>
	struct INKPLUSPLUS_API TInkListItemKeyFunc : public BaseKeyFuncs<ValueType, Ink::FInkListItem, false>
	{
		static FORCEINLINE uint32 GetSetKey(const TPair<Ink::FInkListItem, ValueType>& Element)
		{
			return Element.Key.GetTypeHash();
		}

		static FORCEINLINE bool Matches(const uint32 A, const uint32 B)
		{
			return A == B;
		}

		static int32 GetKeyHash(int32 Key)
		{
			return ::GetTypeHash(Key);
		}
	};
}