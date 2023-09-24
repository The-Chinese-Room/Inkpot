#pragma once

#include "CoreMinimal.h"
#include "Object.h"


namespace Ink
{
	class FPath;
	class FContainer;
	
	/*	The ChoicePoint represents the point within the Story where
		a Choice instance gets generated. The distinction is made
		because the text of the Choice can be dynamically generated. */
	
	class INKPLUSPLUS_API FChoicePoint : public FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FChoicePoint; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FChoicePoint;
		}

		FChoicePoint();
		FChoicePoint(bool InOnceOnly);
		FChoicePoint(const FChoicePoint& InOtherChoicePoint) = default;
		FChoicePoint& operator=(const FChoicePoint& InOtherChoicePoint) = default;
		virtual ~FChoicePoint() override = default;

		virtual FString ToString() override;

		int32 GetFlags() const;
		void SetFlags(int32 InFlags);

		TSharedPtr<Ink::FPath> GetPathOnChoice();
		void SetPathOnChoice(TSharedPtr<Ink::FPath> InPath);

		FString GetPathStringOnChoice();
		void SetPathStringOnChoice(const FString& InPathStringOnChoice);

		TSharedPtr<Ink::FContainer> GetChoiceTarget();
		
		bool HasCondition() const;
		void SetHasCondition(bool InHasCondition);
		bool HasStartContent() const;
		void SetHasStartContent(bool InHasStartContent);
		bool HasChoiceOnlyContent() const;
		void SetHasChoiceOnlyContent(bool InHasChoiceOnlyContent);
		bool IsOnceOnly() const;
		void SetIsOnceOnly(bool InIsOnceOnly);
		bool IsInvisibleDefault() const;
		void SetIsInvisibleDefault(bool InIsInvisibleDefault);
	
	private:

		//int32 Flags;
		
		bool bHasCondition;
		bool bHasStartContent;
		bool bHasChoiceOnlyContent;
		bool bIsOnceOnly;
		bool bIsInvisibleDefault;
		
		TSharedPtr<Ink::FPath> _PathOnChoice;
	};
}
