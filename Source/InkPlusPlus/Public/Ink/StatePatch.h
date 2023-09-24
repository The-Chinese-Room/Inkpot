#pragma once
#include "CoreMinimal.h"
#include "Container.h"
#include "Object.h"


namespace Ink
{
	class FStatePatch
	{
	public:
		TMap<FString, TSharedPtr<Ink::FObject>>& GetGlobals() { return _globals; }
		TSet<FString>& GetChangedVariables() { return _changedVariables; }
		TMap<TSharedPtr<Ink::FContainer>, int>& GetVisitCounts() { return _visitCounts; }
		TMap<TSharedPtr<Ink::FContainer>, int>& GetTurnIndices() { return _turnIndices; }

		bool TryGetGlobal(const FString& Name, TSharedPtr<Ink::FObject>& OutValue);
		void SetGlobal(const FString& Name, const TSharedPtr<Ink::FObject>& Value);
		void AddChangedVariable(const FString& Name);
		bool TryGetVisitCount(TSharedPtr<Ink::FContainer> Container, int& OutCount);
		void SetVisitCount(TSharedPtr<Ink::FContainer> Container, int Count);
		bool TryGetTurnIndex(TSharedPtr<Ink::FContainer> Container, int& OutIndex);
		void SetTurnIndex(TSharedPtr<Ink::FContainer> Container, int Index);

	private:
		TMap<FString, TSharedPtr<Ink::FObject>> _globals;
		TSet<FString> _changedVariables;
		TMap<TSharedPtr<Ink::FContainer>, int> _visitCounts;
		TMap<TSharedPtr<Ink::FContainer>, int> _turnIndices;
	};
}
