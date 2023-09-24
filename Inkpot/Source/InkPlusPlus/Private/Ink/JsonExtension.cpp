#include "Ink/JsonExtension.h"
#include "Ink/Object.h"
#include "Ink/JsonSerialisation.h"


TMap<FString, TSharedPtr<Ink::FObject>> Ink::FJsonExtension::JSONObjectToInkObject(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONObject)
{
	TMap<FString, TSharedPtr<Ink::FObject>> convertedMap;
	convertedMap.Reserve(InJSONObject.Num());

	for (const TPair<FString, TSharedPtr<FJsonValue>>& jsonPair : InJSONObject)
	{
		const FString& key = jsonPair.Key;
		TSharedPtr<Ink::FObject> value(Ink::FJsonSerialisation::JsonTokenToRuntimeObject(*jsonPair.Value));
		convertedMap.Add(jsonPair.Key, value);
	}

	return convertedMap;
}

TMap<FString, int32> Ink::FJsonExtension::JSONObjectToIntDictionary(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONObject)
{
	TMap<FString, int32> convertedMap;
	convertedMap.Reserve(InJSONObject.Num());

	for (const TPair<FString, TSharedPtr<FJsonValue>>& jsonPair : InJSONObject)
	{
		const FString& key = jsonPair.Key;
		int32 value = static_cast<int32>(jsonPair.Value->AsNumber());
		convertedMap.Add(jsonPair.Key, value);
	}

	return convertedMap;
}

TArray<TSharedPtr<Ink::FObject>> Ink::FJsonExtension::JSONValueToInkObject(const TArray<TSharedPtr<FJsonValue>>& InJSONArray)
{
	TArray<TSharedPtr<Ink::FObject>> convertedArray;
	convertedArray.Reserve(InJSONArray.Num());

	for (const TSharedPtr<FJsonValue>& item : InJSONArray)
	{
		convertedArray.Add(Ink::FJsonSerialisation::JsonTokenToRuntimeObject(*item));
	}

	return convertedArray;
}

TSharedPtr<Ink::FObject> Ink::FJsonExtension::JSONTokenToInkObject(const TSharedPtr<FJsonValue> InJSONToken)
{
	const TSharedPtr<Ink::FObject> result(Ink::FJsonSerialisation::JsonTokenToRuntimeObject(*InJSONToken));
	return result;
}

bool Ink::FJsonExtension::TryGetField(const FString& InFieldName, const FJsonObject& InJSONObject, TSharedPtr<FJsonValue>& OutValue)
{
	const TSharedPtr<FJsonValue> value = InJSONObject.TryGetField(InFieldName);
	if (value.IsValid())
	{
		OutValue = value;
		return true;
	}
	return false;
}
