#pragma once

#include "CoreMinimal.h"
#include "InkpotLine.h"
#include "Ink/Choice.h"
#include "InkpotChoice.generated.h"

class UInkpotStory;

UCLASS(BlueprintType)
class INKPOT_API UInkpotChoice : public UInkpotLine
{
	GENERATED_BODY()

public:
	UInkpotChoice();
	void Initialise( TSharedPtr<Ink::FChoice>  InInkChoice );

	UFUNCTION(BlueprintPure, Category="Inkpot|Choice")
	int32 GetIndex() const;
	bool operator == (int32 Index);

	static const int32 BadChoice {-1};

private:
	virtual const TArray<FString>& GetTagsInternal() const override;

private:
	TSharedPtr<Ink::FChoice>  InkChoice;
};

