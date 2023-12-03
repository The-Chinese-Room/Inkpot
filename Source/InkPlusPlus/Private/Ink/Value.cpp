#include "Ink/Value.h"
#include "Ink/Path.h"
#include "Ink/InkListItem.h"
#include "Misc/DefaultValueHelper.h"
#include "Utility/InkPlusPlusLog.h"


bool Ink::FValue::IsTruthy() const
{
	return false;
}

TSharedPtr<Ink::FValue> Ink::FValue::Cast(const EValueType& InNewType)
{
	return nullptr;
}

Ink::FValueType Ink::FValue::GetValueObject() const
{
	return FValueType(-1);
}

Ink::EValueType Ink::FValue::GetType() const
{
	return Type;
}

TSharedPtr<Ink::FValue> Ink::FValue::Create(const FValueType& InValue)
{
	if (InValue.HasSubtype<int32>())
		return MakeShared<Ink::FValueInt>(InValue.GetSubtype<int32>());
	else if (InValue.HasSubtype<float>())
		return MakeShared<Ink::FValueFloat>(InValue.GetSubtype<float>());
	else if (InValue.HasSubtype<bool>())
		return MakeShared<Ink::FValueBool>(InValue.GetSubtype<bool>());
	else if (InValue.HasSubtype<FString>())
		return MakeShared<Ink::FValueString>(InValue.GetSubtype<FString>());
	else if (InValue.HasSubtype<TSharedPtr<Ink::FPath>>())
		return MakeShared<Ink::FValueDivertTarget>(InValue.GetSubtype<TSharedPtr<Ink::FPath>>());
	else if (InValue.HasSubtype<Ink::FInkList>())
		return MakeShared<Ink::FValueList>(InValue.GetSubtype<Ink::FInkList>());
	else
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Value : could not create a Value from the passed in Value, as it wasn't of any accepted types!"));
		return nullptr;
	}
}

void Ink::FValue::BadCastException(const EValueType& InTargetType)
{
	const FString currentType = ValueTypeAsString(Type);
	const FString targetType = ValueTypeAsString(InTargetType);
	const FString objectString = ToString();
	
	checkf(false, TEXT("Can't cast %s from %s to %s"), *objectString, *currentType, *targetType);
}

FString Ink::FValue::ValueTypeAsString(const EValueType& InTargetType) const
{
	FString typeAsString;
	if (InTargetType == EValueType::Bool)					typeAsString = TEXT("Bool");
	else if (InTargetType == EValueType::Int)				typeAsString = TEXT("Int");
	else if (InTargetType == EValueType::Float)				typeAsString = TEXT("Float");
	else if (InTargetType == EValueType::String)			typeAsString = TEXT("String");
	else if (InTargetType == EValueType::List)				typeAsString = TEXT("List");
	else if (InTargetType == EValueType::DivertTarget)		typeAsString = TEXT("DivertTarget");
	else if (InTargetType == EValueType::VariablePointer)	typeAsString = TEXT("VariablePointer");
	else typeAsString = TEXT("Unknown");

	return typeAsString;
}

#pragma region ValueBool
Ink::FValueBool::FValueBool()
	: FValueBool(false)
{
}

Ink::FValueBool::FValueBool(bool InValue)
{
	Value = InValue;
	Type = EValueType::Bool;
}

bool Ink::FValueBool::IsTruthy() const
{
	return Value;
}

Ink::FValueType Ink::FValueBool::GetValueObject() const
{
	return FValueType(Value);
}

TSharedPtr<Ink::FValue> Ink::FValueBool::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueBool>(thisShared);
	}

	if (InNewType == EValueType::Int)
		return MakeShared<Ink::FValueInt>(Value ? 1 : 0);

	if (InNewType == EValueType::Float)
		return MakeShared<Ink::FValueFloat>(Value ? 1.f : 0.f);

	if (InNewType == EValueType::String)
		return MakeShared<Ink::FValueString>(Value ? TEXT("true") : TEXT("false"));

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueBool::ToString()
{
	return Value ? TEXT("true") : TEXT("false");
}
#pragma endregion ValueBool

#pragma region ValueInt
Ink::FValueInt::FValueInt()
	: FValueInt(0)
{
}

Ink::FValueInt::FValueInt(int32 InValue)
{
	Value = InValue;
	Type = EValueType::Int;
}

bool Ink::FValueInt::IsTruthy() const
{
	return Value != 0;
}

Ink::FValueType Ink::FValueInt::GetValueObject() const
{
	return FValueType( Value );
}

TSharedPtr<Ink::FValue> Ink::FValueInt::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueInt>(thisShared);
	}

	if (InNewType == EValueType::Bool)
		return MakeShared<Ink::FValueBool>(Value == 0 ? false : true);

	if (InNewType == EValueType::Float)
		return MakeShared<Ink::FValueFloat>(static_cast<float>(Value));

	if (InNewType == EValueType::String)
		return MakeShared<Ink::FValueString>(FString::FromInt(Value));

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueInt::ToString()
{
	return FString::FromInt(Value);
}
#pragma endregion ValueInt

#pragma region ValueFloat
Ink::FValueFloat::FValueFloat()
	: FValueFloat(0.0f)
{
}

Ink::FValueFloat::FValueFloat(float InValue)
{
	Value = InValue;
	Type = EValueType::Float;
}

bool Ink::FValueFloat::IsTruthy() const
{
	return Value != 0.0f;
}

Ink::FValueType Ink::FValueFloat::GetValueObject() const
{
	return FValueType(Value);
}

TSharedPtr<Ink::FValue> Ink::FValueFloat::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueFloat>(thisShared);
	}

	if (InNewType == EValueType::Bool)
		return MakeShared<Ink::FValueBool>(Value == 0.0f ? false : true);

	if (InNewType == EValueType::Int)
		return MakeShared<Ink::FValueInt>(static_cast<int32>(Value));

	if (InNewType == EValueType::String)
		return MakeShared<Ink::FValueString>(FString::Printf(TEXT("%f"), Value));

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueFloat::ToString()
{
	return FString::Printf(TEXT("%g"), Value);
}
#pragma endregion ValueFloat

#pragma region ValueString
Ink::FValueString::FValueString()
	: FValueString(FString())
{
}

Ink::FValueString::FValueString(const FString& InValue)
{
	Value = InValue;
	Type = EValueType::String;

	bIsNewLine = Value == "\n";
	bIsInlineWhitespace = true;
	for (const TCHAR& character : InValue)
	{
		if (character != ' ' && character != '\t')
		{
			bIsInlineWhitespace = false;
			break;
		}
	}
}

bool Ink::FValueString::IsTruthy() const
{
	return Value.Len() > 0;
}

Ink::FValueType Ink::FValueString::GetValueObject() const
{
	return FValueType(Value);
}

TSharedPtr<Ink::FValue> Ink::FValueString::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueString>(thisShared);
	}

	if (InNewType == EValueType::Int)
	{
		int32 parsedInt;
		if (FDefaultValueHelper::ParseInt(Value, parsedInt))
			return MakeShared<Ink::FValueInt>(parsedInt);
		return nullptr;
	}

	if (InNewType == EValueType::Float)
	{
		float parsedFloat;
		if (FDefaultValueHelper::ParseFloat(Value, parsedFloat))
			return MakeShared<Ink::FValueFloat>(parsedFloat);
		return nullptr;
	}

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueString::ToString()
{
	return Value;
}

bool Ink::FValueString::IsNewLine() const
{
	return bIsNewLine;
}

bool Ink::FValueString::IsInlineWhitespace() const
{
	return bIsInlineWhitespace;
}

bool Ink::FValueString::IsNonWhitespace() const
{
	return !bIsNewLine && !bIsInlineWhitespace;
}
#pragma endregion ValueString

#pragma region ValueDivertTarget
Ink::FValueDivertTarget::FValueDivertTarget()
{
	Value = nullptr;
	Type = EValueType::DivertTarget;
}

Ink::FValueDivertTarget::FValueDivertTarget(TSharedPtr<Ink::FPath> InValue)
{
	Value = InValue;
	Type = EValueType::DivertTarget;
}

bool Ink::FValueDivertTarget::IsTruthy() const
{
	checkf(false, TEXT("Shouldn't be checking the truthiness of a divert target"));
	return false;
}

Ink::FValueType Ink::FValueDivertTarget::GetValueObject() const
{
	return FValueType(Value);
}

TSharedPtr<Ink::FValue> Ink::FValueDivertTarget::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueDivertTarget>(thisShared);
	}

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueDivertTarget::ToString()
{
	return FString::Printf(TEXT("DivertTargetValue(%s)"), *Value->ToString());
}
#pragma endregion ValueDivertTarget

#pragma region ValueVariablePointer
Ink::FValueVariablePointer::FValueVariablePointer()
	: FValueVariablePointer(FString())
{
}

Ink::FValueVariablePointer::FValueVariablePointer(const FString& InValue, int32 InContextIndex /* = -1*/)
{
	Value = InValue;
	Type = EValueType::VariablePointer;
	ContextIndex = InContextIndex;
}

const FString& Ink::FValueVariablePointer::GetVariableName() const
{
	return GetValue();
}

void Ink::FValueVariablePointer::SetVariableName(const FString& VariableName)
{
	SetValue(VariableName);
}

bool Ink::FValueVariablePointer::IsTruthy() const
{
	checkf(false, TEXT("Shouldn't be checking the truthiness of a variable pointer"));
	return false;
}

Ink::FValueType Ink::FValueVariablePointer::GetValueObject() const
{
	return FValueType(Value);
}

TSharedPtr<Ink::FValue> Ink::FValueVariablePointer::Cast(const EValueType& InNewType)
{
	if (Type == InNewType)
	{
		const TSharedPtr<FObject> thisShared = this->AsShared();
		return StaticCastSharedPtr<Ink::FValueVariablePointer>(thisShared);
	}

	BadCastException(InNewType);
	return nullptr;
}

FString Ink::FValueVariablePointer::ToString()
{
	return FString(TEXT("VariablePointerValue(" + Value + TEXT(")")));
}

int32 Ink::FValueVariablePointer::GetContextIndex() const
{
	return ContextIndex;
}

void Ink::FValueVariablePointer::SetContextIndex(int32 InContextIndex)
{
	ContextIndex = InContextIndex;
}
#pragma endregion ValueVariablePointer

#pragma region ValueInkList
Ink::FValueList::FValueList()
{
	Type = EValueType::List;
}

Ink::FValueList::FValueList(const Ink::FInkList& InList)
{
	Value = InList;
	Type = EValueType::List;
}

Ink::FValueList::FValueList(Ink::FInkListItem InItem, int32 InSingleValue)
{
	Value.Add(InItem, InSingleValue);
	Type = EValueType::List;
}

bool Ink::FValueList::IsTruthy() const
{
	return false;
}

Ink::FValueType Ink::FValueList::GetValueObject() const
{
	return FValueType(Value);
}

FString Ink::FValueList::ToString()
{
	return Value.ToString();
}

void Ink::FValueList::RetainListOriginsForAssignment(TSharedPtr<Ink::FObject> InOldValue, TSharedPtr<Ink::FObject> InNewValue)
{
	if (!InOldValue.IsValid() || !InNewValue.IsValid())
		return;
	
	const TSharedPtr<FValueList> oldList = DynamicCastTo<FValueList>(InOldValue);
	const TSharedPtr<FValueList> newList = DynamicCastTo<FValueList>(InNewValue);

	// When assigning the empty list, try to retain any initial origin names
	if (oldList && newList && newList->Value.Num() == 0)
		newList->Value.SetInitialOriginNames(oldList->Value.GetOriginNames());
}
#pragma endregion ValueInkList