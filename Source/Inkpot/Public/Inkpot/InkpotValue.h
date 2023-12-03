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
	/* Deafult value for returning from functions */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue DefaultInkpotValue();

	/* Create an Ink Value from a boolean */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeBoolInkpotValue(bool bValue);

	/* Cast Ink value to a boolean */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotValueAsBool(const FInkpotValue &Value);

	/* Cast Ink array value to a boolean */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotArrayValueAsBool(const TArray<FInkpotValue> &Values, int Index = 0);

	/* Create an Ink Value from an integer */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeIntInkpotValue(int32 Value);

	/* Cast Ink value to an integer*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotValueAsInt(const FInkpotValue &Value);

	/* Cast Ink array value to an integer */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotArrayValueAsInt(const TArray<FInkpotValue> &Values, int Index = 0);

	/* Create an Ink Value from a float */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeFloatInkpotValue(float Value);

	/* Cast Ink value to a float*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotValueAsFloat(const FInkpotValue &Value);

	/* Cast Ink array value to a float */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotArrayValueAsFloat(const TArray<FInkpotValue> &Values, int Index = 0);

	/* Create an Ink Value from a string */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeStringInkpotValue(const FString &Value);

	/* Cast Ink value to a string*/
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotValueAsString(const FInkpotValue &Value);

	/* Cast Ink array value to a string */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotArrayValueAsString(const TArray<FInkpotValue> &Values, int Index = 0);

	/* Create an Ink List from an array of Strings */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeInkpotList(const TArray<FString> &Value);
	static INKPOT_API FInkpotValue MakeInkpotNamedList(const FString &Name, const TArray<FString> &Value);

	/* Get an array of strings from an Ink List */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API TArray<FString> InkpotValueAsList(const FInkpotValue &Value);

	/* Cast Ink array value to a list */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API TArray<FString> InkpotArrayValueAsList(const TArray<FInkpotValue> &Values, int Index = 0);
};

