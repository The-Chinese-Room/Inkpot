#pragma once
#include "CoreMinimal.h"

namespace Ink
{
	class FDebugMetadata
	{
	public:
		FDebugMetadata();
		FDebugMetadata(const Ink::FDebugMetadata& InOtherDebugMetadata) = default;
		FDebugMetadata& operator=(const Ink::FDebugMetadata& InOtherDebugMetadata) = default;
		~FDebugMetadata() = default;

		FString ToString() const;
		TSharedPtr<Ink::FDebugMetadata> Merge(const Ink::FDebugMetadata& InOtherDebugMetadata) const;

	public:
		int32 StartLineNumber;
		int32 EndLineNumber;
		int32 StartCharacterNumber;
		int32 EndCharacterNumber;
		FString FileName;
		FString SourceName;
	};

}