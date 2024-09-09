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

	/* prints the contents of the list to screen for debug */
	UFUNCTION(BlueprintCallable, Category="Inkpot|List")
	static INKPOT_API void PrintToScreen(const FInkpotList &Value, bool bUseOrigin = true );

	/* prints the contents of the list to the ouput log for debug */
	UFUNCTION(BlueprintCallable, Category="Inkpot|List")
	static INKPOT_API void PrintToLog(const FInkpotList &Value, bool bUseOrigin = true );

	/* Create an Ink List from an array of Strings */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotList(FString Definition, TArray<FString> Value);

	/* Create an Ink List from a single string */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotSingleEntryList(FString Definition, FString Value);

	/* Get an array of strings from an Ink List */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void InkpotListAsStrings(const FInkpotList &Value, TArray<FString> &ReturnValue, bool bUseOrigin = true );

	/* Returns a new list that is the combination of both lists passed in. Equivalent to calling (list1 + list2) in ink. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Union", CommutativeAssociativeBinaryOperator = "true", CompactNodeTitle = "+"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Union(const FInkpotList &A, const FInkpotList &B);

	/* Returns a new list that is the intersection of both lists passed in - i.e. a list of the items that are shared between the two lists. Equivalent to calling (list1 ^ list2) in ink. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Intersect", CompactNodeTitle = "^"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Intersect(const FInkpotList &A, const FInkpotList &B);

	/* Returns true if there is an intersection of both lists passed in - i.e. both lists share some items. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Has Intersection", CompactNodeTitle = "(^)"), Category="Inkpot|List")
	static INKPOT_API bool HasIntersection(const FInkpotList &A, const FInkpotList &B);

	/* Returns a new list that is the first list with items in the second list removed. Equivalent to calling (list1 - list2) in ink. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Without", CompactNodeTitle = "-"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Without( const FInkpotList &A, const FInkpotList &B );

	/* Returns true if the the first list contains all the items that are in the second list. Equivalent to calling (list1 ? list2) in ink. */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Contains List", CompactNodeTitle = "?"), Category = "Inkpot|List" )
	static INKPOT_API bool ContainsList( const FInkpotList &Source, const FInkpotList &Querant );

	/* Returns true if the list contains an item matching the given name. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Contains Item", CompactNodeTitle = "?"), Category="Inkpot|List")
	static INKPOT_API bool ContainsItem(const FInkpotList &Source, const FString &ItemName);

	/* Returns true if all the item values in the first list are greater than all the item values in the second list. Equivalent to calling (list1 > list2) in ink. */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater Than", CompactNodeTitle = ">"), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThan(const FInkpotList &A, const FInkpotList &B);

	/* Returns true if the item values in the first list overlap or are all greater than the item values in the second list. None of the item values in the current list must fall below the item values in the passed in list. Equivalent to (list1 >= list2) in ink, or LIST_MIN(list1) >= LIST_MIN(list2) &amp;&amp; LIST_MAX(list1) >= LIST_MAX(list2). */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater than or equals", CompactNodeTitle = ">="), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/* Returns true if all the item values first list are less than all the item values in the second list. Equivalent to calling (list1 < list2) in ink. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than", CompactNodeTitle = "<"), Category="Inkpot|List")
	static INKPOT_API bool LessThan(const FInkpotList &A, const FInkpotList &B);

	/* Returns true if the item values in the first list overlap or are all less than the item values in the second list. None of the item values in the current list must go above the item values in the passed in list. Equivalent to (list1 <= list2) in ink, or LIST_MAX(list1) <= LIST_MAX(list2) && LIST_MIN(list1) <= LIST_MIN(list2). */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than or equals", CompactNodeTitle = "<="), Category="Inkpot|List")
	static INKPOT_API bool LessThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/* Returns true if the both lists contain the same items, false otherwise. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Equals", CompactNodeTitle = "=="), Category="Inkpot|List")
	static INKPOT_API bool Equals(const FInkpotList &A, const FInkpotList &B);
};
