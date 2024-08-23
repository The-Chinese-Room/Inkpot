#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ink/Object.h"
#include "BlotterVariable.generated.h"


UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterVariable : public UObject
{
	GENERATED_BODY()
public:

	//UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetValue(const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetValue() const;

	//UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetType(const FText& Type);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetType() const;

	void SetType(TSharedPtr<Ink::FObject> InObj);

protected:
	FText GetTypeName(TSharedPtr<Ink::FObject> InObj);

private:
	UPROPERTY(VisibleAnywhere)
	FText Name;

	UPROPERTY(VisibleAnywhere)
	FText	Value;

	UPROPERTY(VisibleAnywhere)
	FText	Type;
};
