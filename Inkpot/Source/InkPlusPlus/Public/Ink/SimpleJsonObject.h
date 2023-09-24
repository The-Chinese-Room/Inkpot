/*
#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"


namespace Ink
{
	class FJsonObject;
	
	using TJsonObjectMap = TMap<FString, TSharedPtr<FJsonObject>>;
	using TJsonObjectArray = TArray<TSharedPtr<FJsonObject>>;
	using TJsonObjectUnion = TUnion<int32, float, bool, FString, TJsonObjectMap, TJsonObjectArray>; 	// Don't change the order of the union
	
	/* Object (Union) that can be either:
	*	-> Map of objects
	*	-> Array of objects
	*	-> String
	*	-> Int
	*	-> Float
    *	-> bool
	*	-> Null						#1#
	class FJsonObject final
	{
	public:
		FJsonObject();
		FJsonObject(int32 InInt);
		FJsonObject(float InFloat);
		FJsonObject(bool InBool);
		FJsonObject(const FString& InString);
		FJsonObject(const TJsonObjectMap& InMap);
		FJsonObject(const TJsonObjectArray& InArray);
		FJsonObject(const FJsonObject& InOtherJsonObject);
		FJsonObject& operator=(const FJsonObject& InOtherJsonObject);
		~FJsonObject();

		int32 GetInt() const;
		void SetInt(int32 InInt);

		float GetFloat() const;
		void SetFloat(float InFloat);

		bool GetBool() const;
		void SetBool(bool InBool);
		
		FString GetString() const;
		void SetString(const FString& InString);

		TJsonObjectMap GetMap() const;
		void SetMap(const TJsonObjectMap& InMap);

		TJsonObjectArray GetArray() const;
		void SetArray(const TJsonObjectArray& InArray);

		TJsonObjectUnion GetUnion() const;
		FString GetCurrentUnionType() const; // This depends on union structure!

		bool IsUnionValid() const;
	private:
		bool IsUnionInitialised() const;
	
	private:
		TJsonObjectUnion Union;
	};

}
*/
