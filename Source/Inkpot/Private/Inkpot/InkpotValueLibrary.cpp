#include "Inkpot/InkpotValueLibrary.h"
#include "Utility/InkpotLog.h"

FInkpotValue UInkpotValueLibrary::MakeBoolInkpotValue(bool bInValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(bInValue) );
}

bool UInkpotValueLibrary::InkpotValueAsBool(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<bool>() )
		return (*InValue)->GetSubtype<bool>();
	INKPOT_ERROR( "Value is not a bool, returning default (false)");
	return false;
}

bool UInkpotValueLibrary::InkpotArrayValueAsBool(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsBool( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeIntInkpotValue(int32 InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

int32 UInkpotValueLibrary::InkpotValueAsInt(const FInkpotValue & InValue)
{
	if( (*InValue)->HasSubtype<int32>() )
		return (*InValue)->GetSubtype<int32>();
	INKPOT_ERROR( "Value is not a int, returing default (0)");
	return 0;
}

int32 UInkpotValueLibrary::InkpotArrayValueAsInt(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsInt( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeFloatInkpotValue(float InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

float UInkpotValueLibrary::InkpotValueAsFloat(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<float>() )
		return (*InValue)->GetSubtype<float>();
	else if( (*InValue)->HasSubtype<int32>() )
		return (float)((*InValue)->GetSubtype<int32>());
	INKPOT_ERROR( "Value is not a float, returing default (0.0f)");
	return 0.0f;
}

float UInkpotValueLibrary::InkpotArrayValueAsFloat(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsFloat( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeStringInkpotValue(const FString &InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

FString UInkpotValueLibrary::InkpotValueAsString(const FInkpotValue &InValue)
{
	if( (*InValue)->HasSubtype<FString>() )
		return (*InValue)->GetSubtype<FString>();
	INKPOT_ERROR( "Value is not a string, returing default (\"\")" );
	return FString();
}

FString UInkpotValueLibrary::InkpotArrayValueAsString(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsString( InValues[InIndex] );
}

FInkpotList UInkpotValueLibrary::InkpotValueAsList( const FInkpotValue &InValue )
{
	FInkpotList list;
	if( (*InValue)->HasSubtype<Ink::FInkList>() )
		list = *InValue;
	else
		INKPOT_ERROR( "Value is not an ink list, returing default ()" );
	return list;
}

FInkpotList UInkpotValueLibrary::InkpotArrayValueAsList( const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsList( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::DefaultInkpotValue()
{
	return FInkpotValue();
}


