#pragma once

#include "CoreMinimal.h"
#include "Ink/InkList.h"
#include "Inkpot/InkpotValue.h"
#include "InkpotList.generated.h"

class UInkpotStory;

USTRUCT(BlueprintType)
struct INKPOT_API FInkpotList : public FInkpotValue
{
	GENERATED_BODY()
public:
	FInkpotList();
	FInkpotList(TSharedPtr<Ink::FValueType> InValue);

	Ink::FInkList& GetList() const;

	void ToStringArray( TArray<FString>& OutValues, bool bUseOrigin = true ) const;

	bool ValidateOrigin( UInkpotStory *InStory ) const;
};
