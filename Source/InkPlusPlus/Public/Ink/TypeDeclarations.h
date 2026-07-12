#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

#if	ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 8
#include "Containers/StringFwd.h"
#endif

namespace Ink
{
	class FObject;
	class FStory;
	class FThread;
	class FValueVariablePointer;
	class FContainer;
	class FChoice;
	class FValueList;
	class FListDefinitionsOrigin;
	class FCallStack;

#if	ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 8
	using FStringType = UE::FSharedString;
#else
	using FStringType = FString;
#endif
};

