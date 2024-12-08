#include "Inkpot/InkpotValueLibrary.h"
#include "Utility/InkpotLog.h"

FInkpotValue UInkpotValueLibrary::MakeBoolInkpotValue(bool bInValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(bInValue) );
}

bool UInkpotValueLibrary::IsInkpotValueBool(const FInkpotValue &InValue)
{
	return (*InValue)->HasSubtype<bool>();
}

bool UInkpotValueLibrary::InkpotValueAsBool(const FInkpotValue &InValue)
{
	if( IsInkpotValueBool(InValue) )
		return (*InValue)->GetSubtype<bool>();
	INKPOT_ERROR( "Value is not a bool, returning default (false)");
	return false;
}

bool UInkpotValueLibrary::IsInkpotArrayValueBool(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return IsInkpotValueBool( InValues[InIndex] );
}

bool UInkpotValueLibrary::InkpotArrayValueAsBool(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsBool( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeIntInkpotValue(int32 InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

bool UInkpotValueLibrary::IsInkpotValueInt(const FInkpotValue &InValue)
{
	return (*InValue)->HasSubtype<int32>();
}

int32 UInkpotValueLibrary::InkpotValueAsInt(const FInkpotValue & InValue)
{
	if( IsInkpotValueInt(InValue) )
		return (*InValue)->GetSubtype<int32>();
	INKPOT_ERROR( "Value is not a int, returing default (0)");
	return 0;
}

bool UInkpotValueLibrary::IsInkpotArrayValueInt(const TArray<FInkpotValue> &InValues, int InIndex)
{
	return IsInkpotValueInt( InValues[InIndex] );
}

int32 UInkpotValueLibrary::InkpotArrayValueAsInt(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsInt( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeFloatInkpotValue(float InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

bool UInkpotValueLibrary::IsInkpotValueFloat(const FInkpotValue &InValue)
{
	return (*InValue)->HasSubtype<float>();
}

float UInkpotValueLibrary::InkpotValueAsFloat(const FInkpotValue &InValue)
{
	if( IsInkpotValueFloat(InValue) )
		return (*InValue)->GetSubtype<float>();
	else if( IsInkpotValueInt(InValue) )
		return (float)((*InValue)->GetSubtype<int32>());
	INKPOT_ERROR( "Value is not a float, returing default (0.0f)");
	return 0.0f;
}

bool UInkpotValueLibrary::IsInkpotArrayValueFloat(const TArray<FInkpotValue> &InValues, int InIndex)
{
	return IsInkpotValueFloat( InValues[InIndex] );
}

float UInkpotValueLibrary::InkpotArrayValueAsFloat(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsFloat( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::MakeStringInkpotValue(const FString &InValue)
{
	return FInkpotValue( MakeShared<Ink::FValueType>(InValue) );
}

bool UInkpotValueLibrary::IsInkpotValueString(const FInkpotValue &InValue)
{
	return (*InValue)->HasSubtype<FString>();
}

FString UInkpotValueLibrary::InkpotValueAsString(const FInkpotValue &InValue)
{
	if( IsInkpotValueString(InValue) )
		return (*InValue)->GetSubtype<FString>();
	INKPOT_ERROR( "Value is not a string, returing default (\"\")" );
	return FString();
}

bool UInkpotValueLibrary::IsInkpotArrayValueString(const TArray<FInkpotValue> &InValues, int InIndex)
{
	return IsInkpotValueString( InValues[InIndex] );
}

FString UInkpotValueLibrary::InkpotArrayValueAsString(const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsString( InValues[InIndex] );
}

bool UInkpotValueLibrary::IsInkpotValueList(const FInkpotValue &InValue )
{
	return (*InValue)->HasSubtype<Ink::FInkList>();
}

FInkpotList UInkpotValueLibrary::InkpotValueAsList( const FInkpotValue &InValue )
{
	FInkpotList list;
	if( IsInkpotValueList(InValue) )
		list = *InValue;
	else
		INKPOT_ERROR( "Value is not an ink list, returing default ()" );
	return list;
}

bool UInkpotValueLibrary::IsInkpotArrayValueList(const TArray<FInkpotValue> &InValues, int InIndex)
{
	return IsInkpotValueList( InValues[InIndex] );
}

FInkpotList UInkpotValueLibrary::InkpotArrayValueAsList( const TArray<FInkpotValue> &InValues, int InIndex )
{
	return InkpotValueAsList( InValues[InIndex] );
}

FInkpotValue UInkpotValueLibrary::DefaultInkpotValue()
{
	return FInkpotValue();
}


