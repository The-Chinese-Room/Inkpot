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

bool UInkpotLibrary::InkpotValueAsBool(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<bool>() )
		return (*InValue)->GetSubtype<bool>();
	INKPOT_ERROR( "Value is not a bool, returning default (false)");
	return false;
}

bool UInkpotLibrary::InkpotArrayValueAsBool(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsBool( InValues[InIndex] );
}

FInkpotValue UInkpotLibrary::MakeIntInkpotValue(int32 InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

int32 UInkpotLibrary::InkpotValueAsInt(const FInkpotValue & InValue)
{
	if( (*InValue)->HasSubtype<int32>() )
		return (*InValue)->GetSubtype<int32>();
	INKPOT_ERROR( "Value is not a int, returing default (0)");
	return 0;
}

int32 UInkpotLibrary::InkpotArrayValueAsInt(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsInt( InValues[InIndex] );
}

FInkpotValue UInkpotLibrary::MakeFloatInkpotValue(float InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

float UInkpotLibrary::InkpotValueAsFloat(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<float>() )
		return (*InValue)->GetSubtype<float>();
	else if( (*InValue)->HasSubtype<int32>() )
		return (float)((*InValue)->GetSubtype<int32>());
	INKPOT_ERROR( "Value is not a float, returing default (0.0f)");
	return 0.0f;
}

float UInkpotLibrary::InkpotArrayValueAsFloat(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsFloat( InValues[InIndex] );
}

FInkpotValue UInkpotLibrary::MakeStringInkpotValue(const FString &InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

FString UInkpotLibrary::InkpotValueAsString(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<FString>() )
		return (*InValue)->GetSubtype<FString>();
	INKPOT_ERROR( "Value is not a string, returing default (\"\")" );
	return FString();
}

FString UInkpotLibrary::InkpotArrayValueAsString(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsString( InValues[InIndex] );
}

FInkpotValue UInkpotLibrary::MakeInkpotList(FString InName, TArray<FString> InValues)
{
	Ink::FInkList list;
	int32 count = 0;
	for( const FString &value : InValues )
	{
		FString originName, itemName;
		bool bIsOriginSpecified = value.Split(TEXT("."), &originName, &itemName);
		if (!bIsOriginSpecified)
		{
			originName = InName;
			itemName = value;
		}
		Ink::FInkListItem item( originName, itemName );
		list.Add( item, ++count );
	}
	return FInkpotValue( MakeShared<Ink::FValueType>( list ) );
}

TArray<FString> UInkpotLibrary::InkpotValueAsList(const FInkpotValue &InValue)
{
	TArray<FString> listEntries;
	if( (*InValue)->HasSubtype<Ink::FInkList>() )
	{
		Ink::FInkList list = (*InValue)->GetSubtype<Ink::FInkList>();
		TArray<Ink::FInkListItem> listItems;
		list.GetKeys(listItems);
		for (Ink::FInkListItem& item : listItems)
		{
			FString fullentry = FString::Printf(TEXT("%s.%s"), *item.OriginName, *item.ItemName);
			listEntries.Add( fullentry );
		}
	}
	else
	{
		INKPOT_ERROR( "Value is not an ink list, returing default ()" );
	}
	return listEntries;
}

TArray<FString> UInkpotLibrary::InkpotArrayValueAsList(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsList( InValues[InIndex] );
}

FInkpotValue UInkpotLibrary::DefaultInkpotValue()
{
	return FInkpotValue();
}


