#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Inkpot/InkpotList.h"
#include "InkpotListLibrary.generated.h"

class UInkpotStory;

UCLASS(meta=(BlueprintThreadSafe, ScriptName="InkpotListLibrary"), MinimalAPI)
class UInkpotListLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * ToString ( Inkpot List )
	 * prints the contents of the list to a comma delimeted string.
	 *
	 * @param Value - The list to convert.
	 * @param ReturnValue - The list as a comma delimeted string of item names.
	 * @param bUseOrigin - Should the item names have their origin as a dot delimeted prefix.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void ToString(const FInkpotList &Value, FString &ReturnValue, bool bUseOrigin = true );

	/**
	 * MakeInkpotList 
	 * Creates an Inkpot List from the origin name and a comma delimeted string of item names.
	 * 
	 * Note: 
	 * If all items are dot prefixed with the origin name, origin can be left empty.
	 *
	 * @param Story - The story in which the origin is defined.
	 * @param Origin - The origin, this will be the name of the list in the Ink script. May be empty if all items specify origin.
	 * @param Value - A comma delimeted list of item names. Item names may have a dot delimeted origin name prefix from other origins.  
	 * @returns An Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotList(UInkpotStory *Story, FString Origin, FString Value);

	/**
	 * MakeInkpotListFromStringArray
	 * Creates an Inkpot List from the origin name and an array of item names.
	 * 
	 * Note: 
	 * If all items are dot prefixed with the origin name, origin can be left empty.
	 * 
	 * @param Story - The story in which the origin is defined.
	 * @param Origin - The origin, this will be the name of the list in the Ink script. May be empty if all items specify origin.
	 * @param Values - An array of strings for the item names. Item names may have a dot delimeted origin name prefix from other origins.  
	 * @returns A new Inkpotlist. 
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotListFromStringArray(UInkpotStory *Story, FString Origin,  TArray<FString> Values);

	/**
	 * MakeInkpotListFromGameplayTag
	 * Creates an Inkpot List from a gameplay tag.
	 * 
	 * @param Story - The story in which the origin is defined.
	 * @param Tag - A single gameplay tag, tags need to be generated and added to the project.  
	 * @returns A new Inkpotlist. 
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotListFromGameplayTag(UInkpotStory *Story, UPARAM(meta = (Categories = "Ink.Origin")) FGameplayTag Tag);

	/**
	 * ToGameplayTag
	 * Creates a gameplay tag from an inkpot list.
	 * 
	 * @param Value - The list to convert.
	 * @param ReturnValue - A single gameplay tag, tags need to be generated and added to the project.  
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void ToGameplayTag(const FInkpotList &Value, FGameplayTag &ReturnValue);

	/**
	 * MakeInkpotListFromGameplayTags
	 * Creates an Inkpot List from a gameplay tag collection.
	 * 
	 * @param Story - The story in which the origin is defined.
	 * @param Tags - A gameplay tag container, tags need to be generated and added to the project.  
	 * @returns A new Inkpotlist. 
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API FInkpotList MakeInkpotListFromGameplayTags(UInkpotStory *Story, UPARAM(meta = (Categories = "Ink.Origin")) FGameplayTagContainer Tags);

	/**
	 * ToGameplayTag
	 * Creates a gameplay tag from an inkpot list.
	 * 
	 * @param Value - The list to convert.
	 * @param ReturnValue - A gameplay tag container, tags need to be generated and added to the project.  
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void ToGameplayTags(const FInkpotList &Value, FGameplayTagContainer &ReturnValue);

	/**
	 * ToStringArray ( Inkpot List )
	 * Converts an inkpot list to an array of strings.
	 *
	 * @param Value - List to convert.
	 * @param ReturnValue - The list as a string array of item names.
	 * @param bUseOrigin - Should the item names have their origin as a dot delimeted prefix.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|List")
	static INKPOT_API void ToStringArray(const FInkpotList &Value, TArray<FString> &ReturnValue, bool bUseOrigin = true );

	/**
	 * AddItem
	 * Adds an item to the passed in List.
	 *
	 * @param A - InkpotList.
	 * @param B - String
	 * @returns reference to A.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AddItem", CompactNodeTitle = "+"), Category = "Inkpot|List")
	static INKPOT_API FInkpotList & AddItem(UPARAM(ref) FInkpotList &A, const FString &Item );

	/**
	 * AddItemGT 
	 * ( GameplayTag version of AddItem )
	 * Adds an item to the passed in List.
	 *
	 * @param A - InkpotList.
	 * @param B - Gameplay Tag 
	 * @returns reference to A.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AddItemGT", CompactNodeTitle = "+"), Category = "Inkpot|List")
	static INKPOT_API FInkpotList& AddItemGT(UPARAM(ref) FInkpotList& A, UPARAM(meta = (Categories = "Ink.Origin")) FGameplayTag Item);

	/**
	 * Union
	 * Returns a new list that is the combination of both lists passed in. 
	 * Equivalent to calling (A + B) in ink..
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Union", CommutativeAssociativeBinaryOperator = "true", CompactNodeTitle = "+"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Union(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Intersect
	 * Returns a new list that is the intersection of both lists passed in 
	 * - i.e. a list of the items that are shared between the two lists. 
	 * Equivalent to calling (A ^ B) in ink.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Intersect", CompactNodeTitle = "^"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Intersect(const FInkpotList &A, const FInkpotList &B);

	/**
	 * HasIntersection
	 * Returns true if there is an intersection of both lists passed in 
	 * - i.e. both lists share some items
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if there is an intersection, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Has Intersection", CompactNodeTitle = "(^)"), Category="Inkpot|List")
	static INKPOT_API bool HasIntersection(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Without
	 * Returns a new list that is the first list with items in the second list removed. 
	 * Equivalent to calling (A - B) in ink.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns A new Inkpotlist.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Without", CompactNodeTitle = "-"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Without( const FInkpotList &A, const FInkpotList &B );

	/**
	 * ContainsList
	 * Returns true if the the first list contains all the items that are in the second list. 
	 * Equivalent to calling (A ? B) in ink.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns true if A contains B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Contains List", CompactNodeTitle = "?"), Category = "Inkpot|List" )
	static INKPOT_API bool ContainsList( const FInkpotList &Source, const FInkpotList &Querant );

	/**
	 * ContainsItem
	 * Returns true if the list contains an item matching the given name.
	 *
	 * @param Source - InkpotList to test.
	 * @param ItemName - String, item name.
	 * @returns true if Source contains ItemName, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Contains Item", CompactNodeTitle = "?"), Category="Inkpot|List")
	static INKPOT_API bool ContainsItem(const FInkpotList &Source, const FString &ItemName);

	/**
	 * ContainsTag
	 * Returns true if the list contains an item matching the given Gameplay Tag.
	 *
	 * @param Source - InkpotList to test.
	 * @param Tag - GammeplayTag, item name.
	 * @returns true if Source contains Tag, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Contains Item GT", CompactNodeTitle = "?"), Category="Inkpot|List")
	static INKPOT_API bool ContainsItemGT(const FInkpotList &Source, UPARAM(meta = (Categories = "Ink.Origin")) FGameplayTag Tag);

	/**
	 * GreaterThan
	 * Returns true if all the item values in the first list are greater than all the item values in the second list. 
	 * Equivalent to calling (A > B) in ink.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if A > B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater Than", CompactNodeTitle = ">"), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThan(const FInkpotList &A, const FInkpotList &B);

	/**
	 * GreaterThanOrEquals
	 * Returns true if the item values in the first list overlap or are all greater than the item values in the second list. 
	 * None of the item values in the current list must fall below the item values in the passed in list. 
	 * Equivalent to (A >= B) in ink, or LIST_MIN(A) >= LIST_MIN(B) && LIST_MAX(A) >= LIST_MAX(B).
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if A >= B, false otherwise.
	 */
	UFUNCTION( BlueprintPure, meta = (DisplayName = "Greater than or equals", CompactNodeTitle = ">="), Category = "Inkpot|List" )
	static INKPOT_API bool GreaterThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/**
	 * LessThan
	 * Returns true if all the item values first list are less than all the item values in the second list. 
	 * Equivalent to calling (A < B) in ink.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if A < B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than", CompactNodeTitle = "<"), Category="Inkpot|List")
	static INKPOT_API bool LessThan(const FInkpotList &A, const FInkpotList &B);

	/**
	 * LessThanOrEquals
	 * Returns true if the item values in the first list overlap or are all less than the item values in the second list. 
	 * None of the item values in the current list must go above the item values in the passed in list. 
	 * Equivalent to (A <= B) in ink, or LIST_MAX(A) <= LIST_MAX(B) && LIST_MIN(A) <= LIST_MIN(B).
	 * 
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if A <= B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Less than or equals", CompactNodeTitle = "<="), Category="Inkpot|List")
	static INKPOT_API bool LessThanOrEquals(const FInkpotList &A, const FInkpotList &B);

	/**
	 * Equals
	 * Returns true if the both lists contain the same items, false otherwise.
	 *
	 * @param A - InkpotList.
	 * @param B - InkpotList.
	 * @returns True if A == B, false otherwise.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Equals", CompactNodeTitle = "=="), Category="Inkpot|List")
	static INKPOT_API bool Equals(const FInkpotList &A, const FInkpotList &B);

	/**
	 * MinItem
	 * Returns a list containing the min item from the passed in list.
	 * Equivalent of calling ( LIST_MIN( A ) ) in ink 
	 *
	 * @param A - InkpotList.
	 * @returns A new inkpot list with the min item.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "MinItem", CompactNodeTitle = "min"), Category="Inkpot|List")
	static INKPOT_API FInkpotList MinItem(const FInkpotList &A);

	/**
	 * MaxItem
	 * Returns a list containing the max item from the passed in list.
	 * Equivalent of calling ( LIST_MAX( A ) ) in ink 
	 *
	 * @param A - InkpotList.
	 * @returns A new inkpot list with the max item.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "MaxItem", CompactNodeTitle = "max"), Category="Inkpot|List")
	static INKPOT_API FInkpotList MaxItem(const FInkpotList &A);

	/**
	 * Inverse
	 * Returns a list containing the inverse of the list passed in with respect to the origin.
	 * Equivalent of calling LIST_INVERT( A ) in ink 
	 * ( List will need a validated origin )
	 *
	 * @param A - InkpotList.
	 * @returns A new inkpot list that is the inverse of the passed in list.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Inverse", CompactNodeTitle = "~"), Category="Inkpot|List")
	static INKPOT_API FInkpotList Inverse(const FInkpotList &A);

	/**
	 * All
	 * Returns a list containing the all of the items as defined by the list origin.
	 * Equivalent of calling LIST_ALL( A ) in ink 
	 * ( List will need a validated origin )
	 *
	 * @param A - InkpotList.
	 * @returns A new inkpot list that cotains all the items defined in the origin.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "All", CompactNodeTitle = "all"), Category="Inkpot|List")
	static INKPOT_API FInkpotList All(const FInkpotList &A);

	/**
	 * Validate
	 * Validates the lists origin againt a story.
	 * This makes sure that the list is valid in the context of the passed in story.
	 * ie checks that the origin name of the list matches a list declaration in the ink story. 
	 * 
	 * Only required for lists that were created with no story context for certain operations (inverse, all).
	 * List validation will happen automatically when a list value is passed to the Ink runtime story. 
	 * Lists passed to Blueprint from Ink will not need validation. 
	 *
	 * @param A - InkpotList.
	 * @returns A reference to the list passed in.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Validate"), Category="Inkpot|List")
	static INKPOT_API const FInkpotList & Validate( UInkpotStory *Story, const FInkpotList &A );
};
