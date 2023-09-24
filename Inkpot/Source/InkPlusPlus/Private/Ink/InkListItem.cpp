#include "Ink/InkListItem.h"

Ink::FInkListItem::FInkListItem(const FString& InOriginName, const FString& InItemName)
	: OriginName(InOriginName),
	ItemName(InItemName)
{
}

Ink::FInkListItem::FInkListItem(const FString& InFullName)
{
	TArray<FString> nameParts;
	InFullName.ParseIntoArray(nameParts, TEXT("."));

	checkf(nameParts.Num() == 2, TEXT("Passed in FullName (%s) could not be parsed into origin and item name. Make sure that FullName is delimited with '.'!"), *InFullName);
	
	OriginName = nameParts[0];
	ItemName = nameParts[1];
}

bool Ink::FInkListItem::operator==(const Ink::FInkListItem& InOtherInkListItem) const
{
	return InOtherInkListItem.OriginName == OriginName && InOtherInkListItem.ItemName == ItemName;
}

bool Ink::FInkListItem::IsNull() const
{
	return OriginName.IsEmpty() && ItemName.IsEmpty();
}

FString Ink::FInkListItem::GetFullName() const
{
	return FString::Printf(TEXT("%s.%s"), *OriginName, *ItemName);
}

FString Ink::FInkListItem::ToString() const
{
	return GetFullName();
}

uint32 Ink::FInkListItem::GetTypeHash() const
{
	uint32 originCode = 0;
	const int32 itemCode = TextKeyUtil::HashString(ItemName);
	if (!OriginName.IsEmpty())
		originCode = TextKeyUtil::HashString(OriginName);

	return originCode + itemCode;
}

Ink::FInkListItem Ink::FInkListItem::Null()
{
	return FInkListItem(FString(), FString());
}
