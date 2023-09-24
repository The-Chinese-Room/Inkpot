#include "Ink/Tag.h"


Ink::FTag::FTag(const FString& InText)
	: Text(InText)
{
}

FString Ink::FTag::ToString()
{
	return FString(TEXT("# " + Text));
}

FString Ink::FTag::GetText() const
{
	return Text;
}

void Ink::FTag::SetText(const FString& InText)
{
	Text = InText;
}
