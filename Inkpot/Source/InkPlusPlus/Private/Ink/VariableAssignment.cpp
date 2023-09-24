#include "Ink/VariableAssignment.h"

Ink::FVariableAssignment::FVariableAssignment()
	: VariableName(FString()),
	bIsNewDeclaration(false),
	bIsGlobal(false)
{
}

Ink::FVariableAssignment::FVariableAssignment(const FString& InVariableName, bool InIsNewDeclaration)
	: VariableName(InVariableName),
	bIsNewDeclaration(InIsNewDeclaration),
	bIsGlobal(false)
{
}

FString Ink::FVariableAssignment::ToString()
{
	return FString::Printf(TEXT("VarAssign to %s"), *VariableName);
}

FString Ink::FVariableAssignment::GetVariableName() const
{
	return VariableName;
}

bool Ink::FVariableAssignment::IsNewDeclaration() const
{
	return bIsNewDeclaration;
}

bool Ink::FVariableAssignment::IsGlobal() const
{
	return bIsGlobal;
}

void Ink::FVariableAssignment::IsGlobal(bool InIsGlobal)
{
	bIsGlobal = InIsGlobal;
}
