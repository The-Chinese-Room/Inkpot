#pragma once
#include "CoreMinimal.h"
#include "ListDefinition.h"

namespace Ink
{
	class FValueList;
	
	class FListDefinitionsOrigin
	{	
	public:
		FListDefinitionsOrigin() = delete;
		FListDefinitionsOrigin(TArray<TSharedPtr<Ink::FListDefinition>> InLists);
		FListDefinitionsOrigin(const Ink::FListDefinitionsOrigin& InOtherListDefinitionsOrigin) = default;
		FListDefinitionsOrigin& operator=(const Ink::FListDefinitionsOrigin& InOtherListDefinitionsOrigin) = default;
		~FListDefinitionsOrigin() = default;

		bool TryListGetDefinition(const FString& InName, TSharedPtr<FListDefinition>& OutListDefinition);
		TSharedPtr<Ink::FValueList> FindSingleItemListWithName(const FString& InName);
		
		INKPLUSPLUS_API TArray<TSharedPtr<FListDefinition>> GetLists() const;
	
	private:
		TMap<FString, TSharedPtr<Ink::FListDefinition>> Lists;
		TMap<FString, TSharedPtr<Ink::FValueList>> AllUnambiguousListValueCache;
	};
}