#pragma once
#include "CoreMinimal.h"

INKPOT_API DECLARE_LOG_CATEGORY_EXTERN(Inkpot, Log, All);

#define INKPOT_LOG( msg, ... )   UE_LOG( Inkpot, Display, TEXT( msg ), ##__VA_ARGS__ )
#define INKPOT_WARN( msg, ... )  UE_LOG( Inkpot, Warning, TEXT( msg ), ##__VA_ARGS__ )
#define INKPOT_ERROR( msg, ... ) UE_LOG( Inkpot, Error,   TEXT( msg ), ##__VA_ARGS__ )
