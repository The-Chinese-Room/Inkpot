#include "Inkpot/InkpotList.h"
#include "Inkpot/InkpotStory.h"
#include "Utility/InkpotLog.h"

static Ink::FInkList BADLIST;

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
	if ( IsValid() )
	{
		return Value->GetSubtype<Ink::FInkList>();
	}
	else
	{
		INKPOT_ERROR("List is invalid");
		return BADLIST;
	}
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

	// create a new origins array if non exists
	TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>> >&origins = list.GetOrigins();
	if( !origins.IsValid() )
	{
		origins = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>();
		TSharedPtr<TArray<FString>> names = list.GetOriginNames();
		for( const FString &name : *names )
		{
			TSharedPtr<Ink::FListDefinition> origin = InStory->GetListOrigin( name, FString() );
			if ( !origin.IsValid() )
				return false;
			origins->AddUnique( origin );
		}
	}

	for( auto &pair : list )
	{
		Ink::FInkListItem& item = pair.Key;
		// check the items has an origin named in the story and that the item matches too
		TSharedPtr<Ink::FListDefinition> origin = InStory->GetListOrigin( item.OriginName, item.ItemName );
		if (!origin.IsValid())
			return false;

		// set correct value to that in the origin
		pair.Value = origin->GetValueForItem( item );
		origins->AddUnique( origin );
	}

	return true;
}

void FInkpotList::ToString( FString& OutValue, bool bInUseOrigin ) const
{
	OutValue.Reset();

	TArray<FString> items;
	ToStringArray( items, bInUseOrigin );

	FString delim(TEXT( ", "));

	int len = 0;
	for( FString &item : items )
		len+=item.Len() + delim.Len();
	OutValue.Reserve(len);

	bool first = true;
	for( FString &item : items )
	{
		if(!first)
			OutValue.Append(delim);
		first = false;
		OutValue.Append(item);
	}
}