#pragma once

#include "CoreMinimal.h"
#include "Ink/Value.h"
#include "InkpotValue.generated.h"

// a reference to the value type in the Ink VM 
USTRUCT(BlueprintType)
struct INKPOT_API FInkpotValue
{
	GENERATED_BODY()

public:
	FInkpotValue();

    // this sets as a reference of orginal 
	FInkpotValue(TSharedPtr<Ink::FValueType> InValue);

    // this sets as a copy of orginal 
	FInkpotValue(Ink::FValueType InValue);

	TSharedPtr<Ink::FValueType> operator *() const;

protected:
	TSharedPtr<Ink::FValueType> Value;
};
