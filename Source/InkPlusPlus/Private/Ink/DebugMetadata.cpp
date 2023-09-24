#include "Ink/DebugMetadata.h"

Ink::FDebugMetadata::FDebugMetadata()
	: StartLineNumber(0),
	EndLineNumber(0),
	StartCharacterNumber(0),
	EndCharacterNumber(0),
	FileName(FString()),
	SourceName(FString())
{
}

FString Ink::FDebugMetadata::ToString() const
{
	if (FileName.IsEmpty())
	{
		return FString::Printf(TEXT("line %d"), StartLineNumber);
	}
	
	return FString::Printf(TEXT("line %d of %s"), StartLineNumber, *FileName);
}

TSharedPtr<Ink::FDebugMetadata> Ink::FDebugMetadata::Merge(const Ink::FDebugMetadata& InOtherDebugMetadata) const
{
	TSharedPtr<Ink::FDebugMetadata> newDebugMetadata = MakeShared<Ink::FDebugMetadata>();

	// These are not supposed to be differ between 'this' and 'InOtherDebugMetadata'.
	newDebugMetadata->FileName = FileName;
	newDebugMetadata->SourceName = SourceName;

	if (StartLineNumber < InOtherDebugMetadata.StartLineNumber)
	{
		newDebugMetadata->StartLineNumber = StartLineNumber;
		newDebugMetadata->StartCharacterNumber = StartCharacterNumber;
	}
	else if (StartLineNumber > InOtherDebugMetadata.StartLineNumber)
	{
		newDebugMetadata->StartLineNumber = InOtherDebugMetadata.StartLineNumber;
		newDebugMetadata->StartCharacterNumber = InOtherDebugMetadata.StartCharacterNumber;
	}
	else
	{
		newDebugMetadata->StartLineNumber = StartLineNumber;
		newDebugMetadata->StartCharacterNumber = FMath::Min<int32>(StartCharacterNumber, InOtherDebugMetadata.StartCharacterNumber);
	}

	if (EndLineNumber > InOtherDebugMetadata.EndLineNumber)
	{
		newDebugMetadata->EndLineNumber = EndLineNumber;
		newDebugMetadata->EndCharacterNumber = EndCharacterNumber;
	}
	else if (EndLineNumber < InOtherDebugMetadata.EndLineNumber)
	{
		newDebugMetadata->EndLineNumber = InOtherDebugMetadata.EndLineNumber;
		newDebugMetadata->EndCharacterNumber = InOtherDebugMetadata.EndCharacterNumber;
	}
	else
	{
		newDebugMetadata->EndLineNumber = EndLineNumber;
		newDebugMetadata->EndCharacterNumber = FMath::Max<int32>(EndCharacterNumber, InOtherDebugMetadata.EndCharacterNumber);
	}

	return newDebugMetadata;
}
