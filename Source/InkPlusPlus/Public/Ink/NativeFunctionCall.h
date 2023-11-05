#pragma once
#include "CoreMinimal.h"
#include "Containers/Union.h"

#include "Object.h"
#include "Value.h"
#include "Utility/InkPlusPlusLog.h"


namespace Ink
{
	class FInkList;
	
	using FNativeFunctionOp = TUnion<int32, float, bool, FString, TSharedPtr<Ink::FPath>, Ink::FInkList>;
	typedef TFunction<FNativeFunctionOp(FNativeFunctionOp InLeft, FNativeFunctionOp InRight)> FBinaryOperation;
	typedef TFunction<FNativeFunctionOp(FNativeFunctionOp InValue)> FUnaryOperation;
	
	
	class INKPLUSPLUS_API FNativeFunctionCall : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FNativeFunctionCall; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FNativeFunctionCall;
		}

		FNativeFunctionCall();
		FNativeFunctionCall(const FString& InName);
		FNativeFunctionCall(const FString& InName, int32 InNumberOfParameters);
		FNativeFunctionCall(const Ink::FNativeFunctionCall& InOtherNativeFunctionCall) = default;
		FNativeFunctionCall& operator=(const Ink::FNativeFunctionCall& InOtherNativeFunctionCall) = default;
		virtual ~FNativeFunctionCall() override = default;

		virtual FString ToString() override;
		
		TSharedPtr<Ink::FObject> Call(const TArray<TSharedPtr<Ink::FObject>>& InParameters);

		FString GetName() const override;
		void SetName(const FString& InName) override;
		void SetNameInternal(const FString& InName);
		int32 GetNumberOfParameters() const;

		static TSharedPtr<Ink::FNativeFunctionCall> CallWithName(const FString& InFunctionName);
		static bool CallExistsWithName(const FString& InFunctionName);
	
	private:
		// Only called internally to generate prototypes
		void SetNumberOfParameters(int32 InNumberOfParameters);
		
		TSharedPtr<Ink::FValue> CallBinaryListOperation(const TArray<TSharedPtr<Ink::FObject>>& InParameters);
		TSharedPtr<Ink::FValue> CallListIncrementOperation(const TArray<TSharedPtr<Ink::FObject>>& InListIntParameters);

		TArray<TSharedPtr<Ink::FValue>> CoerceValuesToSingleType(const TArray<TSharedPtr<Ink::FObject>>& InParameters);

		static void GenerateNativeFunctionsIfNecessary();
	
	private:
		void AddOpFuncForType(const Ink::EValueType& InValueType, FBinaryOperation InOp);
		void AddOpFuncForType(const Ink::EValueType& InValueType, FUnaryOperation InOp);

		static void AddOpToNativeFunc(const FString& InName, int32 InArgs, const Ink::EValueType& InValueType, FBinaryOperation InOp);
		static void AddOpToNativeFunc(const FString& InName, int32 InArgs, const Ink::EValueType& InValueType, FUnaryOperation InOp);
		
		static void AddIntBinaryOp(const FString& InName, FBinaryOperation InOp);
		static void AddIntUnaryOp(const FString& InName, FUnaryOperation InOp);
		static void AddFloatBinaryOp(const FString& InName, FBinaryOperation InOp);
		static void AddFloatUnaryOp(const FString& InName, FUnaryOperation InOp);
		static void AddStringBinaryOp(const FString& InName, FBinaryOperation InOp);
		static void AddListBinaryOp(const FString& InName, FBinaryOperation InOp);
		static void AddListUnaryOp(const FString& InName, FUnaryOperation InOp);
		static void AddDivertTargetBinaryOp(const FString& InName, FBinaryOperation InOp);

	private:
		template<class T>
		TSharedPtr<Ink::FValue> Call(const TArray<TSharedPtr<Ink::FValue>>& InParametersOfSingleType)
		{
			if (InParametersOfSingleType.Num() == 0)
			{
				UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : passed in parameters list of single type is empty!"))
				return nullptr;
			}
			
			const TSharedPtr<Ink::FValue> param1 = InParametersOfSingleType[0];
			const Ink::EValueType valType = param1->GetType();

			const TSharedPtr<Ink::TValue<T>> value1 = DynamicCastTo<Ink::TValue<T>>(param1);
			if (!value1.IsValid())
			{
				UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : could not convert value to the type of the template!"))
				return nullptr;
			}
			
			const int32 paramCount = InParametersOfSingleType.Num();
			const bool paramCountIsValid = paramCount == 1 || paramCount == 2;
			if (paramCountIsValid == false)
			{
				UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : count of the passed in parameters list has invalid range!"))
				return nullptr;
			}

			// Check binary & unary operations for the function
			FBinaryOperation* findResult = BinaryOperations.Find(valType);
			FBinaryOperation binaryOpForTypeObj = (findResult != nullptr) ? *findResult : nullptr;
			FUnaryOperation unaryOpForTypeObj = nullptr;

			// Try to find unary operation, as could not find binary one
			if (binaryOpForTypeObj == nullptr)
			{
				unaryOpForTypeObj = *UnaryOperations.Find(valType);
				if (unaryOpForTypeObj == nullptr)
				{
					StoryException.Throw(TEXT("NativeFunctionCall : could not find both binary or unary function '" + this->Name +"' for the value type"));
					return nullptr;
				}
			}
			
			// Binary Operation
			if (paramCount == 2 && binaryOpForTypeObj)
			{
				Ink::FValue* param2 = InParametersOfSingleType[1].Get();
				const Ink::TValue<T>* value2 = static_cast<Ink::TValue<T>*>(param2);

				// Return value unknown until it's evaluated
				const FNativeFunctionOp result = binaryOpForTypeObj(FNativeFunctionOp(value1->GetValue()), FNativeFunctionOp(value2->GetValue()));
				return Ink::FValue::Create(result);
			}
			
			// Unary Operation
			if (paramCount == 1 && unaryOpForTypeObj)
			{
				const FNativeFunctionOp result = unaryOpForTypeObj(FNativeFunctionOp(value1->GetValue()));
				return Ink::FValue::Create(result);
			}

			UE_LOG(InkPlusPlus, Error, TEXT("NativeFunctionCall : could not match with either binary or unary function!"));
			return nullptr;
		}
	
	private:
		FString Name;
		int32 NumberOfParameters;

		bool bIsPrototype;
		TSharedPtr<Ink::FNativeFunctionCall> Prototype;		
		
		// Operations for each data type, for a single operation (e.g. "+")
		TMap<Ink::EValueType, FBinaryOperation> BinaryOperations;
		TMap<Ink::EValueType, FUnaryOperation> UnaryOperations;
		
		static TMap<FString, TSharedPtr<Ink::FNativeFunctionCall>> NativeFunctions;
	
		static FString Add;
		static FString Subtract;
		static FString Divide;
		static FString Multiply;
		static FString Mod;
		static FString Negate;

		static FString Equal;
		static FString Greater;
		static FString Less;
		static FString GreaterThanOrEquals;
		static FString LessThanOrEquals;
		static FString NotEquals;

		static FString Not;
		static FString And;
		static FString Or;

		static FString Min;
		static FString Max;
		static FString Pow;
		static FString Floor;
		static FString Ceiling;

		static FString Int;
		static FString Float;

		static FString Has;
		static FString Hasnt;
		static FString Intersect;

		static FString ListMin;
		static FString ListMax;
		static FString All;
		static FString Count;
		static FString ValueOfList;
		static FString Invert;
	};
}
