﻿#pragma once
#include "CoreMinimal.h"

namespace Ink
{
	/*	Author errors will only ever come from the compiler so don't need to be handled
		by your Story error handler. The "Error" ErrorType is by far the most common
		for a runtime story error (rather than compiler error), though the Warning type
		is also possible.																	*/
	enum class EErrorType
	{
		// Generated by a "T O D O" note in the ink source
		Author = 0,
		// You should probably fix this, but it's not critical
		Warning,
		// Critical error that can't be recovered from
		Error
	};

	// Callback for errors throughout both the ink runtime and compiler.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FErrorHandler, FString, EErrorType);
}