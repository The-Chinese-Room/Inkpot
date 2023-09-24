#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Containers/Union.h"

#include "Object.h"
#include "InkList.h"


namespace Ink
{
	using FValueType = TUnion<int32, float, bool, FString, TSharedPtr<Ink::FPath>, Ink::FInkList>;
	
	class FPath;
	struct FInkListItem;
	
	/*	Order is significant for type coersion.
		If types aren't directly compatible for an operation,
		they're coerced to the same type, downward.
		Higher value types "infect" an operation.
		(This may not be the most sensible thing to do, but it's worked so far!)	*/
	enum class INKPLUSPLUS_API EValueType
	{
		/*	Bool is new addition, keep enum values the same, with Int==0, Float==1 etc,
			but for coersion rules, we want to keep bool with a lower value than Int
			so that it converts in the right direction										*/
		Bool = -1,
		// Used in coersion
		Int,
		Float,
		List,
		String,

		// Not used for coersion described above
		DivertTarget,
		VariablePointer
	};
	
	class INKPLUSPLUS_API FValue : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValue; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue;
		}

		FValue() = default;
		FValue(const Ink::FValue& InOtherValue) = default;
		FValue& operator=(const Ink::FValue& InOtherValue) = default;;
		virtual ~FValue() override = default;;

		virtual bool IsTruthy() const;
		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType);
		virtual FValueType GetValueObject() const;
		
		EValueType GetType() const;
		
		static TSharedPtr<Ink::FValue> Create(const FValueType& InValue);

	protected:
		void BadCastException(const EValueType& InTargetType);
		FString ValueTypeAsString(const EValueType& InTargetType) const;
	
	protected:
		EValueType Type;
	};

	template <class T>
	class INKPLUSPLUS_API TValue : public Ink::FValue
	{
	public:
		T& GetValue() { return Value; }
		const T& GetValue() const { return Value; }
		void SetValue(const T& InValue) { Value = InValue; }
	
	protected:
		T Value;
	};

#pragma region ValueBool
	class INKPLUSPLUS_API FValueBool : public Ink::TValue<bool>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueBool; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueBool;
		}

		FValueBool();
		FValueBool(bool InValue);
		virtual ~FValueBool() override = default;

		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;
	};
#pragma endregion ValueBool

#pragma region ValueInt
	class INKPLUSPLUS_API FValueInt final : public Ink::TValue<int32>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueInt; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueInt;
		}

		FValueInt();
		FValueInt(int32 InValue);
		virtual ~FValueInt() override = default;

		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;
	};
#pragma endregion ValueInt

#pragma region ValueFloat
	class INKPLUSPLUS_API FValueFloat final : public Ink::TValue<float>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueFloat; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueFloat;
		}

		FValueFloat();
		FValueFloat(float InValue);
		virtual ~FValueFloat() override = default;

		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;
	};
#pragma endregion ValueFloat

#pragma region ValueString
	class INKPLUSPLUS_API FValueString final : public Ink::TValue<FString>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueString; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueString;
		}

		FValueString();
		FValueString(const FString& InValue);
		virtual ~FValueString() override = default;
		
		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;

		bool IsNewLine() const;
		bool IsInlineWhitespace() const;
		bool IsNonWhitespace() const;
	
	private:
		bool bIsNewLine;
		bool bIsInlineWhitespace;
	};
#pragma endregion ValueString

#pragma region ValueDivertTarget
	class INKPLUSPLUS_API FValueDivertTarget final : public Ink::TValue<TSharedPtr<Ink::FPath>>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueDivertTarget; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueDivertTarget;
		}

		FValueDivertTarget();
		FValueDivertTarget(TSharedPtr<Ink::FPath> InValue);
		virtual ~FValueDivertTarget() override = default;

		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;
	};
#pragma endregion ValueDivertTarget

#pragma region ValueVariablePointer
	class INKPLUSPLUS_API FValueVariablePointer final : public Ink::TValue<FString>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueVariablePointer; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueVariablePointer;
		}

		FValueVariablePointer();
		FValueVariablePointer(const FString& InValue, int32 InContextIndex = -1);
		virtual ~FValueVariablePointer() override = default;

		const FString& GetVariableName() const;
		void SetVariableName(const FString& VariableName);

		virtual bool IsTruthy() const override;
		virtual FValueType GetValueObject() const override;

		virtual TSharedPtr<Ink::FValue> Cast(const EValueType& InNewType) override;
		virtual FString ToString() override;

		int32 GetContextIndex() const;
		void SetContextIndex(int32 InContextIndex);
	
	private:
		int32 ContextIndex;
	};
#pragma endregion ValueVariablePointer

#pragma region ValueInkList
class INKPLUSPLUS_API FValueList final : public Ink::TValue<Ink::FInkList>
{
public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FValueList; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FValue
				|| inkClass == EInkObjectClass::FValueList;
		}

	FValueList();
	FValueList(const Ink::FInkList& InList);
	FValueList(Ink::FInkListItem InItem, int32 InSingleValue);
	virtual ~FValueList() override = default;

	virtual bool IsTruthy() const override;
	virtual FValueType GetValueObject() const override;
	virtual FString ToString() override;

	static void RetainListOriginsForAssignment(TSharedPtr<Ink::FObject> InOldValue, TSharedPtr<Ink::FObject> InNewValue);
};
#pragma endregion ValueInkList
}
