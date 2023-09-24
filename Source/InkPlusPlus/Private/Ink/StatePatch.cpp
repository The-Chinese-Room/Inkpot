#include "Ink/StatePatch.h"


bool Ink::FStatePatch::TryGetGlobal(const FString& Name, TSharedPtr<Ink::FObject>& OutValue)
{
	TSharedPtr<Ink::FObject>* result = _globals.Find(Name);
	if (result != nullptr)
	{
		OutValue = *result;
		return true;
	}
	return false;
}


void Ink::FStatePatch::SetGlobal(const FString& Name, const TSharedPtr<Ink::FObject>& Value)
{
	_globals.Add(Name, Value);
}


void Ink::FStatePatch::AddChangedVariable(const FString& Name)
{
	_changedVariables.Add(Name);
}


bool Ink::FStatePatch::TryGetVisitCount(TSharedPtr<Ink::FContainer> Container, int& OutCount)
{
	int* result = _visitCounts.Find(Container);
	if (result != nullptr)
	{
		OutCount = *result;
		return true;
	}
	return false;
}


void Ink::FStatePatch::SetVisitCount(TSharedPtr<Ink::FContainer> Container, int Count)
{
	_visitCounts.Add(Container, Count);
}


bool Ink::FStatePatch::TryGetTurnIndex(TSharedPtr<Ink::FContainer> Container, int& OutIndex)
{
	int* result = _turnIndices.Find(Container);
	if (result != nullptr)
	{
		OutIndex = *result;
		return true;
	}
	return false;
}


void Ink::FStatePatch::SetTurnIndex(TSharedPtr<Ink::FContainer> Container, int Index)
{
	_turnIndices.Add(Container, Index);
}
