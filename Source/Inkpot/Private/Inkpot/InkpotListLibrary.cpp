#include "Inkpot/InkpotListLibrary.h"
#include "Utility/InkpotLog.h"

void UInkpotListLibrary::PrintToScreen(const FInkpotList &InList, bool bInUseOrigin )
{
	if (!GEngine)
		return;

	TArray<FString> items;
	InkpotListAsStrings(InList, items, bInUseOrigin );
	for( FString &item : items )
	{
		GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Green, item );
	}
}

void UInkpotListLibrary::PrintToLog(const FInkpotList &InList, bool bInUseOrigin )
{
	TArray<FString> items;
	InkpotListAsStrings(InList, items, bInUseOrigin );
	for( FString &item : items )
	{
		INKPOT_LOG("%s", *item );
	}
}

void UInkpotListLibrary::ToString(const FInkpotList &InList, FString &ReturnValue, bool bInUseOrigin )
{
	InList.ToString( ReturnValue, bInUseOrigin );
}

FInkpotList UInkpotListLibrary::MakeInkpotList(FString InDefinition, TArray<FString> InValues)
{
	Ink::FInkList list;
	int32 count = 0;
	for( const FString &value : InValues )
	{
		FString originName, itemName;
		bool bIsOriginSpecified = value.Split(TEXT("."), &originName, &itemName);
		if (!bIsOriginSpecified)
		{
			originName = InDefinition;
			itemName = value;
		}
		Ink::FInkListItem item( originName, itemName );
		list.Add( item, ++count );
	}
	return FInkpotList( MakeShared<Ink::FValueType>( list ) );
}

FInkpotList UInkpotListLibrary::MakeInkpotSingleEntryList(FString InDefinition, FString InValue)
{
	return MakeInkpotList( InDefinition, { InValue } );
}

void UInkpotListLibrary::InkpotListAsStrings(const FInkpotList &InList, TArray<FString> &ReturnValue, bool bUseOrigin )
{
	InList.ToStringArray( ReturnValue, bUseOrigin );
}

FInkpotList UInkpotListLibrary::Union(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	Ink::FInkList rval = listA.Union( listB );

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

FInkpotList UInkpotListLibrary::Intersect(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	Ink::FInkList rval = listA.Intersect( listB );

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

bool UInkpotListLibrary::HasIntersection(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.HasIntersection( listB );

	return rval;
}

FInkpotList UInkpotListLibrary::Without( const FInkpotList &A, const FInkpotList &B )
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	Ink::FInkList rval = listA.Without( listB );

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

bool UInkpotListLibrary::ContainsList( const FInkpotList &Source, const FInkpotList &Querant )
{
	Ink::FInkList &listSource = Source.GetList();
	Ink::FInkList &listQuerant = Querant.GetList();

	bool rval = listSource.ContainsList( listQuerant );

	return rval;
}

bool UInkpotListLibrary::ContainsItem(const FInkpotList &Source, const FString &InListItemName)
{
	Ink::FInkList &listSource = Source.GetList();

	FString originName, itemName;
	bool bIsOriginSpecified = InListItemName.Split(TEXT("."), &originName, &itemName);
	if(!bIsOriginSpecified)
		itemName = InListItemName;

	bool rval = listSource.ContainsItemNamed( itemName );

	return rval;
}

bool UInkpotListLibrary::GreaterThan(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.GreaterThan( listB );

	return rval;
}

bool UInkpotListLibrary::GreaterThanOrEquals(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.GreaterThanOrEquals( listB );

	return rval;
}

bool UInkpotListLibrary::LessThan(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.LessThan( listB );

	return rval;
}

bool UInkpotListLibrary::LessThanOrEquals(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.LessThanOrEquals( listB );

	return rval;
}

bool UInkpotListLibrary::Equals(const FInkpotList &A, const FInkpotList &B)
{
	Ink::FInkList &listA = A.GetList();
	Ink::FInkList &listB = B.GetList();

	bool rval = listA.Equals( listB );

	return rval;
}
