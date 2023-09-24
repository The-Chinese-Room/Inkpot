#pragma once
#include "CoreMinimal.h"
#include "Object.h"

namespace Ink
{
	class FPath;
	class FContainer;
	
	class FVariableReference : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FVariableReference; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FVariableReference;
		}

		FVariableReference();
		FVariableReference(const FString& InName);
		FVariableReference(const Ink::FVariableReference& InOtherVariableReference) = default;
		FVariableReference& operator=(const Ink::FVariableReference& InOtherVariableReference) = default;
		virtual ~FVariableReference() override = default;

		virtual FString ToString() override;

		FString GetName() const;
		void SetName(const FString& InName);
		
		FString GetPathStringForCount();
		void SetPathStringForCount(const FString& InPathString);
		
		TSharedPtr<Ink::FContainer> GetContainerForCount();

	private:
		// Normal named variable
		FString Name;

		// Variable reference is actually a path for a visit (read) count
		TSharedPtr<Ink::FPath> PathForCount;
	};	
}
