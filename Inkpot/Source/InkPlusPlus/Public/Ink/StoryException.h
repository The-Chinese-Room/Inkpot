#pragma once

#include "CoreMinimal.h"

namespace Ink
{
	// we can't write exceptions in this code, but the c# makes quite extensive use of it
	// this struct is an attempt to get closer to the source code we are copying
	class FStoryExceptionSubstitute
	{
	public:
		bool hasException = false;
		bool useEndLineNumber = false;
		FString message = "";

		void Throw(const FString& InMessage, bool InUseEndLineNumber = false);
		void Reset();
	};
}
