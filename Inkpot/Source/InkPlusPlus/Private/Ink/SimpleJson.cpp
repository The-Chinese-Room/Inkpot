/*
#include "Ink/SimpleJson.h"
#include "Misc/DefaultValueHelper.h"


#pragma region Reader
Ink::FReader::FReader(const FString& InText)
	: Text(InText),
	Offset(0)
{
	SkipWhitespace();
	//RootObject = ReadObject();
}

Ink::FReader::~FReader()
{
}

Ink::TJsonObjectMap Ink::FReader::ToMap() const
{
	return RootObject->GetMap();
}

Ink::TJsonObjectArray Ink::FReader::ToArray() const 
{
	return RootObject->GetArray();
}

TSharedPtr<Ink::FJsonObject> Ink::FReader::ReadObject()
{
	const TCHAR currentCharacter = Text[Offset];

	if (currentCharacter == '{')
		return ReadMap();

	else if (currentCharacter == '[')
		return ReadArray();

	else if (currentCharacter == '"')
		return ReadString();

	else if (IsFirstNumberChar(currentCharacter))
		return ReadNumber();

	else if (TryRead(FString(TEXT("true"))))
		return MakeShared<Ink::FJsonObject>(true);

	else if (TryRead(FString(TEXT("false"))))
		return MakeShared<Ink::FJsonObject>(false);

	else if (TryRead(FString(TEXT("null"))))
		return nullptr;

	checkf(false, TEXT("Unhandled object type in JSON: %s"), *Text.Mid(Offset, 30));
	return MakeShared<Ink::FJsonObject>();
}

TSharedPtr<Ink::FJsonObject> Ink::FReader::ReadMap()
{
	TJsonObjectMap map;

	Expect(FString(TEXT("{")));
	SkipWhitespace();

	// Empty Map?
	if (TryRead("}"))
		return MakeShared<Ink::FJsonObject>(map);

	do
	{
		SkipWhitespace();

		// Key
		FString key = ReadString()->GetString();
		
		Expect(!key.IsEmpty(), "dictionary key");
		SkipWhitespace();

		// :
		Expect(FString(TEXT(":")));
		SkipWhitespace();

		// Value
		TSharedPtr<Ink::FJsonObject> object = ReadObject();
		Expect(object->IsUnionValid(), "dictionary value");

		// Add to Map
		map.Add(key, object);

		SkipWhitespace();
	} while (TryRead(","));

	Expect(FString(TEXT("}")));
	return MakeShared<Ink::FJsonObject>(map);
}

TSharedPtr<Ink::FJsonObject> Ink::FReader::ReadArray()
{
	TJsonObjectArray array;
	Expect(FString(TEXT("[")));
	SkipWhitespace();

	// Empty List?
	if (TryRead("["))
		return MakeShared<Ink::FJsonObject>(array);

	do
	{
		SkipWhitespace();

		// Value
		TSharedPtr<FJsonObject> value = ReadObject();

		// Add to array
		array.Add(value);

		SkipWhitespace();
	} while (TryRead(","));

	Expect(FString(TEXT("]")));
	return MakeShared<Ink::FJsonObject>(array);
}

TSharedPtr<Ink::FJsonObject> Ink::FReader::ReadString()
{
	Expect(FString(TEXT("\"")));

	FString string = FString();

	// TODO: That might be wrong, advancing class member var??
	for (; Offset < Text.Len(); ++Offset)
	{
		TCHAR character = Text[Offset];

		if (character == '\\')
		{
			// Escaped character
			++Offset;
			if (Offset >= Text.Len())
				checkf(false, TEXT("Unexpected EOF while reading string"))
			
			character = Text[Offset];
			int32 uchar = -1;
			FString digits = FString();
			
			switch (character)
			{
				case '"':
				case '\\':
				case '/': // Yes, JSON allows this to be escaped
					string += character;
					break;
				case 'n':
					string += '\n';
					break;
				case 't':
					string += '\t';
					break;
				case 'r':
				case 'b':
				case 'f':
					// Ignore other control character
					break;
				case 'u':
					// 4-digit Unicode
					if (Offset + 4 >= Text.Len()) // TODO: Get rid off this magic number ;P
						checkf(false, TEXT("Unexpected EOF while reading string"));
					digits = Text.Mid(Offset + 1, 4);
					uchar = -1;
					if (FDefaultValueHelper::ParseInt(digits, uchar))
					{
						string += static_cast<TCHAR>(uchar); // TODO: Will this convert the int32 to TCHAR letter?
						//string += TCHAR(uchar); 
						Offset += 4;
					}
					else
					{
						//const FString message = FString::Printf(TEXT("Invalid Unicode escape character at offset %d"), (Offset - 1));
						checkf(false, TEXT("Invalid Unicode escape character at offset %d"), Offset - 1);
					}
					break;
				default:
					// The escaped character is invalid per json spec
					checkf(false, TEXT("Invalid Unicode escape character at offset %d"), Offset - 1);
			}
		}
		else if (character == '"')
		{
			break;
		}
		else
		{
			string += character;
		}
	}

	Expect(FString(TEXT("\"")));

	TSharedPtr<Ink::FJsonObject> jsonObject = MakeShared<Ink::FJsonObject>(string);
	return jsonObject;
}

TSharedPtr<Ink::FJsonObject> Ink::FReader::ReadNumber()
{
	const int32 startOffset = Offset;
	bool isFloat = false;
	
	for (; Offset < Text.Len(); ++Offset)
	{
		TCHAR character = Text[Offset];
		if (character == '.' || character == 'e' || character == 'E')
			isFloat = true;
		
		if (IsNumberChar(character))
			continue;
		else
			break;
	}

	const FString numberString = Text.Mid(startOffset, Offset - startOffset);
	if (isFloat)
	{
		float numberFloat;
		if (FDefaultValueHelper::ParseFloat(numberString, numberFloat))
			return MakeShared<Ink::FJsonObject>(numberFloat);
	}
	else
	{
		int32 numberInt;
		if (FDefaultValueHelper::ParseInt(numberString, numberInt))
			return MakeShared<Ink::FJsonObject>(numberInt);
	}

	checkf(false, TEXT("Failed to parse number value: %s"), *numberString);
	return nullptr;
}

bool Ink::FReader::IsNumberChar(const TCHAR& InCharacter)
{
	return InCharacter >= '0' && InCharacter <= '9' || InCharacter == '.' || InCharacter == '-' || InCharacter == '+' || InCharacter == 'E' || InCharacter == 'e';
}

bool Ink::FReader::IsFirstNumberChar(const TCHAR& InCharacter)
{
	return InCharacter >= '0' && InCharacter <= '9' || InCharacter == '-' || InCharacter == '+';
}

bool Ink::FReader::TryRead(const FString& InTextToRead)
{
	if (Offset + InTextToRead.Len() > Text.Len())
		return false;

	for (int32 i = 0; i < InTextToRead.Len(); ++i)
	{
		if (InTextToRead[i] != Text[Offset + i])
			return false;
	}

	Offset += InTextToRead.Len();
	return true;
}

void Ink::FReader::Expect(const FString& InExpectedString)
{
	if (!TryRead(InExpectedString))
		Expect(false, InExpectedString);
}

void Ink::FReader::Expect(bool InCondition, const FString& InMessage)
{
	if (!InCondition)
	{
		FString message;
		
		if (InMessage.IsEmpty())
			message = TEXT("Unexpected token");
		else
			message = TEXT("Expected ") + InMessage;
		
		message += FString::Printf(TEXT(" at offset %d"), Offset);
		checkf(false, TEXT("%s"), *message);
	}
}

void Ink::FReader::SkipWhitespace()
{
	while (Offset < Text.Len())
	{
		const TCHAR character = Text[Offset];
		if (character == ' ' || character == '\t' || character == '\n' || character == '\r')
			++Offset;
		else
			break;
	}
}
#pragma endregion Reader
*/

