#pragma once
#include "Json.h"

namespace Ink
{
	class FObject;
	
	struct FJsonExtension
	{
		static TMap<FString, TSharedPtr<Ink::FObject>> JSONObjectToInkObject(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONObject);
		static TMap<FString, int32> JSONObjectToIntDictionary(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONObject);
		static TArray<TSharedPtr<Ink::FObject>> JSONValueToInkObject(const TArray<TSharedPtr<FJsonValue>>& InJSONArray);
		static TSharedPtr<Ink::FObject> JSONTokenToInkObject(const TSharedPtr<FJsonValue> InJSONToken);

		static bool TryGetField(const FString& InFieldName, const FJsonObject& InJSONObject, TSharedPtr<FJsonValue>& OutValue);
	};
}