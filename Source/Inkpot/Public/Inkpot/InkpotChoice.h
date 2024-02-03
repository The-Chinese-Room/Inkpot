#pragma once

#include "CoreMinimal.h"
#include "InkpotLine.h"
#include "InkpotChoice.generated.h"

class UInkpotStory;

UCLASS(BlueprintType)
class INKPOT_API UInkpotChoice : public UInkpotLine
{
	GENERATED_BODY()

public:
	UInkpotChoice();
	void Initialise( int32 Index, const FString &String);

	UFUNCTION(BlueprintPure, Category="Inkpot|Choice")
	int32 GetIndex() const;
	bool operator == (int32 Index);

	static const int32 BadChoice {-1};

private:

	UPROPERTY(VisibleAnywhere, Category="Inkpot|Choice")
	int32 Index;
};

