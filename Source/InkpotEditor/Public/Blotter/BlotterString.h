#pragma once

#include "CoreMinimal.h"
#include "BlotterString.generated.h"

UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterString : public UObject
{
	GENERATED_BODY()
public:
	void Set(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetText() const;

private:
	UPROPERTY(VisibleAnywhere)
	FText Text;
};
