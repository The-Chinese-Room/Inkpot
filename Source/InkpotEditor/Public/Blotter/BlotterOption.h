#pragma once

#include "CoreMinimal.h"
#include "BlotterString.h"
#include "BlotterOption.generated.h"

UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterOption : public UBlotterString
{
	GENERATED_BODY()
public:
	void Set(const FString& Name, bool bIsSelected );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsSelected() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	ECheckBoxState GetState() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetSelected( bool bIsSelected );

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsSelected;
};
