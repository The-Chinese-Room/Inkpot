#include "Test/InkFunctionTests.h"
#include "Inkpot/InkpotValueLibrary.h"

const FString UInkFunctionTests::FuncName_Message = TEXT("message");
const FString UInkFunctionTests::FuncName_Multiply = TEXT("multiply");
const FString UInkFunctionTests::FuncName_Times = TEXT("times");
const FString UInkFunctionTests::FuncName_TRUE = TEXT("TRUE");

FInkpotValue UInkFunctionTests::Message(const TArray<FInkpotValue> & InValues)
{
	FString output = UInkpotValueLibrary::InkpotArrayValueAsString( InValues, 0 );
	return UInkpotValueLibrary::MakeStringInkpotValue( output );
}

FInkpotValue UInkFunctionTests::Multiply(const TArray<FInkpotValue> & InValues)
{
	float A = UInkpotValueLibrary::InkpotArrayValueAsFloat( InValues, 0 );
	float B = UInkpotValueLibrary::InkpotArrayValueAsFloat( InValues, 1 );
	float C =  A * B;
	return UInkpotValueLibrary::MakeFloatInkpotValue( C );
}

FInkpotValue UInkFunctionTests::Times(const TArray<FInkpotValue> & InValues)
{
	int repeat = UInkpotValueLibrary::InkpotArrayValueAsInt( InValues, 0 );
	FString msg = UInkpotValueLibrary::InkpotArrayValueAsString( InValues, 1 );
	FString output;
	for( int i=0 ; i <repeat ; ++i)
		output += msg;
	return UInkpotValueLibrary::MakeStringInkpotValue( output );
}

FInkpotValue UInkFunctionTests::TRUE(const TArray<FInkpotValue> & InValues)
{
	return UInkpotValueLibrary::MakeBoolInkpotValue( true );
}
