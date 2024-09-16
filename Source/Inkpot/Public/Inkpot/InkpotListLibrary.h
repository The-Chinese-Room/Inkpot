// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inkpot/InkpotList.h"
#include "InkpotListLibrary.generated.h"

UCLASS(meta=(BlueprintThreadSafe, ScriptName="InkpotListLibrary"), MinimalAPI)
class UInkpotListLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * ToString ( Inkpot List )
	 * prints the contents of the list to a comma delimeted string.
	 *
	 * @param Value, name of the list to print.
	 * @param ReturnValue, the list as a comma delimeted string of item names.
	 * @param bUseOrigin, should the item names have their origin as a dot delimeted prefix.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void ToString(const FInkpotList &Value, FString &ReturnValue, bool bUseOrigin = true );

	/**
	 * MakeInkpotList 
	 * Creates an Inkpot List consiting of defintion name and array of item names.
	 * If all items are dot prefixed with the origin name, defintion can be left blank.
	 * Actual origin reolution happens when the list is passed to the ink story.
	 * 
	 * @param Defintion, the origin, this will be the name of the list in the Ink script.
	 * @param Values, an array of strings for the items. Other list definitions can be included as dot prefixed item names.  
	 * @return An Inkpotlist. 
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotList(FString Definition, TArray<FString> Value);

	/**
	 * MakeInkpotListFromString
	 * Creates an Inkpot List consiting of defintion name and a comma delimeted string of item names.
	 * If all items are dot prefixed with the origin name, defintion can be left blank.
	 * Actual origin reolution happens when the list is passed to the ink story.
	 *
	 * @param Defintion, the origin, this will be the name of the list in the Ink script.
	 * @param Value, commas delimeted list of item names. Other list definitions can be included as dot prefixed item names.
	 * @return An Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotListFromString(FString Definition, FString Value);

	/**
	 * InkpotListAsStrings
	 * Converts an inkpot list to an array of strings.
	 *
	 * @param Value, list to convert.
	 * @param ReturnValue, the list as a string array of item names.
	 * @param bUseOrigin, should the item names have their origin as a dot delimeted prefix.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void InkpotListAsStrings(const FInkpotList &Value, TArray<FString> &ReturnValue, bool bUseOrigin = true );

	/**
	 * Union
	 * Returns a new list that is the combination of both lists passed in. 
	 * Equivalent to calling (A + B) in ink..
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Union", CommutativeAssociativeBinaryOperator = "true", CompactNodeTitle = "+"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Union(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Intersect
	 * Returns a new list that is the intersection of both lists passed in 
	 * - i.e. a list of the items that are shared between the two lists. 
	 * Equivalent to calling (A ^ B) in ink.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Intersect", CompactNodeTitle = "^"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Intersect(const FInkpotList &A, const FInkpotList &B);

	/**
	 * HasIntersection
	 * Returns true if there is an intersection of both lists passed in 
	 * - i.e. both lists share some items
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if there is an intersection, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Has Intersection", CompactNodeTitle = "(^)"), Category="Inkpot|List")
	static INKPOT_API bool HasIntersection(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Without
	 * Returns a new list that is the first list with items in the second list removed. 
	 * Equivalent to calling (A - B) in ink.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Without", CompactNodeTitle = "-"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Without( const FInkpotList &A, const FInkpotList &B );

	/**
	 * ContainsList
	 * Returns true if the the first list contains all the items that are in the second list. 
	 * Equivalent to calling (A ? B) in ink.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A contains B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Contains List", CompactNodeTitle = "?"), Category = "Inkpot|List" )
	static INKPOT_API bool ContainsList( const FInkpotList &Source, const FInkpotList &Querant );

	/*  */
	/**
	 * ContainsList
	 * Returns true if the list contains an item matching the given name.
	 *
	 * @param Source, InkpotList to test.
	 * @param ItemName, string, item name.
	 * @return true if Source contains ItemName, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Contains Item", CompactNodeTitle = "?"), Category="Inkpot|List")
	static INKPOT_API bool ContainsItem(const FInkpotList &Source, const FString &ItemName);

	/**
	 * GreaterThan
	 * Returns true if all the item values in the first list are greater than all the item values in the second list. 
	 * Equivalent to calling (A > B) in ink.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A > B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater Than", CompactNodeTitle = ">"), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThan(const FInkpotList &A, const FInkpotList &B);

	/**
	 * GreaterThanOrEquals
	 * Returns true if the item values in the first list overlap or are all greater than the item values in the second list. 
	 * None of the item values in the current list must fall below the item values in the passed in list. 
	 * Equivalent to (A >= B) in ink, or LIST_MIN(A) >= LIST_MIN(B) && LIST_MAX(A) >= LIST_MAX(B).
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A >= B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater than or equals", CompactNodeTitle = ">="), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/**
	 * LessThan
	 * Returns true if all the item values first list are less than all the item values in the second list. 
	 * Equivalent to calling (A < B) in ink.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A < B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than", CompactNodeTitle = "<"), Category="Inkpot|List")
	static INKPOT_API bool LessThan(const FInkpotList &A, const FInkpotList &B);

	/**
	 * LessThanOrEquals
	 * Returns true if the item values in the first list overlap or are all less than the item values in the second list. 
	 * None of the item values in the current list must go above the item values in the passed in list. 
	 * Equivalent to (A <= B) in ink, or LIST_MAX(A) <= LIST_MAX(B) && LIST_MIN(A) <= LIST_MIN(B).
	 * 
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A <= B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than or equals", CompactNodeTitle = "<="), Category="Inkpot|List")
	static INKPOT_API bool LessThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Equals
	 * Returns true if the both lists contain the same items, false otherwise.
	 *
	 * @param A, InkpotList.
	 * @param B, InkpotList.
	 * @return true if A == B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Equals", CompactNodeTitle = "=="), Category="Inkpot|List")
	static INKPOT_API bool Equals(const FInkpotList &A, const FInkpotList &B);
};
