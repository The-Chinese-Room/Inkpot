#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotValue.h"
#include "Inkpot/InkpotList.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InkpotValueLibrary.generated.h"


UCLASS(meta=(ScriptName="InkpotValueLibrary"), MinimalAPI)
class UInkpotValueLibrary : public UBlueprintFunctionLibrary
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

	/* Cast Inkpot value to an InkpotList */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotValueAsList( const FInkpotValue &Values );

	/* Cast Ink array value to an InkpotList */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotArrayValueAsList( const TArray<FInkpotValue> &Values, int Index = 0 );
};

