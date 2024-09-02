#pragma once
#include "CoreMinimal.h"
#include "ListDefinition.h"

namespace Ink
{
	class FValueList;
	
	class INKPLUSPLUS_API FListDefinitionsOrigin
	{	
	public:
		FListDefinitionsOrigin() = delete;
		FListDefinitionsOrigin(TArray<TSharedPtr<Ink::FListDefinition>> InLists);
		FListDefinitionsOrigin(const Ink::FListDefinitionsOrigin& InOtherListDefinitionsOrigin) = default;
		FListDefinitionsOrigin& operator=(const Ink::FListDefinitionsOrigin& InOtherListDefinitionsOrigin) = default;
		~FListDefinitionsOrigin() = default;

		bool TryListGetDefinition(const FString& InName, TSharedPtr<FListDefinition>& OutListDefinition);
		TSharedPtr<Ink::FValueList> FindSingleItemListWithName(const FString& InName);
		
		TArray<TSharedPtr<FListDefinition>> GetLists() const;

		void AddListDefinition(TSharedPtr<Ink::FListDefinition> InOrigin);
	
	private:
		TMap<FString, TSharedPtr<Ink::FListDefinition>> Lists;
		TMap<FString, TSharedPtr<Ink::FValueList>> AllUnambiguousListValueCache;
	};
}