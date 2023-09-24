/*
#pragma once

#include "CoreMinimal.h"
#include "SimpleJsonObject.h"

namespace Ink
{
	
#pragma region SimpleJson
	class FSimpleJson
	{
	public:

		//TMap<FString, > TextToDictionary(const FString& InText);
		//TArray<> TextToArray(const FString& InText);
	};
#pragma endregion SimpleJson
	
	
#pragma region Reader
	class FReader
	{
	public:
		FReader() = delete;
		FReader(const FString& InText);
		FReader(const Ink::FReader& InOtherReader) = default;
		Ink::FReader& operator=(const Ink::FReader& InOtherReader) = default;
		~FReader();

		TJsonObjectMap ToMap() const;
		TJsonObjectArray ToArray() const;
	
	private:
		TSharedPtr<Ink::FJsonObject> ReadObject();
		TSharedPtr<Ink::FJsonObject> ReadMap();
		TSharedPtr<Ink::FJsonObject> ReadArray();
		TSharedPtr<Ink::FJsonObject> ReadString();
		TSharedPtr<Ink::FJsonObject> ReadNumber();
		
		bool TryRead(const FString& InTextToRead);
		void Expect(const FString& InExpectedString);
		void Expect(bool InCondition, const FString& InMessage = FString());
		void SkipWhitespace();

		static bool IsNumberChar(const TCHAR& InCharacter);
		static bool IsFirstNumberChar(const TCHAR& InCharacter);
	
	private:
		FString Text;
		int32 Offset;
		TSharedPtr<Ink::FJsonObject> RootObject;
	};
#pragma endregion Reader


#pragma region Writer
	class FWriter
	{
	public:
		
		/*FWriter();
		FWriter(/* Stream??#2#);

		void WriteObject(/*Action of writer#2#);
		void WriteObjectStart();
		void WriteObjectEnd();
		
		void WriteProperty(const FString& InName /*, ACtion of writer#2#);
		void WriteProperty(int32 InID /*, ACtion of writer#2#);
		void WriteProperty(const FString& InName, const FString& InContent);
		void WriteProperty(const FString& InName, int32 InContent);
		void WriteProperty(const FString& InName, bool InContent);
		
		void WritePropertyStart(const FString& InName);
		void WritePropertyStart(int32 InID);
		void WritePropertyEnd();
		
		void WritePropertyNameStart();
		void WritePropertyNameEnd();
		void WritePropertyNameInner(const FString& InString);

		template <typename T>
		void WritePropertyStart(T InName);

		template <typename T>
		void WriteProperty(T InName /*, ACtion of writer#2#);

		void WriteArrayStart();
		void WriteArrayEnd();
		
		void Write(int32 InInt);
		void Write(float InFloat);
		void Write(bool InBool);
		void Write(const FString& InString, bool InEscape = true);

		void WriteStringStart();
		void WriteStringEnd();
		void WriteStringInner();

		void WriteEscapedString(const FString& InString);

		void StartNewObject(bool InContainer);#1#
		
	};
#pragma endregion Writer
}
*/
