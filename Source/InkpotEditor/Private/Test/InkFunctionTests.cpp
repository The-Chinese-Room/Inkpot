#include "Test/InkFunctionTests.h"

const FString UInkFunctionTests::FuncName_Message = TEXT("message");
const FString UInkFunctionTests::FuncName_Multiply = TEXT("multiply");
const FString UInkFunctionTests::FuncName_Times = TEXT("times");
const FString UInkFunctionTests::FuncName_TRUE = TEXT("TRUE");

FInkpotValue UInkFunctionTests::Message(const TArray<FInkpotValue> & InValues)
{
	FString output = UInkpotLibrary::InkpotArrayValueAsString( InValues, 0 );
	return UInkpotLibrary::MakeStringInkpotValue( output );
}

FInkpotValue UInkFunctionTests::Multiply(const TArray<FInkpotValue> & InValues)
{
	float A = UInkpotLibrary::InkpotArrayValueAsFloat( InValues, 0 );
	float B = UInkpotLibrary::InkpotArrayValueAsFloat( InValues, 1 );
	float C =  A * B;
	return UInkpotLibrary::MakeFloatInkpotValue( C );
}

FInkpotValue UInkFunctionTests::Times(const TArray<FInkpotValue> & InValues)
{
	int repeat = UInkpotLibrary::InkpotArrayValueAsInt( InValues, 0 );
	FString msg = UInkpotLibrary::InkpotArrayValueAsString( InValues, 1 );
	FString output;
	for( int i=0 ; i <repeat ; ++i)
		output += msg;
	return UInkpotLibrary::MakeStringInkpotValue( output );
}

FInkpotValue UInkFunctionTests::TRUE(const TArray<FInkpotValue> & InValues)
{
	return UInkpotLibrary::MakeBoolInkpotValue( true );
}
