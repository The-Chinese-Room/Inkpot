#pragma once

#include "CoreMinimal.h"
#include "InkpotChoice.generated.h"

class UInkpotStory;

UCLASS(BlueprintType)
class INKPOT_API UInkpotChoice : public UObject
{
	GENERATED_BODY()

public:
	UInkpotChoice();
	void Initialise( int32 Index, const FString &String);

	UFUNCTION(BlueprintPure, Category="Inkpot|Choice")
	int32 GetIndex() const;
	bool operator == (int32 Index);

	UFUNCTION(BlueprintPure, Category="Inkpot|Choice")
	const FString& GetString() const;

	UFUNCTION(BlueprintPure, Category="Inkpot|Choice")
	UInkpotStory* GetStory() const;

	static const int32 BadChoice {-1};

private:

	UPROPERTY(VisibleAnywhere, Category="Inkpot|Choice")
	int32 Index;

	UPROPERTY(VisibleAnywhere, Category="Inkpot|Choice")
	FString String;
	
};

