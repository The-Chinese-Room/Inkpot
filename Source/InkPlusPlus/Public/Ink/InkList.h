#pragma once
#include "CoreMinimal.h"
#include "InkListItem.h"
#include "Containers/Union.h"

namespace Ink
{
	class FStory;
	class FListDefinition;
	class FPath;
	class FInkList;

	using FValueType = TUnion<int32, float, bool, FString, TSharedPtr<FPath>, FInkList>;

	/*	The InkList is the underlying type that's used to store an instance of a
		list in ink. It's not used for the *definition* of the list, but for a list
		value that's stored in a variable. Somewhat confusingly, it's backed by a Unreal Dictionary	*/
	class INKPLUSPLUS_API FInkList final : public TMap<Ink::FInkListItem, int32>
	{
	public:
		FInkList();

		// Create a new ink list that contains the same contents as another list.
		FInkList(const Ink::FInkList& InOtherInkList);

		// Create a new empty ink list that's intended to hold items from a particular origin
		// list definition. The origin Story is needed in order to be able to look up that definition.
		FInkList(const FString& InSingleOriginListName, Ink::FStory* InOriginStory);
		
		FInkList(const Ink::FInkListItem& InKey, int32 InValue);
		FInkList(const TPair<Ink::FInkListItem, int32>& InSingleElement);
		

		// Adds the given item to the ink list. Note that the item must come from a list definition that
		// is already "known" to this list, so that the item's value can be looked up. By "known", we mean
		// that it already has items in it from that source, or it did at one point - it can't be a 
		// completely fresh empty list, or a list that only contains items from a different list definition.
		void AddItem(const Ink::FInkListItem& InItem);

		// Adds the given item to the ink list, attempting to find the origin list definition that it belongs to.
		// The item must therefore come from a list definition that is already "known" to this list, so that the
		// item's value can be looked up. By "known", we mean that it already has items in it from that source, or
		// it did at one point - it can't be a completely fresh empty list, or a list that only contains items from
		// a different list definition.
		void AddItem(const FString& InItemName);
		bool TryAddItem(const FString& InItemName);

		// Returns true if this ink list contains an item with the given short name
		// (ignoring the original list where it was defined).
		bool ContainsItemNamed(const FString& InString) const;
		TSharedPtr<Ink::FListDefinition> OriginOfMaxItem() const;

		void SetInitialOriginName(const FString& InInitialOriginName);
		void SetInitialOriginNames(TSharedPtr<TArray<FString>> InInitialOriginNames);

		TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>>>& GetOrigins();
		
		// Get the maximum item in the list, equivalent to calling LIST_MAX(list) in ink.
		TPair<Ink::FInkListItem, int32> MaxItem() const;

		// Get the minimum item in the list, equivalent to calling LIST_MIN(list) in ink.
		TPair<Ink::FInkListItem, int32> MinItem() const;

		// The inverse of the list, equivalent to calling LIST_INVERSE(list) in ink
		FInkList Inverse() const;

		// The list of all items from the original list definition, equivalent to calling
		// LIST_ALL(list) in ink.
		FInkList All() const;

		// Returns a new list that is the combination of the current list and one that's
		// passed in. Equivalent to calling (list1 + list2) in ink.
		FInkList Union(const Ink::FInkList& InOtherList) const;

		// Returns a new list that is the intersection of the current list with another
		// list that's passed in - i.e. a list of the items that are shared between the
		// two other lists. Equivalent to calling (list1 ^ list2) in ink.
		FInkList Intersect(const Ink::FInkList& InOtherList) const;

		/// Fast test for the existence of any intersection between the current list and another
		bool HasIntersection(const Ink::FInkList& InOtherList) const;
		
		// Returns a new list that's the same as the current one, except with the given items
		// removed that are in the passed in list. Equivalent to calling (list1 - list2) in ink.
		FInkList Without(const Ink::FInkList& InListToRemove) const;

		// Returns true if the current list contains all the items that are in the list that
		// is passed in. Equivalent to calling (list1 ? list2) in ink.
		bool ContainsList(const Ink::FInkList& InOtherList) const;

		/// Returns true if the current list contains an item matching the given name.
		bool ContainsItem(const FString &InListItemName) const;

		// Returns true if all the item values in the current list are greater than all the
		// item values in the passed in list. Equivalent to calling (list1 > list2) in ink.
		bool GreaterThan(const Ink::FInkList& InOtherList) const;
		
		// Returns true if the item values in the current list overlap or are all greater than
		// the item values in the passed in list. None of the item values in the current list must
		// fall below the item values in the passed in list. Equivalent to (list1 >= list2) in ink,
		// or LIST_MIN(list1) >= LIST_MIN(list2) &amp;&amp; LIST_MAX(list1) >= LIST_MAX(list2).
		bool GreaterThanOrEquals(const Ink::FInkList& InOtherList) const;

		// Returns true if all the item values in the current list are less than all the
		// item values in the passed in list. Equivalent to calling (list1 &lt; list2) in ink.
		bool LessThan(const Ink::FInkList& InOtherList) const;

		// Returns true if the item values in the current list overlap or are all less than
		// the item values in the passed in list. None of the item values in the current list must
		// go above the item values in the passed in list. Equivalent to (list1 &lt;= list2) in ink,
		// or LIST_MAX(list1) &lt;= LIST_MAX(list2) &amp;&amp; LIST_MIN(list1) &lt;= LIST_MIN(list2).
		bool LessThanOrEquals(const Ink::FInkList& InOtherList) const;

		FInkList MaxAsList() const;
		FInkList MinAsList() const;

		// Returns a sublist with the elements given the minimum and maxmimum bounds.
		// The bounds can either be ints which are indices into the entire (sorted) list,
		// or they can be InkLists themselves. These are intended to be single-item lists so
		// you can specify the upper and lower bounds. If you pass in multi-item lists, it'll
		// use the minimum and maximum items in those lists respectively.
		// WARNING: Calling this method requires a full sort of all the elements in the list.
		FInkList ListWithSubRange(const Ink::FValueType& InMinBound, const Ink::FValueType& InMaxBound) const;

		// Returns true if the passed object is also an ink list that contains
		// the same items as the current list, false otherwise.
		bool Equals(const Ink::FInkList& InOtherList) const;

		// Return the hashcode for this object, used for comparisons and inserting into dictionaries.
		int32 GetHashCode() const;

		// Returns a string in the form "a, b, c" with the names of the items in the list, without
		// the origin list definition names. Equivalent to writing {list} in ink.
		FString ToString() const;

		TArray<TPair<Ink::FInkListItem, int32>> GetOrderedItems() const;

		TSharedPtr<TArray<FString>> GetOriginNames() const;
		
		// Converts a string to an ink list and returns for use in the story.
		static Ink::FInkList FromString(const FString& InMyListItem, const Ink::FStory& InOriginStory);

	private:
		static TArray<TPair<Ink::FInkListItem, int32>> MapToArray(const Ink::FInkList& InList);
	
	private:
		// Origin name needs to be serialised when content is empty,
		// assuming a name is availble, for list definitions with variable
		// that is currently empty.
		mutable TSharedPtr<TArray<FString>> _originNames;
		
		// Story has to set this so that the value knows its origin,
		// necessary for certain operations (e.g. interacting with ints).
		// Only the story has access to the full set of lists, so that
		// the origin can be resolved from the originListName.
		TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>>> Origins;
	};
}
