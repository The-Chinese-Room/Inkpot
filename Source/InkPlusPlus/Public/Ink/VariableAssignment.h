#pragma once
#include "CoreMinimal.h"
#include "Object.h"

namespace Ink
{
	class INKPLUSPLUS_API FVariableAssignment : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FVariableAssignment; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FVariableAssignment;
		}

		FVariableAssignment();
		FVariableAssignment(const FString& InVariableName, bool InIsNewDeclaration);
		FVariableAssignment(const Ink::FVariableAssignment& InOtherVariableAssignment) = default;
		FVariableAssignment& operator=(const Ink::FVariableAssignment& InOtherVariableAssignment) = default;
		virtual ~FVariableAssignment() override = default;

		virtual FString ToString() override;

		FString GetVariableName() const;
		bool IsNewDeclaration() const;
		bool IsGlobal() const;
		void IsGlobal(bool InIsGlobal);
	
	private:
		FString VariableName;
		bool bIsNewDeclaration;
		bool bIsGlobal;
	};
}
