#include "Ink/ListDefinitionsOrigin.h"
//#include "Ink/ListDefinition.h"
#include "Ink/Value.h"


Ink::FListDefinitionsOrigin::FListDefinitionsOrigin(TArray<TSharedPtr<Ink::FListDefinition>> InLists)
{
	Lists.Reserve(InLists.Num());
	
	for (TSharedPtr<Ink::FListDefinition> list : InLists)
	{
		Lists.Add(list->GetName(), list);
		for (const TPair<Ink::FInkListItem, int32>& itemWithValue : list->GetItems())
		{
			const Ink::FInkListItem item = itemWithValue.Key;
			const int32 value = itemWithValue.Value;
			const TSharedPtr<Ink::FValueList> listValue = MakeShared<Ink::FValueList>(item, value);

			// May be ambiguous, but compiler should've caught that,
			// so we may be doing some replacement here, but that's okay.
			AllUnambiguousListValueCache.Add(item.ItemName, listValue);
			AllUnambiguousListValueCache.Add(item.GetFullName(), listValue);
		}
	}
}

bool Ink::FListDefinitionsOrigin::TryListGetDefinition(const FString& InName, TSharedPtr<FListDefinition>& OutListDefinition)
{
	const TSharedPtr<Ink::FListDefinition>* outDefinition = Lists.Find(InName);
	if (outDefinition != nullptr)
	{
		OutListDefinition = *outDefinition;
		return true;
	}

	OutListDefinition = nullptr;
	return false;
}

TSharedPtr<Ink::FValueList> Ink::FListDefinitionsOrigin::FindSingleItemListWithName(const FString& InName)
{
	TSharedPtr<Ink::FValueList>* outValueList = AllUnambiguousListValueCache.Find(InName);
	if (outValueList == nullptr)
	{
		return nullptr;
	}
	else
	{
		return *outValueList;
	}
}

TArray<TSharedPtr<Ink::FListDefinition>> Ink::FListDefinitionsOrigin::GetLists() const
{
	TArray<TSharedPtr<Ink::FListDefinition>> listOfLists;
	Lists.GenerateValueArray(listOfLists);
	return listOfLists;
}
