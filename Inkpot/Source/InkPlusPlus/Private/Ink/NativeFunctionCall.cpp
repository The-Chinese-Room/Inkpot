#include "Ink/NativeFunctionCall.h"
#include "Ink/InkList.h"
#include "Ink/ListDefinition.h"
#include "Ink/Void.h"
#include "Ink/Path.h"


TMap<FString, TSharedPtr<Ink::FNativeFunctionCall>> Ink::FNativeFunctionCall::NativeFunctions = TMap<FString, TSharedPtr<Ink::FNativeFunctionCall>>();

FString Ink::FNativeFunctionCall::Add = TEXT("+");
FString Ink::FNativeFunctionCall::Subtract = TEXT("-");
FString Ink::FNativeFunctionCall::Divide = TEXT("/");
FString Ink::FNativeFunctionCall::Multiply = TEXT("*");
FString Ink::FNativeFunctionCall::Mod = TEXT("%");
FString Ink::FNativeFunctionCall::Negate = TEXT("_"); // distinguish from "-" for subtraction
FString Ink::FNativeFunctionCall::Equal = TEXT("==");
FString Ink::FNativeFunctionCall::Greater = TEXT(">");
FString Ink::FNativeFunctionCall::Less = TEXT("<");
FString Ink::FNativeFunctionCall::GreaterThanOrEquals = TEXT(">=");
FString Ink::FNativeFunctionCall::LessThanOrEquals = TEXT("<=");
FString Ink::FNativeFunctionCall::NotEquals = TEXT("!=");
FString Ink::FNativeFunctionCall::Not = TEXT("!");
FString Ink::FNativeFunctionCall::And = TEXT("&&");
FString Ink::FNativeFunctionCall::Or = TEXT("||");
FString Ink::FNativeFunctionCall::Min = TEXT("MIN");
FString Ink::FNativeFunctionCall::Max = TEXT("MAX");
FString Ink::FNativeFunctionCall::Pow = TEXT("POW");
FString Ink::FNativeFunctionCall::Floor = TEXT("FLOOR");
FString Ink::FNativeFunctionCall::Ceiling = TEXT("CEILING");
FString Ink::FNativeFunctionCall::Int = TEXT("INT");
FString Ink::FNativeFunctionCall::Float = TEXT("FLOAT");
FString Ink::FNativeFunctionCall::Has = TEXT("?");
FString Ink::FNativeFunctionCall::Hasnt = TEXT("!?");
FString Ink::FNativeFunctionCall::Intersect = TEXT("^");
FString Ink::FNativeFunctionCall::ListMin = TEXT("LIST_MIN");
FString Ink::FNativeFunctionCall::ListMax = TEXT("LIST_MAX");
FString Ink::FNativeFunctionCall::All = TEXT("LIST_ALL");
FString Ink::FNativeFunctionCall::Count = TEXT("LIST_COUNT");
FString Ink::FNativeFunctionCall::ValueOfList = TEXT("LIST_VALUE");
FString Ink::FNativeFunctionCall::Invert = TEXT("LIST_INVERT");


Ink::FNativeFunctionCall::FNativeFunctionCall(const FString& InName)
	: Name(InName),
	NumberOfParameters(0),
	bIsPrototype(false),
	Prototype(nullptr)
{
	GenerateNativeFunctionsIfNecessary();
	SetNameInternal(InName);
	/*
	 * The C# code had a property "name", which - when set - would assign Prototype if not already set
	 * So by setting the name, you would also be creating the association with the function call
	 */
}


// Require default constructor for serialisation
Ink::FNativeFunctionCall::FNativeFunctionCall()
	: Name(FString()),
	NumberOfParameters(0),
	bIsPrototype(false),
	Prototype(nullptr)
{
	GenerateNativeFunctionsIfNecessary();
}

// Only called internally to generate prototypes
Ink::FNativeFunctionCall::FNativeFunctionCall(const FString& InName, int32 InNumberOfParameters)
	: Name(InName),
	NumberOfParameters(InNumberOfParameters),
	bIsPrototype(true),
	Prototype(nullptr)
{
	SetName(InName);
}

FString Ink::FNativeFunctionCall::ToString()
{
	return FString::Printf(TEXT("Native '%s'"), *Name);
}

TSharedPtr<Ink::FObject> Ink::FNativeFunctionCall::Call(const TArray<TSharedPtr<Ink::FObject>>& InParameters)
{
	if (Prototype)
		return Prototype->Call(InParameters);

	if (NumberOfParameters != InParameters.Num())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : Unexpected number of parameters in a Call function"))
		return nullptr;
	}

	bool hasList = false;
	for (const TSharedPtr<Ink::FObject>& parameter : InParameters)
	{
		if (DynamicCastTo<Ink::FVoid>(parameter))
		{
			StoryException.Throw(TEXT("NativeFunctionCall : Attempting to perform operation on a void value. Did you forget to 'return' a value from a function you called here?"));
			return nullptr;
		}
		if (DynamicCastTo<Ink::FValueList>(parameter))
		{
			hasList = true;
		}
	}

	// Binary operations on lists are treated outside of the standard coerscion rules
	if (InParameters.Num() == 2 && hasList)
		return CallBinaryListOperation(InParameters);

	TArray<TSharedPtr<Ink::FValue>> coercedParameters = CoerceValuesToSingleType(InParameters);
	if (coercedParameters.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : coerced parameters list is empty!"))
		return nullptr;
	}
	
	const Ink::EValueType coercedType = coercedParameters[0]->GetType();
	switch (coercedType)
	{
		case Ink::EValueType::Int:
			return Call<int32>(coercedParameters);
		case Ink::EValueType::Float:
			return Call<float>(coercedParameters);
		case Ink::EValueType::String:
			return Call<FString>(coercedParameters);
		case Ink::EValueType::DivertTarget:
			return Call<TSharedPtr<Ink::FPath>>(coercedParameters);
		case Ink::EValueType::List:
			return Call<Ink::FInkList>(coercedParameters);
		default:
			UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : coerced parameters type %d is unexpected!"), coercedType);
			break;
	}

	return nullptr;
}

FString Ink::FNativeFunctionCall::GetName() const
{
	return Name;
}

void Ink::FNativeFunctionCall::SetName(const FString& InName)
{
	// SetNameInternal exists so that we are not calling a virtual function in the constructor
	SetNameInternal(InName);
}

void Ink::FNativeFunctionCall::SetNameInternal(const FString& InName)
{
	Name = InName;
	if (!bIsPrototype)
		Prototype = NativeFunctions[Name];
}

int32 Ink::FNativeFunctionCall::GetNumberOfParameters() const
{
	if (Prototype)
		return Prototype->NumberOfParameters;
	
	return NumberOfParameters;
}

void Ink::FNativeFunctionCall::SetNumberOfParameters(int32 InNumberOfParameters)
{
	NumberOfParameters = InNumberOfParameters;
}

TSharedPtr<Ink::FNativeFunctionCall> Ink::FNativeFunctionCall::CallWithName(const FString& InFunctionName)
{
	return MakeShared<Ink::FNativeFunctionCall>(InFunctionName);
}

bool Ink::FNativeFunctionCall::CallExistsWithName(const FString& InFunctionName)
{
	GenerateNativeFunctionsIfNecessary();
	return NativeFunctions.Contains(InFunctionName);
}

TSharedPtr<Ink::FValue> Ink::FNativeFunctionCall::CallBinaryListOperation(const TArray<TSharedPtr<Ink::FObject>>& InParameters)
{
	if (InParameters.Num() < 2)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : passed in parameters list is too short to proceed with CallBinaryListOperation!"))
		return nullptr;
	}
	
	// List-Int addition/subtraction returns a List (e.g. "alpha" + 1 = "beta")
	const bool isAdditionOrSubtraction = Name == TEXT("+") || Name == TEXT("-");
	const bool isListAndInt = DynamicCastTo<Ink::FValueList>(InParameters[0]) && DynamicCastTo<Ink::FValueInt>(InParameters[1]);
	if (isAdditionOrSubtraction && isListAndInt)
	{
		return CallListIncrementOperation(InParameters);
	}

	const TSharedPtr<Ink::FValue> value1 = StaticCastSharedPtr<Ink::FValue>(InParameters[0]);
	const TSharedPtr<Ink::FValue> value2 = StaticCastSharedPtr<Ink::FValue>(InParameters[1]);

	// And/or with any other type requires coersion to bool (int)
	const bool isConditional = Name == TEXT("&&") || Name == TEXT("||");
	const bool valuesAreNotLists = ((value1->GetType() != Ink::EValueType::List) || (value2->GetType() != Ink::EValueType::List));
	if (isConditional && valuesAreNotLists)
	{
		FBinaryOperation op = BinaryOperations[Ink::EValueType::Int];
		bool result = op(FNativeFunctionOp(value1->IsTruthy() ? true : false), FNativeFunctionOp(value2->IsTruthy() ? true : false)).GetSubtype<bool>();
		return MakeShared<Ink::FValueBool>(result);
	}

	// Normal (list • list) operation
	if (value1->GetType() == Ink::EValueType::List && value2->GetType() == Ink::EValueType::List)
	{
		TArray<TSharedPtr<Ink::FValue>> list = {value1, value2};
		return Call<Ink::FInkList>(list);
	}

	StoryException.Throw(TEXT("NativeFunctionCall : Cannot call use '"+Name+"' operation on "+FString::FromInt((int)value1->GetType()) +" and "+ FString::FromInt((int)value2->GetType())));
	return nullptr;
}

TSharedPtr<Ink::FValue> Ink::FNativeFunctionCall::CallListIncrementOperation(const TArray<TSharedPtr<Ink::FObject>>& InListIntParameters)
{
	TSharedPtr<Ink::FValueList> listValue = StaticCastSharedPtr<Ink::FValueList>(InListIntParameters[0]);
	TSharedPtr<Ink::FValueInt> intValue = StaticCastSharedPtr<Ink::FValueInt>(InListIntParameters[1]);

	Ink::FInkList resultRawList;

	for (const TPair<Ink::FInkListItem, int32>& pair : listValue->GetValue())
	{
		const Ink::FInkListItem listItem = pair.Key;
		const int32 listItemValue = pair.Value;

		// Find + or - operation
		FBinaryOperation intOp = BinaryOperations[Ink::EValueType::Int];

		// Return value unknown until it's evaluated
		int32 targetInt = intOp(FNativeFunctionOp(listItemValue), FNativeFunctionOp(intValue->GetValue())).GetSubtype<int32>();

		// Find this item's origin (linear search should be ok, should be short)
		TSharedPtr<Ink::FListDefinition> itemOrigin = nullptr;

		for (const TSharedPtr<Ink::FListDefinition>& origin : *listValue->GetValue().GetOrigins())
		{
			if (origin->GetName() == listItem.OriginName)
			{
				itemOrigin = origin;
				break;
			}
		}

		if (itemOrigin.IsValid())
		{
			Ink::FInkListItem incrementedItem;
			if (itemOrigin->TryGetItemWithValue(targetInt, incrementedItem))
				resultRawList.Add(incrementedItem, targetInt);
		}
	}
	
	return MakeShared<Ink::FValueList>(resultRawList);
}

TArray<TSharedPtr<Ink::FValue>> Ink::FNativeFunctionCall::CoerceValuesToSingleType(const TArray<TSharedPtr<Ink::FObject>>& InParameters)
{
	Ink::EValueType valueType = Ink::EValueType::Int;
	TSharedPtr<Ink::FValueList> specialCaseList = nullptr;

	// Find out what the output type is
	// "higher level" types infect both so that binary operations
	// use the same type on both sides. e.g. binary operation of
	// int and float causes the int to be casted to a float.
	for (TSharedPtr<Ink::FObject> object : InParameters)
	{
		TSharedPtr<Ink::FValue> value = DynamicCastTo<Ink::FValue>(object);
		if (!value.IsValid())
		{
			UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : failed to cast object to FValue!"));
			continue;
		}

		const Ink::EValueType objValueType = value->GetType();
		if (objValueType > valueType)
			valueType = objValueType;

		if (objValueType == Ink::EValueType::List)
			specialCaseList = StaticCastSharedPtr<Ink::FValueList>(value);
	}
	
	// Coerce to this chosen type
	TArray<TSharedPtr<Ink::FValue>> parametersOut;

	// Special case: Coercing to Ints to Lists
	// We have to do it early when we have both parameters
	// to hand - so that we can make use of the List's origin
	if (valueType == Ink::EValueType::List)
	{
		for (const TSharedPtr<Ink::FObject>& object : InParameters)
		{
			TSharedPtr<Ink::FValue> value = StaticCastSharedPtr<Ink::FValue>(object);
			const Ink::EValueType type = value->GetType();
			if (type == Ink::EValueType::List)
			{
				parametersOut.Add(value);
			}
			else if (type == Ink::EValueType::Int)
			{
				int32 intValue = value->GetValueObject().GetSubtype<int32>();
				TSharedPtr<Ink::FListDefinition> list = specialCaseList->GetValue().OriginOfMaxItem();
				if (!list.IsValid())
				{
					UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : failed to to get list definition from value's origin of max item!"));
					continue;
				}

				Ink::FInkListItem item;
				if (!list->TryGetItemWithValue(intValue, item))
				{
					StoryException.Throw(TEXT("NativeFunctionCall : Could not find List item with the value "+FString::FromInt(intValue)+" in "+list->GetName()));
					return parametersOut;
				}
				
				TSharedPtr<Ink::FValueList> castedValue = MakeShared<Ink::FValueList>(item, intValue);
				parametersOut.Add(castedValue);
			}
			else
			{
				StoryException.Throw(TEXT("NativeFunctionCall : Cannot mix Lists and "+FString::FromInt((int)valueType)+" valies in this operation"));
				return parametersOut;
			}
		}
	}

	// Normal Coercing (with standard casting)
	else
	{
		for (const TSharedPtr<Ink::FObject>& object : InParameters)
		{
			TSharedPtr<Ink::FValue> value = StaticCastSharedPtr<Ink::FValue>(object);
			TSharedPtr<Ink::FValue> castedValue = value->Cast(valueType);
			parametersOut.Add(castedValue);
		}
	}

	return parametersOut;
}

void Ink::FNativeFunctionCall::GenerateNativeFunctionsIfNecessary()
{
	if (NativeFunctions.Num() > 0)
		return;

	// Why no bool operations?
	// Before evaluation, all bools are coerced to ints in
	// CoerceValuesToSingleType (see default value for valType at top).
	// So, no operations are ever directly done in bools themselves.
	// This also means that 1 == true works, since true is always converted
	// to 1 first.
	// However, many operations return a "native" bool (equals, etc).

	// Int operations
	AddIntBinaryOp(Add, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() + InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Subtract, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() - InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Multiply, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() * InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Divide, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() / InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Mod, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() % InRight.GetSubtype<int32>());});
	AddIntUnaryOp(Negate, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(-(InValue.GetSubtype<int32>()));});

	AddIntBinaryOp(Equal, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() == InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Greater, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() > InRight.GetSubtype<int32>());});
	AddIntBinaryOp(Less, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() < InRight.GetSubtype<int32>());});
	AddIntBinaryOp(GreaterThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() >= InRight.GetSubtype<int32>());});
	AddIntBinaryOp(LessThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() <= InRight.GetSubtype<int32>());});
	AddIntBinaryOp(NotEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<int32>() != InRight.GetSubtype<int32>());});
	AddIntUnaryOp(Not, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<int32>() == 0);});

	AddIntBinaryOp(And, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<int32>() != 0) && (InRight.GetSubtype<int32>() != 0));});
	AddIntBinaryOp(Or, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<int32>() != 0) || (InRight.GetSubtype<int32>() != 0));});

	AddIntBinaryOp(Max, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Max<int32>(InLeft.GetSubtype<int32>(), InRight.GetSubtype<int32>()));});
	AddIntBinaryOp(Min, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Min<int32>(InLeft.GetSubtype<int32>(), InRight.GetSubtype<int32>()));});
	AddIntBinaryOp(Pow, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Pow((float)InLeft.GetSubtype<int32>(), (float)InRight.GetSubtype<int32>()));});
	AddIntUnaryOp(Floor, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return InValue;});
	AddIntUnaryOp(Ceiling, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return InValue;});
	AddIntUnaryOp(Int, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return InValue;});
	AddIntUnaryOp(Float, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(static_cast<float>(InValue.GetSubtype<int32>()));});

	// Float Operations
	AddFloatBinaryOp(Add, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() + InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Subtract, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() - InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Multiply, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() * InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Divide, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() / InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Mod, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(-1);}); // Undefined behaviour float % float
	AddFloatUnaryOp(Negate, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(-(InValue.GetSubtype<float>()));});

	AddFloatBinaryOp(Equal, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() == InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Greater, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() > InRight.GetSubtype<float>());});
	AddFloatBinaryOp(Less, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() < InRight.GetSubtype<float>());});
	AddFloatBinaryOp(GreaterThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() >= InRight.GetSubtype<float>());});
	AddFloatBinaryOp(LessThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() <= InRight.GetSubtype<float>());});
	AddFloatBinaryOp(NotEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<float>() != InRight.GetSubtype<float>());});
	AddFloatUnaryOp(Not, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<float>() == 0.f);});

	AddFloatBinaryOp(And, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<float>() != 0.f) && (InRight.GetSubtype<float>() != 0.f));});
	AddFloatBinaryOp(Or, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<float>() != 0.f) || (InRight.GetSubtype<float>() != 0.f));});

	AddFloatBinaryOp(Max, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Max<float>(InLeft.GetSubtype<float>(), InRight.GetSubtype<float>()));});
	AddFloatBinaryOp(Min, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Min<float>(InLeft.GetSubtype<float>(), InRight.GetSubtype<float>()));});
	AddFloatBinaryOp(Pow, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp( FMath::Pow(InLeft.GetSubtype<float>(), InRight.GetSubtype<float>()));});
	AddFloatUnaryOp(Floor, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(FMath::Floor(InValue.GetSubtype<float>()));});
	AddFloatUnaryOp(Ceiling, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(FMath::CeilToFloat(InValue.GetSubtype<float>()));});
	AddFloatUnaryOp(Int, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(static_cast<int32>(InValue.GetSubtype<float>()));});
	AddFloatUnaryOp(Float, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return InValue;});

	// String Operations
	AddStringBinaryOp(Add, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<FString>() + InRight.GetSubtype<FString>());});
	AddStringBinaryOp(Equal, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<FString>().Equals(InRight.GetSubtype<FString>()));});
	AddStringBinaryOp(NotEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(!(InLeft.GetSubtype<FString>().Equals( InRight.GetSubtype<FString>())));});
	AddStringBinaryOp(Has, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InRight.GetSubtype<FString>().IsEmpty() || InLeft.GetSubtype<FString>().Contains(InRight.GetSubtype<FString>()));});
	AddStringBinaryOp(Hasnt, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(!(InLeft.GetSubtype<FString>().Contains( InRight.GetSubtype<FString>())));});

	// List Operations
	AddListBinaryOp(Add, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().Union(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(Subtract, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().Without(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(Has, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().ContainsList(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(Hasnt, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(!(InLeft.GetSubtype<Ink::FInkList>().ContainsList(InRight.GetSubtype<Ink::FInkList>())));});
	AddListBinaryOp(Intersect, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().Intersect(InRight.GetSubtype<Ink::FInkList>()));});

	AddListBinaryOp(Equal, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().Equals(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(Greater, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().GreaterThan(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(Less, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().LessThan(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(GreaterThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().GreaterThanOrEquals(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(LessThanOrEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(InLeft.GetSubtype<Ink::FInkList>().LessThanOrEquals(InRight.GetSubtype<Ink::FInkList>()));});
	AddListBinaryOp(NotEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp(!(InLeft.GetSubtype<Ink::FInkList>().Equals(InRight.GetSubtype<Ink::FInkList>())));});

	AddListBinaryOp(And, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<Ink::FInkList>().Num() > 0) && (InRight.GetSubtype<Ink::FInkList>().Num() > 0));});
	AddListBinaryOp(Or, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp{ return FNativeFunctionOp((InLeft.GetSubtype<Ink::FInkList>().Num() > 0) || (InRight.GetSubtype<Ink::FInkList>().Num() > 0));});
	AddListUnaryOp(Not, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().Num() == 0 ? 1 : 0);});

	// Placeholders to ensure that these special case functions can exist,
	// since these function is never actually run, and is special cased in Call
	AddListUnaryOp(Invert, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().Inverse());});
	AddListUnaryOp(All, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().All());});
	AddListUnaryOp(ListMin, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().MinAsList());});
	AddListUnaryOp(ListMax, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().MaxAsList());});
	AddListUnaryOp(Count, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().Num());});
	AddListUnaryOp(ValueOfList, [](FNativeFunctionOp InValue)->FNativeFunctionOp{return FNativeFunctionOp(InValue.GetSubtype<Ink::FInkList>().MaxItem().Value);});

	// Special case: The only operations you can do on divert target values
	AddDivertTargetBinaryOp(Equal, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp
	{
		const TSharedPtr<Ink::FPath> leftPath = InLeft.GetSubtype<TSharedPtr<Ink::FPath>>();
		const TSharedPtr<Ink::FPath> rightPath = InRight.GetSubtype<TSharedPtr<Ink::FPath>>();
		const bool isEqual = leftPath->Equals(rightPath);
		return FNativeFunctionOp(isEqual);
	});
	AddDivertTargetBinaryOp(NotEquals, [](FNativeFunctionOp InLeft, FNativeFunctionOp InRight)->FNativeFunctionOp
	{
		const TSharedPtr<Ink::FPath> leftPath = InLeft.GetSubtype<TSharedPtr<Ink::FPath>>();
		const TSharedPtr<Ink::FPath> rightPath = InRight.GetSubtype<TSharedPtr<Ink::FPath>>();
		const bool isEqual = leftPath->Equals(rightPath);
		return FNativeFunctionOp(!isEqual);
	});
}

void Ink::FNativeFunctionCall::AddOpFuncForType(const Ink::EValueType& InValueType, FBinaryOperation InOp)
{
	BinaryOperations.Add(InValueType, InOp);
}

void Ink::FNativeFunctionCall::AddOpFuncForType(const Ink::EValueType& InValueType, FUnaryOperation InOp)
{
	UnaryOperations.Add(InValueType, InOp);
}

void Ink::FNativeFunctionCall::AddOpToNativeFunc(const FString& InName, int32 InArgs, const Ink::EValueType& InValueType, FBinaryOperation InOp)
{
	TSharedPtr<Ink::FNativeFunctionCall> nativeFunc;
	
	TSharedPtr<Ink::FNativeFunctionCall>* findResult = NativeFunctions.Find(InName);
	if (findResult == nullptr)
	{
		// add native func if it does not already exist
		nativeFunc = MakeShared<Ink::FNativeFunctionCall>(InName, InArgs);
		NativeFunctions.Add(InName, nativeFunc);
	}
	else
	{
		nativeFunc = *findResult;
	}

	if (nativeFunc.IsValid())
	{
		nativeFunc->AddOpFuncForType(InValueType, InOp);
	}
}

void Ink::FNativeFunctionCall::AddOpToNativeFunc(const FString& InName, int32 InArgs, const Ink::EValueType& InValueType, FUnaryOperation InOp)
{
	TSharedPtr<Ink::FNativeFunctionCall> nativeFunc;

	TSharedPtr<Ink::FNativeFunctionCall>* findResult = NativeFunctions.Find(InName);
	if (findResult == nullptr)
	{
		// add native func if it does not already exist
		nativeFunc = MakeShared<Ink::FNativeFunctionCall>(InName, InArgs);
		NativeFunctions.Add(InName, nativeFunc);
	}
	else
	{
		nativeFunc = *findResult;
	}

	if (nativeFunc.IsValid())
	{
		nativeFunc->AddOpFuncForType(InValueType, InOp);
	}
}

void Ink::FNativeFunctionCall::AddIntBinaryOp(const FString& InName, FBinaryOperation InOp)
{
	AddOpToNativeFunc(InName, 2, Ink::EValueType::Int, InOp);
}

void Ink::FNativeFunctionCall::AddIntUnaryOp(const FString& InName, FUnaryOperation InOp)
{
	AddOpToNativeFunc(InName, 1, Ink::EValueType::Int, InOp);
}

void Ink::FNativeFunctionCall::AddFloatBinaryOp(const FString& InName, FBinaryOperation InOp)
{
	AddOpToNativeFunc(InName, 2, Ink::EValueType::Float, InOp);
}

void Ink::FNativeFunctionCall::AddFloatUnaryOp(const FString& InName, FUnaryOperation InOp)
{
	AddOpToNativeFunc(InName, 1, Ink::EValueType::Float, InOp);
}

void Ink::FNativeFunctionCall::AddStringBinaryOp(const FString& InName, FBinaryOperation InOp)
{
	AddOpToNativeFunc(InName, 2, Ink::EValueType::String, InOp);
}

void Ink::FNativeFunctionCall::AddListBinaryOp(const FString& InName, FBinaryOperation InOp)
{
	AddOpToNativeFunc(InName, 2, Ink::EValueType::List, InOp);
}

void Ink::FNativeFunctionCall::AddListUnaryOp(const FString& InName, FUnaryOperation InOp)
{
	AddOpToNativeFunc(InName, 1, Ink::EValueType::List, InOp);
}

void Ink::FNativeFunctionCall::AddDivertTargetBinaryOp(const FString& InName, FBinaryOperation InOp)
{
	AddOpToNativeFunc(InName, 2, Ink::EValueType::DivertTarget, InOp);
}
