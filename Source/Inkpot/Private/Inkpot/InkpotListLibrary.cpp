#include "Inkpot/InkpotListLibrary.h"
#include "Inkpot/InkpotGameplayTagLibrary.h"
#include "Inkpot/InkpotStory.h"
#include "Utility/InkpotLog.h"

void UInkpotListLibrary::ToString(const FInkpotList &InList, FString &ReturnValue, bool bInUseOrigin )
{
	InList.ToString( ReturnValue, bInUseOrigin );
}

FInkpotList UInkpotListLibrary::MakeInkpotListFromStringArray(UInkpotStory *InStory, FString InOrigin, TArray<FString> InValues)
{
	Ink::FInkList list;
	list.SetInitialOriginName(InOrigin);

	int32 count = 0;
	for( const FString &value : InValues )
	{
		FString originName, itemName;
		bool bIsOriginSpecified = value.Split(TEXT("."), &originName, &itemName);
		if (!bIsOriginSpecified)
		{
			originName = InOrigin;
			itemName = value;
		}
		Ink::FInkListItem item( originName, itemName );
		list.Add( item, ++count );
	}

	FInkpotList rval( MakeShared<Ink::FValueType>( list ) );

	if(InStory)
		rval.ValidateOrigin( InStory );
	else
		INKPOT_WARN("No story, list is not validated");

	return rval;
}

FInkpotList UInkpotListLibrary::MakeInkpotList(UInkpotStory *InStory, FString InOrigin, FString InValue)
{
	TArray<FString> items;
	InValue.ParseIntoArray( items, TEXT( "," ), true );
	for(FString &item : items )
		item.TrimStartAndEndInline();
	FInkpotList list = MakeInkpotListFromStringArray(InStory, InOrigin, items );
	return list;
}

void UInkpotListLibrary::ToStringArray(const FInkpotList &InList, TArray<FString> &ReturnValue, bool bUseOrigin )
{
	InList.ToStringArray( ReturnValue, bUseOrigin );
}

FInkpotList UInkpotListLibrary::MakeInkpotListFromGameplayTag(UInkpotStory *InStory, FGameplayTag InTag)
{
	FString value = InTag.ToString();
	value.RemoveFromStart(INK_ORIGIN_GAMEPLAYTAG_PREFIX);
	return MakeInkpotList(InStory, FString(), value);
}

void UInkpotListLibrary::ToGameplayTag(const FInkpotList &Value, FGameplayTag &ReturnValue)
{
	Ink::FInkList &list = Value.GetList();
	for( auto &pair : list )
	{
		FString sTag = FString::Printf(TEXT("%s%s"),INK_ORIGIN_GAMEPLAYTAG_PREFIX, *pair.Key.GetFullName() );
		ReturnValue = FGameplayTag::RequestGameplayTag(FName(sTag));
		break;
	}
}

FInkpotList UInkpotListLibrary::MakeInkpotListFromGameplayTags(UInkpotStory *InStory, FGameplayTagContainer InTags)
{
	const TArray<FGameplayTag>&  tags = InTags.GetGameplayTagArray();
	bool first = true;

	TArray<FString> sTags;
	for(const FGameplayTag& tag : tags )
	{
		FString sTag = tag.ToString();
		sTag.RemoveFromStart(INK_ORIGIN_GAMEPLAYTAG_PREFIX);
		sTags.Add( sTag );
	}

	return MakeInkpotListFromStringArray( InStory, FString(), sTags );
}

void UInkpotListLibrary::ToGameplayTags(const FInkpotList &Value, FGameplayTagContainer &ReturnValue)
{
	Ink::FInkList &list = Value.GetList();
	for( auto &pair : list )
	{
		FString sTag = FString::Printf(TEXT("%s%s"),INK_ORIGIN_GAMEPLAYTAG_PREFIX, *pair.Key.GetFullName() );
		FGameplayTag tag = FGameplayTag::RequestGameplayTag(FName(sTag));
		ReturnValue.AddTag( tag );
	}
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

bool UInkpotListLibrary::ContainsTag(const FInkpotList &Source, FGameplayTag InTag)
{
	FString sTag = InTag.ToString();
	sTag.RemoveFromStart(INK_ORIGIN_GAMEPLAYTAG_PREFIX);
	return ContainsItem(Source, sTag);
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

FInkpotList UInkpotListLibrary::MinItem(const FInkpotList &A)
{
	Ink::FInkList &listA = A.GetList();

	Ink::FInkList rval = listA.MinAsList();

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

FInkpotList UInkpotListLibrary::MaxItem(const FInkpotList &A)
{
	Ink::FInkList &listA = A.GetList();

	Ink::FInkList rval = listA.MaxAsList();

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

FInkpotList UInkpotListLibrary::Inverse(const FInkpotList &A)
{
	Ink::FInkList &listA = A.GetList();

	if(!listA.GetOrigins().IsValid())
	{
		INKPOT_ERROR("List has no origin set, validate against story first.");
	}

	Ink::FInkList rval = listA.Inverse();

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

FInkpotList UInkpotListLibrary::All(const FInkpotList &A)
{
	Ink::FInkList &listA = A.GetList();

	if(!listA.GetOrigins().IsValid())
	{
		INKPOT_ERROR("List has no origin set, validate against story first.");
	}

	Ink::FInkList rval = listA.All();

	return FInkpotList( MakeShared<Ink::FValueType>(rval) );
}

const FInkpotList& UInkpotListLibrary::Validate(UInkpotStory *InStory, const FInkpotList &A)
{
	A.ValidateOrigin(InStory);
	return A;
}

