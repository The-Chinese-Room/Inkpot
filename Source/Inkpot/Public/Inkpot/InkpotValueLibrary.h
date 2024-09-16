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
	/**
	 * DefaultInkpotValue
	 * Default value for returning from functions.
	 * 
	 * @return an empty inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue DefaultInkpotValue();

	/**
	 * MakeBoolInkpotValue
	 * Create an Inkpot Value from a boolean.
	 *
	 * @param bValue, the boolean to convert.
	 * @return a new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeBoolInkpotValue(bool bValue);

	/**
	 * InkpotValueAsBool
	 * Cast an Inkpot value to a boolean.
	 *
	 * @param Value, the Inkpot value to convert.
	 * @return a boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotValueAsBool(const FInkpotValue &Value);

	/**
	 * InkpotArrayValueAsBool
	 * Cast an Inkpot array value to a boolean.
	 *
	 * @param Values, the array of Inkpot values.
	 * @param Index, the indexed value in the array to convert.
	 * @return a boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotArrayValueAsBool(const TArray<FInkpotValue> &Values, int Index = 0);

	/* Create an Ink Value from an integer */
	/**
	 * MakeIntInkpotValue
	 * Create an Inkpot Value from an integer.
	 *
	 * @param Value, the integer to convert.
	 * @return a new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeIntInkpotValue(int32 Value);

	/**
	 * InkpotValueAsInt
	 * Cast Inkpot value to an integer.
	 *
	 * @param Value, the Inkpot value to convert.
	 * @return an integer.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotValueAsInt(const FInkpotValue &Value);

	/**
	 * InkpotArrayValueAsInt
	 * Cast an Inkpot array value to an integer.
	 *
	 * @param Values, the array of Inkpot values.
	 * @param Index, the indexed value in the array to convert.
	 * @return an integer.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotArrayValueAsInt(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * MakeFloatInkpotValue
	 * Create an Inkpot value from a floating point number.
	 *
	 * @param Value, the floating point number to convert.
	 * @return a new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeFloatInkpotValue(float Value);

	/**
	 * InkpotValueAsFloat
	 * Cast Inkpot value to a floating point number.
	 *
	 * @param Value, the Inkpot value to convert.
	 * @return a float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotValueAsFloat(const FInkpotValue &Value);

	/**
	 * InkpotArrayValueAsFloat
	 * Cast an Inkpot array value to a floating point number.
	 *
	 * @param Values, the array of Inkpot values.
	 * @param Index, the indexed value in the array to convert.
	 * @return a float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotArrayValueAsFloat(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * MakeStringInkpotValue
	 * Create an Inkpot value from a string.
	 *
	 * @param Value, the string to convert.
	 * @return a new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeStringInkpotValue(const FString &Value);

	/**
	 * InkpotValueAsString
	 * Cast Ink value to a string.
	 *
	 * @param Value, the Inkpot value to convert.
	 * @return a string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotValueAsString(const FInkpotValue &Value);

	/**
	 * InkpotArrayValueAsString
	 * Cast an Inkpot array value to a string.
	 *
	 * @param Values, the array of Inkpot values.
	 * @param Index, the indexed value in the array to convert.
	 * @return a string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotArrayValueAsString(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotValueAsList
	 * Cast Inkpot value to an InkpotList.
	 *
	 * @param Value, the Inkpot value to convert.
	 * @return an InkpotList.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotValueAsList( const FInkpotValue &Value );

	/**
	 * InkpotArrayValueAsList
	 * Cast Inkpot array value to an InkpotList.
	 *
	 * @param Values, the array of Inkpot values.
	 * @param Index, the indexed value in the array to convert.
	 * @return an InkpotList.
	 */

	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotArrayValueAsList( const TArray<FInkpotValue> &Values, int Index = 0 );
};

