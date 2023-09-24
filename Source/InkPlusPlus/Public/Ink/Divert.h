#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "PushPop.h"
#include "Pointer.h"


namespace Ink
{
	class FPath;
	
	class INKPLUSPLUS_API FDivert : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FDivert; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FDivert;
		}

		FDivert();
		FDivert(const Ink::EPushPopType& InStackPushType);
		FDivert(const Ink::FDivert& InOtherDivert) = default;
		virtual ~FDivert() override = default;

		virtual FString ToString() override;
		bool Equals(Ink::FDivert& InOtherDivert);
		virtual int32 GetHashCode() override;
		
		TSharedPtr<Ink::FPath> GetTargetPath();
		void SetTargetPath(TSharedPtr<Ink::FPath> InTargetPath);

		FString GetTargetPathString();
		void SetTargetPathString(const FString& InTargetPathString);

		FPointer GetTargetPointer();

		bool HasVariableTarget() const;
		
		FString GetVariableDivertName() const;
		void SetVariableDivertName(const FString& InVariableDivertName);

		bool GetPushesToStack() const;
		void SetPushesToStack(bool InPushesToStack);

		bool IsExternal() const;
		void SetIsExternal(bool InIsExternal);

		bool IsConditional() const;
		void SetIsConditional(bool InIsConditional);

		EPushPopType GetStackPushType() const;
		void SetStackPushType(const Ink::EPushPopType& InStackPushType);
		
		int32 GetExternalsArgs() const;
		void SetExternalArgs(int32 InExternalArgs);
	
	private:
		FString VariableDivertName;
		bool bPushesToStack;
		bool bIsExternal;
		bool bIsConditional;
		int32 ExternalArgs;
		Ink::EPushPopType StackPushType;
		
		mutable TSharedPtr<Ink::FPath> _TargetPath;
		mutable FPointer _TargetPointer;
	};
}

