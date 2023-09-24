#pragma once

#include "CoreMinimal.h"
#include "Ink/Object.h"


namespace Ink
{
	class INKPLUSPLUS_API FGlue : public FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FGlue; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FGlue;
		}

		FGlue() = default;
		virtual ~FGlue() override = default;

		virtual FString ToString() override {return FString(TEXT("Glue"));};
	};
}
