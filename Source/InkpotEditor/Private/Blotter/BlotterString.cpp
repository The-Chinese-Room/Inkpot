#include "Blotter/BlotterString.h"


void UBlotterString::Set(const FString& InText)
{
	Text = FText::FromString( InText ); 
}

const FText& UBlotterString::GetText() const
{
	return Text;
}
