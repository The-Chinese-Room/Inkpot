#include "Blotter/BlotterVariable.h"
#include "Blotter/InkpotBlotter.h"
#include "Blotter/BlotterUIEntryVariable.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotListLibrary.h"
#include "Inkpot/InkpotValueLibrary.h"
#include "Inkpot/InkpotListLibrary.h"

void UBlotterVariable::SetStory( UInkpotStory *InStory )
{
	Story = InStory;
}

void UBlotterVariable::SetDisplayWidget(UBlotterUIEntryVariable *InDisplayWidget)
{
	DisplayWidget = InDisplayWidget;
}

void UBlotterVariable::SetName(const FString& InName)
{
	Name = FText::FromString(InName);
}

const FText& UBlotterVariable::GetName() const
{
	return Name;
}

void UBlotterVariable::SetDisplayValue(const FString& InValue)
{
	Value = FText::FromString( InValue );
}

const FText& UBlotterVariable::GetDisplayValue() const
{
	return Value;
}

void UBlotterVariable::SetType(EBlotterVariableType InType)
{
	Type = InType;
	TypeText = GetTypeText(Type);
}

const FText& UBlotterVariable::GetTypeName() const
{
	return TypeText;
}

void UBlotterVariable::SetType(TSharedPtr<Ink::FObject> InObj)
{
	Type = GetTypeFromObject( InObj );
	TypeText = GetTypeText( Type );
}

EBlotterVariableType UBlotterVariable::GetTypeFromObject(TSharedPtr<Ink::FObject> InObj)
{
	if (!InObj.IsValid())
		return EBlotterVariableType::None;

	if( InObj->CanCastTo(Ink::EInkObjectClass::FChoicePoint					))	return EBlotterVariableType::ChoicePoint;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FContainer				))	return EBlotterVariableType::Container;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FControlCommand			))	return EBlotterVariableType::ControlCommand;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FDivert					))	return EBlotterVariableType::Divert;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FGlue					))	return EBlotterVariableType::Glue;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FNativeFunctionCall		))	return EBlotterVariableType::NativeFunctionCall;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FTag					))	return EBlotterVariableType::Tag;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueBool				))	return EBlotterVariableType::ValueBool;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueDivertTarget		))	return EBlotterVariableType::ValueDivertTarget;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueFloat				))	return EBlotterVariableType::ValueFloat;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueInt				))	return EBlotterVariableType::ValueInt;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueList				))	return EBlotterVariableType::ValueList;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueString			))	return EBlotterVariableType::ValueString;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValueVariablePointer	))	return EBlotterVariableType::ValueVariablePointer;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FValue					))	return EBlotterVariableType::Value;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVariableAssignment		))	return EBlotterVariableType::VariableAssignment;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVariableReference		))	return EBlotterVariableType::VariableReference;
	else if( InObj->CanCastTo(Ink::EInkObjectClass::FVoid					))	return EBlotterVariableType::Void;
	else																		return EBlotterVariableType::Object;
}

FText UBlotterVariable::GetTypeText(EBlotterVariableType InType)
{
	switch (InType)
	{
	default:
	case EBlotterVariableType::None:						return FText::FromString(TEXT("None"));
	case EBlotterVariableType::ChoicePoint:					return FText::FromString(TEXT("Choice Point"));
	case EBlotterVariableType::Container:					return FText::FromString(TEXT("Container"));
	case EBlotterVariableType::ControlCommand:				return FText::FromString(TEXT("Control Command"));
	case EBlotterVariableType::Divert:						return FText::FromString(TEXT("Divert"));
	case EBlotterVariableType::Glue:						return FText::FromString(TEXT("Glue"));
	case EBlotterVariableType::NativeFunctionCall:			return FText::FromString(TEXT("Native Function Call"));
	case EBlotterVariableType::Tag:							return FText::FromString(TEXT("Tag"));
	case EBlotterVariableType::ValueBool:					return FText::FromString(TEXT("Bool Value"));
	case EBlotterVariableType::ValueDivertTarget:			return FText::FromString(TEXT("Divert Target Value"));
	case EBlotterVariableType::ValueFloat:					return FText::FromString(TEXT("Float Value"));
	case EBlotterVariableType::ValueInt:					return FText::FromString(TEXT("Int Value"));
	case EBlotterVariableType::ValueList:					return FText::FromString(TEXT("List Value"));
	case EBlotterVariableType::ValueString:					return FText::FromString(TEXT("String Value"));
	case EBlotterVariableType::ValueVariablePointer:		return FText::FromString(TEXT("Variable Pointer Value"));
	case EBlotterVariableType::Value:						return FText::FromString(TEXT("Value"));
	case EBlotterVariableType::VariableAssignment:			return FText::FromString(TEXT("Variable Assignment"));
	case EBlotterVariableType::VariableReference:			return FText::FromString(TEXT("Variable Reference"));
	case EBlotterVariableType::Void:						return FText::FromString(TEXT("Void"));
	case EBlotterVariableType::Object:						return FText::FromString(TEXT("Object"));
	case EBlotterVariableType::ListOrigin:					return FText::FromString(TEXT("List Origin"));
	}
}

bool UBlotterVariable::IsReadOnly() const
{
	switch (Type)
	{
	default:
	case EBlotterVariableType::None:						
	case EBlotterVariableType::ChoicePoint:					
	case EBlotterVariableType::Container:					
	case EBlotterVariableType::ControlCommand:				
	case EBlotterVariableType::Divert:						
	case EBlotterVariableType::Glue:						
	case EBlotterVariableType::NativeFunctionCall:			
	case EBlotterVariableType::Tag:							
	case EBlotterVariableType::ValueDivertTarget:			
	case EBlotterVariableType::ValueVariablePointer:		
	case EBlotterVariableType::Value:						
	case EBlotterVariableType::VariableAssignment:			
	case EBlotterVariableType::VariableReference:			
	case EBlotterVariableType::Void:						
	case EBlotterVariableType::Object:						
	case EBlotterVariableType::ListOrigin:				
		return true;

	case EBlotterVariableType::ValueBool:					
	case EBlotterVariableType::ValueFloat:					
	case EBlotterVariableType::ValueInt:					
	case EBlotterVariableType::ValueList:					
	case EBlotterVariableType::ValueString:					
		return false;
	}
}

int32 UBlotterVariable::GetIndex() const
{
	return Index;
}

void UBlotterVariable::SetIndex(uint32 InIndex)
{
	Index = InIndex;
}

bool UBlotterVariable::IsIndexEven() const
{
	return (Index & 0x01) == 0;
}

bool UBlotterVariable::SetVariableFromString(const FString& InValue)
{
	if(IsReadOnly())
		return false;

	if(!IsValid(Story))
		return false;

	bool bSuccess = false;

	switch (Type)
	{
	default:
		break;

	case EBlotterVariableType::ValueBool:
		{
			bool bValue = InValue.ToBool();
			Story->SetBool( Name.ToString(), bValue, bSuccess );
		}
		break;

	case EBlotterVariableType::ValueFloat:					
		{
			float value = FCString::Atof( *InValue );
			Story->SetFloat( Name.ToString(), value, bSuccess );
		}
		break;

	case EBlotterVariableType::ValueInt:					
		{
			int value = FCString::Atod( *InValue );
			Story->SetInt( Name.ToString(), value, bSuccess );
		}
		break;

	case EBlotterVariableType::ValueList:			
		{
			FInkpotList list = UInkpotListLibrary::MakeInkpotList(Story, Name.ToString(), InValue);
			Story->SetList( Name.ToString(), list, bSuccess );
		}
		break;

	case EBlotterVariableType::ValueString:					
		{
			Story->SetString( Name.ToString(), InValue, bSuccess );
		}
		break;
	}

	return bSuccess;
}
 
 EBlotterVariableType UBlotterVariable::GetType() const
 {
	 return Type;
 }

bool UBlotterVariable::IsType(EBlotterVariableType InType) const
{
	return Type == InType;
}

void UBlotterVariable::GetListOptions( TArray<UBlotterOption*> &ReturnValue, bool &bSuccess )
{
	ReturnValue.Reset();
	bSuccess = false;
	if(!IsType(EBlotterVariableType::ValueList))
		return;

	FInkpotList list;
	Story->GetList( Name.ToString(), list, bSuccess );
	if(!bSuccess)
		return;

	FInkpotList possible = UInkpotListLibrary::All(list);

	TArray<FString> keys;
	possible.ToStringArray(keys, true);

	ReturnValue.Reserve(keys.Num());
	for( auto key : keys )
	{
		UBlotterOption *option = NewObject<UBlotterOption>(this);
		bool bIsSet = UInkpotListLibrary::ContainsItem(list, key);
		option->Set( key, bIsSet );
		ReturnValue.Add( option );
	}
}

void UBlotterVariable::SetListOption( UBlotterOption* InOption )
{
	if(!IsType(EBlotterVariableType::ValueList))
		return;

	bool bSuccess;
	FInkpotList list;
	Story->GetList( Name.ToString(), list, bSuccess );
	if(!bSuccess)
		return;

	FInkpotList option = UInkpotListLibrary::MakeInkpotList(Story, FString(), InOption->GetText().ToString() );
	if(InOption->IsSelected())
		list = UInkpotListLibrary::Union(list, option);
	else
		list = UInkpotListLibrary::Without(list, option);

	Story->SetList( Name.ToString(), list, bSuccess );
}

bool UBlotterVariable::SetOptionsOpen(bool bInIsOptionsOpen)
{
	bOptionsOpen = bInIsOptionsOpen;
	return bOptionsOpen;
}

bool UBlotterVariable::IsOptionsOpen() const
{
	return bOptionsOpen;
}

FString UBlotterVariable::GetListAsDisplayString(const FInkpotList &InList)
{
	FString rval;
	UInkpotListLibrary::ToString( InList, rval );
	FString origin = Name.ToString() + TEXT(".");
	rval.ReplaceInline( *origin, TEXT("") );
	return rval;
}

void UBlotterVariable::SetValue( UInkpotStory *InStory, const FString &InVariableName )
{
	SetStory(InStory);
	SetName(InVariableName);
	Refresh();
}

void UBlotterVariable::Refresh()
{
	FInkpotValue value;
	bool bSuccess;

	FString variableName = GetName().ToString();

	Story->GetValue(variableName, value, bSuccess );
	if(!bSuccess)
		return;

	TSharedPtr<Ink::FObject> obj = Story->GetVariable( variableName );
	SetType(obj);

	FString displayvalue;
	if(IsType(EBlotterVariableType::ValueList))
		displayvalue = GetListAsDisplayString( UInkpotValueLibrary::InkpotValueAsList( value ) );
	else
		displayvalue = obj->ToString();

	SetDisplayValue(displayvalue);
}

void UBlotterVariable::SetEmpty()
{
	SetStory(nullptr);
	SetName(TEXT("[EMPTY]"));
	SetDisplayValue(TEXT(""));
	SetType(EBlotterVariableType::None);
	SetIndex(0);
}


