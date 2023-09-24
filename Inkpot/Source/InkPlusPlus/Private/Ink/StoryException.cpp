#include "Ink/StoryException.h"
#include "Utility/InkPlusPlusLog.h"

namespace Ink
{
	// we can't write exceptions in this code, but the c# makes quite extensive use of it
	// this struct is an attempt to get closer to the source code we are copying

	void FStoryExceptionSubstitute::Throw(const FString& InMessage, bool InUseEndLineNumber)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("%s"), *InMessage);
		// hasException = true;
		message = InMessage;
		useEndLineNumber = InUseEndLineNumber;
	}

	void FStoryExceptionSubstitute::Reset()
	{
		hasException = false;
		useEndLineNumber = false;
		message = "";
	}
}
