#include "Inkpot/InkpotValue.h"
#include "Utility/InkpotLog.h"

FInkpotValue::FInkpotValue()
{
}

FInkpotValue::FInkpotValue(TSharedPtr<Ink::FValueType> InValue)
: Value( InValue )
{
}

FInkpotValue::FInkpotValue(Ink::FValueType InValue)
: Value( MakeShared<Ink::FValueType>(InValue) )
{
}

TSharedPtr<Ink::FValueType> FInkpotValue::operator *() const
{
	return Value;
}

FInkpotValue UInkpotLibrary::MakeBoolInkpotValue(bool bInValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(bInValue) );
}

bool UInkpotLibrary::InkpotValueAsBool(FInkpotValue InValue)
{
	if( (*InValue)->HasSubtype<bool>() )
		return (*InValue)->GetSubtype<bool>();
	INKPOT_ERROR( "Value is not a bool, returning default (false)");
	return false;
}

FInkpotValue UInkpotLibrary::MakeIntInkpotValue(int32 InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

int32 UInkpotLibrary::InkpotValueAsInt(FInkpotValue InValue)
{
	if( (*InValue)->HasSubtype<int32>() )
		return (*InValue)->GetSubtype<int32>();
	INKPOT_ERROR( "Value is not a int, returing default (0)");
	return 0;
}

FInkpotValue UInkpotLibrary::MakeFloatInkpotValue(float InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

float UInkpotLibrary::InkpotValueAsFloat(FInkpotValue InValue)
{
	if( (*InValue)->HasSubtype<float>() )
		return (*InValue)->GetSubtype<float>();
	INKPOT_ERROR( "Value is not a float, returing default (0.0f)");
	return 0.0f;
}

FInkpotValue UInkpotLibrary::MakeStringInkpotValue(const FString &InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

FString UInkpotLibrary::InkpotValueAsString(FInkpotValue InValue)
{
	if( (*InValue)->HasSubtype<FString>() )
		return (*InValue)->GetSubtype<FString>();
	INKPOT_ERROR( "Value is not a string, returing default (\"\")" );
	return FString();
}

FInkpotValue UInkpotLibrary::MakeInkpotList(const TArray<FString> &InValues)
{
	return MakeInkpotNamedList( TEXT( "InkpotTemp" ), InValues );
}

FInkpotValue UInkpotLibrary::MakeInkpotNamedList(const FString &InName, const TArray<FString> &InValues)
{
	Ink::FInkList list;
	int32 count = 0;
	for( const FString &value : InValues )
	{
		Ink::FInkListItem item( InName, value );
		list.Add( item, ++count );
	}
	return FInkpotValue( MakeShared<Ink::FValueType>( list ) );
}

TArray<FString> UInkpotLibrary::InkpotValueAsList(FInkpotValue InValue)
{
	TArray<FString> listEntries;
	if( (*InValue)->HasSubtype<Ink::FInkList>() )
	{
		Ink::FInkList list = (*InValue)->GetSubtype<Ink::FInkList>();
		TArray<Ink::FInkListItem> listItems;
		list.GetKeys(listItems);
		for( Ink::FInkListItem &item : listItems )
			listEntries.Add( item.ItemName );
	}
	else
	{
		INKPOT_ERROR( "Value is not an ink list, returing default ()" );
	}
	return listEntries;
}



