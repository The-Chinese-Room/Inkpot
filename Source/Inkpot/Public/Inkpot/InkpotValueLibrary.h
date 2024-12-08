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
	 * @returns an empty inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue DefaultInkpotValue();

	/**
	 * MakeBoolInkpotValue
	 * Create an Inkpot Value from a boolean.
	 *
	 * @param bValue - The boolean to convert.
	 * @returns A new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeBoolInkpotValue(bool bValue);

	/**
	 * IsInkpotValueBool
	 * Tests if the Value is a boolean.
	 *
	 * @param Value - The Inkpot value to test.
	 * @returns true if the value is a a boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotValueBool(const FInkpotValue &Value);

	/**
	 * InkpotValueAsBool
	 * Cast an Inkpot value to a boolean.
	 *
	 * @param Value - The Inkpot value to convert.
	 * @returns A boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotValueAsBool(const FInkpotValue &Value);

	/**
	 * IsInkpotArrayValueBool
	 * Tests if the array Value is a boolean.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to test.
	 * @returns true if the value is a a boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotArrayValueBool(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotArrayValueAsBool
	 * Cast an Inkpot array value to a boolean.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to convert.
	 * @returns A boolean.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool InkpotArrayValueAsBool(const TArray<FInkpotValue> &Values, int Index = 0);

	/*
	 * MakeIntInkpotValue
	 * Create an Inkpot Value from an integer.
	 *
	 * @param Value - The integer to convert.
	 * @returns A new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeIntInkpotValue(int32 Value);

	/**
	 * IsInkpotValueInt
	 * Tests if the Value is an integer.
	 *
	 * @param Value - The Inkpot value to test.
	 * @returns true if the value is an integer.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotValueInt(const FInkpotValue &Value);

	/**
	 * InkpotValueAsInt
	 * Cast Inkpot value to an integer.
	 *
	 * @param Value - The Inkpot value to convert.
	 * @returns An integer.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotValueAsInt(const FInkpotValue &Value);

	/**
	 * IsInkpotArrayValueInt
	 * Tests if the array Value is an integer.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to test.
	 * @returns true if the value is an int.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotArrayValueInt(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotArrayValueAsInt
	 * Cast an Inkpot array value to an integer.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to convert.
	 * @returns An integer.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API int32 InkpotArrayValueAsInt(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * MakeFloatInkpotValue
	 * Create an Inkpot value from a floating point number.
	 *
	 * @param Value - The floating point number to convert.
	 * @returns A new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeFloatInkpotValue(float Value);

	/**
	 * IsInkpotValueFloat
	 * Tests if the Value is a floating point number.
	 *
	 * @param Value - The Inkpot value to test.
	 * @returns true if the value is a float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotValueFloat(const FInkpotValue &Value);


	/**
	 * InkpotValueAsFloat
	 * Cast Inkpot value to a floating point number.
	 *
	 * @param Value - The Inkpot value to convert.
	 * @returns A float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotValueAsFloat(const FInkpotValue &Value);

	/**
	 * IsInkpotArrayValueFloat
	 * Tests if the array Value is a floating point value.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to test.
	 * @returns true if the value is a float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotArrayValueFloat(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotArrayValueAsFloat
	 * Cast an Inkpot array value to a floating point number.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to convert.
	 * @returns A float.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API float InkpotArrayValueAsFloat(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * MakeStringInkpotValue
	 * Create an Inkpot value from a string.
	 *
	 * @param Value - The string to convert.
	 * @returns A new inkpot value.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotValue MakeStringInkpotValue(const FString &Value);

	/**
	 * IsInkpotValueString
	 * Tests if the Value is a string.
	 *
	 * @param Value - The Inkpot value to test.
	 * @returns true if the value is a string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotValueString(const FInkpotValue &Value);

	/**
	 * InkpotValueAsString
	 * Cast Ink value to a string.
	 *
	 * @param Value - The Inkpot value to convert.
	 * @returns A string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotValueAsString(const FInkpotValue &Value);

	/**
	 * IsInkpotArrayValueString
	 * Tests if the array Value is a string.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to test.
	 * @returns true if the value is a string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotArrayValueString(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotArrayValueAsString
	 * Cast an Inkpot array value to a string.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to convert.
	 * @returns A string.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FString InkpotArrayValueAsString(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * IsInkpotValueList
	 * Tests if the Value is a List.
	 *
	 * @param Value - The Inkpot value to test.
	 * @returns true if the value is a List.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotValueList(const FInkpotValue &Value);

	/**
	 * InkpotValueAsList
	 * Cast Inkpot value to an InkpotList.
	 *
	 * @param Value - The Inkpot value to convert.
	 * @returns An InkpotList.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotValueAsList( const FInkpotValue &Value );

	/**
	 * IsInkpotArrayValueList
	 * Tests if the array Value is a list.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to test.
	 * @returns true if the value is a list.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API bool IsInkpotArrayValueList(const TArray<FInkpotValue> &Values, int Index = 0);

	/**
	 * InkpotArrayValueAsList
	 * Cast Inkpot array value to an InkpotList.
	 *
	 * @param Values - The array of Inkpot values.
	 * @param Index - The indexed value in the array to convert.
	 * @returns An InkpotList.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Value")
	static INKPOT_API FInkpotList InkpotArrayValueAsList( const TArray<FInkpotValue> &Values, int Index = 0 );
};

