#pragma once

#include "Object.h"
#include "CoreMinimal.h"


namespace Ink
{
	class FTag : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FTag; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FTag;
		}

		FTag() = delete;
		FTag(const FString& InText);
		FTag(const FTag& InOtherTag) = default;
		FTag& operator=(const FTag& InOtherTag) = default;
		virtual ~FTag() override = default;

		virtual FString ToString() override;

		FString GetText() const;
		void SetText(const FString& InText);
	
	private:
		FString Text;
	};
}
