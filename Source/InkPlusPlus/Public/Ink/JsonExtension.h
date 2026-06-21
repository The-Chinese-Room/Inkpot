#pragma once
#include "Json.h"
#include "Containers/StringFwd.h"


namespace Ink
{
	class FObject;

	using FStringType = UE::FSharedString;

	struct FJsonExtension
	{
		static TMap<FString, TSharedPtr<Ink::FObject>> JSONObjectToInkObject(const TMap<FStringType, TSharedPtr<FJsonValue>>& InJSONObject);
		static TMap<FString, int32> JSONObjectToIntDictionary(const TMap<FStringType, TSharedPtr<FJsonValue>>& InJSONObject);
		static TArray<TSharedPtr<Ink::FObject>> JSONValueToInkObject(const TArray<TSharedPtr<FJsonValue>>& InJSONArray);
		static TSharedPtr<Ink::FObject> JSONTokenToInkObject(const TSharedPtr<FJsonValue> InJSONToken);

		static bool TryGetField(const FString& InFieldName, const FJsonObject& InJSONObject, TSharedPtr<FJsonValue>& OutValue);
	};
}
