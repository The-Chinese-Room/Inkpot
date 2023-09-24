/*
#include "Ink/SimpleJsonObject.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FJsonObject::FJsonObject()
{
}

Ink::FJsonObject::FJsonObject(int32 InInt)
{
	SetInt(InInt);
}

Ink::FJsonObject::FJsonObject(float InFloat)
{
	SetFloat(InFloat);
}

Ink::FJsonObject::FJsonObject(bool InBool)
{
	SetBool(InBool);
}

Ink::FJsonObject::FJsonObject(const FString& InString)
{
	SetString(InString);
}

Ink::FJsonObject::FJsonObject(const TJsonObjectMap& InMap)
{
	SetMap(InMap);
}

Ink::FJsonObject::FJsonObject(const TJsonObjectArray& InArray)
{
	SetArray(InArray);
}

Ink::FJsonObject::FJsonObject(const FJsonObject& InOtherJsonObject)
{
	Union = InOtherJsonObject.Union;
}

Ink::FJsonObject& Ink::FJsonObject::operator=(const FJsonObject& InOtherJsonObject)
{
	Union = InOtherJsonObject.Union;
	return *this;
}

Ink::FJsonObject::~FJsonObject()
{
}

int32 Ink::FJsonObject::GetInt() const
{
	if (!IsUnionInitialised())
		return -1;
	
	if (!Union.HasSubtype<int32>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get Int32 from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return -1;
	}

	return Union.GetSubtype<int32>();
}

void Ink::FJsonObject::SetInt(int32 InInt)
{
	Union.SetSubtype<int32>(InInt);
}

float Ink::FJsonObject::GetFloat() const
{
	if (!IsUnionInitialised())
		return -1.f;
	
	if (!Union.HasSubtype<float>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get Float from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return -1.f;
	}

	return Union.GetSubtype<int32>();
}

void Ink::FJsonObject::SetFloat(float InFloat)
{
	Union.SetSubtype<float>(InFloat);
}

bool Ink::FJsonObject::GetBool() const
{
	if (!IsUnionInitialised())
		return false;
	
	if (!Union.HasSubtype<bool>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get Bool from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return false;
	}

	return Union.GetSubtype<bool>();
}

void Ink::FJsonObject::SetBool(bool InBool)
{
	Union.SetSubtype<bool>(InBool);
}

FString Ink::FJsonObject::GetString() const
{
	if (!IsUnionInitialised())
		return FString();
	
	if (!Union.HasSubtype<FString>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get FString from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return FString();
	}

	return Union.GetSubtype<FString>();
}

void Ink::FJsonObject::SetString(const FString& InString)
{
	Union.SetSubtype<FString>(InString);
}

Ink::TJsonObjectMap Ink::FJsonObject::GetMap() const
{
	if (!IsUnionInitialised())
		return TJsonObjectMap();;

	// TODO: Perhaps change to assert
	if (!Union.HasSubtype<TJsonObjectMap>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get Map from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return TJsonObjectMap();
	}

	return Union.GetSubtype<TJsonObjectMap>();
}

void Ink::FJsonObject::SetMap(const TJsonObjectMap& InMap)
{
	Union.SetSubtype<TJsonObjectMap>(InMap);
}

Ink::TJsonObjectArray Ink::FJsonObject::GetArray() const
{
	if (!IsUnionInitialised())
		return TJsonObjectArray();;

	// TODO: Perhaps change to assert
	if (!Union.HasSubtype<TJsonObjectArray>())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : trying to get Array from union, but the union is of a different type {%s} !"), *GetCurrentUnionType());
		return TJsonObjectArray();
	}

	return Union.GetSubtype<TJsonObjectArray>();
}

void Ink::FJsonObject::SetArray(const TJsonObjectArray& InArray)
{
	Union.SetSubtype<TJsonObjectArray>(InArray);
}

Ink::TJsonObjectUnion Ink::FJsonObject::GetUnion() const
{
	return Union;
}

FString Ink::FJsonObject::GetCurrentUnionType() const
{
	FString unionType;
	switch (Union.GetCurrentSubtypeIndex())
	{
		case -1:
			unionType = TEXT("Uninitialised");
			break;
		case 1:
			unionType = TEXT("Int");
			break;
		case 2:
			unionType = TEXT("Float");
			break;
		case 3:
			unionType = TEXT("Bool");
			break;
		case 4:
			unionType = TEXT("FString");
			break;
		case 5:
			unionType = TEXT("TJsonObjectMap");
			break;
		case 6:
			unionType = TEXT("TJsonObjectArray");
			break;
		default:
			unionType = TEXT("Uninitialised");
			break;
	}

	return unionType;
}

bool Ink::FJsonObject::IsUnionValid() const
{
	return IsUnionInitialised();
}

bool Ink::FJsonObject::IsUnionInitialised() const
{
	if (Union.GetCurrentSubtypeIndex() == -1)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("SimpleJsonObject : union is not initialised!"))
		return false;
	}

	return true;
}
*/
