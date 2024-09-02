#pragma once
#include "CoreMinimal.h"
#include "Object.h"
#include "CallStack.h"
#include "StatePatch.h"
#include "VariableAssignment.h"
#include "Dom/JsonValue.h"
#include "ListDefinitionsOrigin.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FInkVariableObserver, const FString&, TSharedPtr<Ink::FObject>);

class JsonObject;


namespace Ink
{
	class FValueVariablePointer;


	class INKPLUSPLUS_API FVariableState
	{
	public:
		FInkVariableObserver VariableChanged;

		FVariableState(TSharedPtr<Ink::FCallStack> CallStack, TSharedPtr<Ink::FListDefinitionsOrigin> ListDefsOrigin);

		TSharedPtr<Ink::FStatePatch> GetStatePatch();
		void SetStatePatch(const TSharedPtr<Ink::FStatePatch>& Patch);

		bool GetBatchObservingVariableChanges() const;
		void SetBatchObservingVariableChanges(bool Value);

		// Allow StoryState to change the current callstack, e.g. for
		// temporary function evaluation.
		TSharedPtr<Ink::FCallStack> GetCallStack();
		void SetCallStack(TSharedPtr<Ink::FCallStack> CallStack);

		TSharedPtr<Ink::FObject> GetVariable(const FString& VariableName);
		bool SetVariable(const FString& VariableName, const TSharedPtr<Ink::FObject>& Value);

		void ApplyPatch();
		void SetJsonToken(const TMap<FString, TSharedPtr<FJsonValue>>& JsonToken);
		void WriteJson(TJsonWriter<>* Writer);
		bool RuntimeObjectsEqual(TSharedPtr<Ink::FObject> Object1, TSharedPtr<Ink::FObject> Object2) const;

		const TMap<FString, TSharedPtr<Ink::FObject>>& GetVariables() const;
		TSharedPtr<Ink::FObject> GetVariableWithName(const FString& Name);
		TSharedPtr<Ink::FObject> TryGetDefaultVariableName(const FString& Name);
		bool GlobalVariableExistsWithName(const FString& Name);
		TSharedPtr<Ink::FObject> GetVariableWithName(const FString& Name, int ContextIndex);
		TSharedPtr<Ink::FObject> GetRawVariableWithName(const FString& Name, int ContextIndex);
		void Assign(TSharedPtr<Ink::FVariableAssignment> VarAss, TSharedPtr<Ink::FObject> Value);
		void SnapshotDefaultGlobals();

		void SetGlobal(const FString& VariableName, TSharedPtr<Ink::FObject> Value);
		TSharedPtr<Ink::FValueVariablePointer> ResolveVariablePointer(TSharedPtr<Ink::FValueVariablePointer> VariablePointer);
		int GetContextIndexOfVariableNamed(const FString& varName);

	private:
		TSharedPtr<Ink::FStatePatch> patch;
		bool _batchObservingVariableChanges;
		bool _dontSaveDefaultValues;

		TMap<FString, TSharedPtr<Ink::FObject>> _globalVariables;
		TMap<FString, TSharedPtr<Ink::FObject>> _defaultGlobalVariables;

		// used for accessing temporary variables
		TSharedPtr<Ink::FCallStack> _callStack;
		TSet<FString> _changedVariablesForBatchObs;
		TSharedPtr<Ink::FListDefinitionsOrigin> _listDefsOrigin;
	};
}
