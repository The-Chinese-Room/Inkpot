#include "Blotter/BlotterVariable.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"


void UBlotterVariable::SetName(const FString& InName)
{
	Name = FText::FromString(InName);
}

const FText& UBlotterVariable::GetName() const
{
	return Name;
}

void UBlotterVariable::SetValue(const FString& InValue)
{
	Value = FText::FromString(InValue);
}

const FText& UBlotterVariable::GetValue() const
{
	return Value;
}

void UBlotterVariable::SetType(const FText& InType)
{
	Type = InType;
}

const FText& UBlotterVariable::GetType() const
{
	return Type;
}

void UBlotterVariable::SetType(TSharedPtr<Ink::FObject> InObj)
{

	FText type = GetTypeName(InObj);
	SetType(type);
}

FText UBlotterVariable::GetTypeName(TSharedPtr<Ink::FObject> InObj)
{
	if (!InObj.IsValid())
		return FText::FromString(TEXT("[none]"));

	if( InObj->CanCastTo(Ink::EInkObjectClass::FChoicePoint					))	return FText::FromString(TEXT("Choice Point"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FContainer				))	return FText::FromString(TEXT("Container"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FControlCommand			))	return FText::FromString(TEXT("Control Command"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FDivert					))	return FText::FromString(TEXT("Divert"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FGlue					))	return FText::FromString(TEXT("Glue"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FNativeFunctionCall		))	return FText::FromString(TEXT("Native Function Call"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FTag					))	return FText::FromString(TEXT("Tag"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueBool				))	return FText::FromString(TEXT("Value Bool"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueDivertTarget		))	return FText::FromString(TEXT("Value Divert Target"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueFloat				))	return FText::FromString(TEXT("Value Float"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueInt				))	return FText::FromString(TEXT("Value Int"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueList				))	return FText::FromString(TEXT("Value List"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueString			))	return FText::FromString(TEXT("Value String"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueVariablePointer	))	return FText::FromString(TEXT("Value Variable Pointer"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValue					))	return FText::FromString(TEXT("Value"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVariableAssignment		))	return FText::FromString(TEXT("Variable Assignment"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVariableReference		))	return FText::FromString(TEXT("Variable Reference"));
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVoid					))	return FText::FromString(TEXT("Void"));
	else																		return FText::FromString(TEXT("Object"));
}


