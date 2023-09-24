#pragma once

#include "Coreminimal.h"
#include "Ink/Value.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InkpotValue.generated.h"


USTRUCT(BlueprintType)
struct INKPOT_API FInkpotValue
{
	GENERATED_BODY()

public:
	FInkpotValue();
	FInkpotValue(TSharedPtr<Ink::FValueType> InValue);
	FInkpotValue(Ink::FValueType InValue);

	TSharedPtr<Ink::FValueType> operator *() const;

private:
	TSharedPtr<Ink::FValueType> Value;
};


UCLASS(meta=(ScriptName="InkpotLibrary"), MinimalAPI)
class UInkpotLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Create an Ink Value from a boolean */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeBoolInkpotValue(bool bValue);

	/* Cast Ink value to a boolean */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotValueAsBool(FInkpotValue Value);

	/* Create an Ink Value from an integer */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeIntInkpotValue(int32 Value);

	/* Cast Ink value to an integer*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotValueAsInt(FInkpotValue Value);

	/* Create an Ink Value from a float */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeFloatInkpotValue(float Value);

	/* Cast Ink value to a float*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotValueAsFloat(FInkpotValue Value);

	/* Create an Ink Value from a string */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeStringInkpotValue(const FString &Value);

	/* Cast Ink value to a string*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotValueAsString(FInkpotValue Value);

	/* Create an Ink List from an array of Strings */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeInkpotList(const TArray<FString> &Value);
	static INKPOT_API FInkpotValue MakeInkpotNamedList(const FString &Name, const TArray<FString> &Value);

	/* Get an array of strings from an Ink List */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API TArray<FString> InkpotValueAsList(FInkpotValue Value);
};

