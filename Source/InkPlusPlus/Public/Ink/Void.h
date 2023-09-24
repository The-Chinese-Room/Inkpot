#pragma once
#include "CoreMinimal.h"
#include "Object.h"


namespace Ink
{
	class FVoid : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FVoid; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FVoid;
		}

		FVoid() = default;
		FVoid(const FVoid& InOtherVoid) = default;
		FVoid& operator=(const FVoid& InOtherVoid) = default;
		virtual ~FVoid() override = default;
	};
}