#include "Inkpot/InkpotValue.h"
#include "Utility/InkpotLog.h"

FInkpotValue::FInkpotValue()
{
}

// this sets as a reference
FInkpotValue::FInkpotValue(TSharedPtr<Ink::FValueType> InValue)
: Value( InValue )
{
}

// this sets as a copy
FInkpotValue::FInkpotValue(Ink::FValueType InValue)
: Value( MakeShared<Ink::FValueType>(InValue) )
{
}

TSharedPtr<Ink::FValueType> FInkpotValue::operator *() const
{
	return Value;
}

