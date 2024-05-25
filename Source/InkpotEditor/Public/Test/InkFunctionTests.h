#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotValue.h"
#include "InkFunctionTests.generated.h"


UCLASS()
class UInkFunctionTests : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	FInkpotValue Message(const TArray<FInkpotValue> & InValues);
	static const FString FuncName_Message;

	UFUNCTION()
	FInkpotValue Multiply(const TArray<FInkpotValue> & InValues);
	static const FString FuncName_Multiply;

	UFUNCTION()
	FInkpotValue Times(const TArray<FInkpotValue> & InValues);
	static const FString FuncName_Times;

	UFUNCTION()
	FInkpotValue TRUE(const TArray<FInkpotValue> & InValues);
	static const FString FuncName_TRUE;
};

