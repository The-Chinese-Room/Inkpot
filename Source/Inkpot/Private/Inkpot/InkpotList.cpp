#include "Inkpot/InkpotList.h"
#include "Inkpot/InkpotStory.h"


FInkpotList::FInkpotList()
: FInkpotValue()
{
}

FInkpotList::FInkpotList(TSharedPtr<Ink::FValueType> InValue)
: FInkpotValue(InValue)
{
}

Ink::FInkList &FInkpotList::GetList() const
{
	return Value->GetSubtype<Ink::FInkList>();
}

void FInkpotList::ToStringArray( TArray<FString>& OutValues, bool bUseOrigin ) const
{
	OutValues.Reset();
	Ink::FInkList &list = GetList();
	TArray<Ink::FInkListItem> listItems;
	list.GetKeys(listItems);
	OutValues.Reserve( listItems.Num() );
	for (Ink::FInkListItem& item : listItems)
	{
		FString entry;
		if(bUseOrigin)
			entry = FString::Printf( TEXT( "%s.%s" ), *item.OriginName, *item.ItemName );
		else
			entry = item.ItemName;

		OutValues.Add( entry );
	}
}

bool FInkpotList::ValidateOrigin( UInkpotStory *InStory ) const
{
	Ink::FInkList &list = GetList();

	TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>> >&origins = list.GetOrigins();
	if( !origins.IsValid() )
		origins = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>();

	TArray<Ink::FInkListItem> listItems;
	list.GetKeys(listItems);
	for (Ink::FInkListItem& item : listItems)
	{
		TSharedPtr<Ink::FListDefinition> origin = InStory->GetListOrigin(item.OriginName, item.ItemName);
		if (!origin.IsValid())
			return false;

		origins->AddUnique(origin);
	}

	return true;
}


