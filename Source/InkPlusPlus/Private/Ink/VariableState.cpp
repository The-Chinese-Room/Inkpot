#include "Ink/VariableState.h"
#include "Ink/Value.h"
#include "Ink/Object.h"
#include "Ink/JsonSerialisation.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FVariableState::FVariableState(TSharedPtr<Ink::FCallStack> CallStack, TSharedPtr<Ink::FListDefinitionsOrigin> ListDefsOrigin)
	: _batchObservingVariableChanges(false)
	, _dontSaveDefaultValues(true)
	, _callStack(CallStack)
	, _listDefsOrigin(ListDefsOrigin)
{
}


TSharedPtr<Ink::FStatePatch> Ink::FVariableState::GetStatePatch()
{
	return patch;
}


void Ink::FVariableState::SetStatePatch(const TSharedPtr<Ink::FStatePatch>& Patch)
{
	patch = Patch;
}


bool Ink::FVariableState::GetBatchObservingVariableChanges() const
{
	return _batchObservingVariableChanges;
}


void Ink::FVariableState::SetBatchObservingVariableChanges(bool Value)
{
	_batchObservingVariableChanges = Value;
	if (!Value && VariableChanged.IsBound())
	{
		for (const FString& variableName : _changedVariablesForBatchObs)
		{
			const TSharedPtr<Ink::FObject> value = _globalVariables[variableName];
			VariableChanged.Broadcast(variableName, value);
		}
	}
	_changedVariablesForBatchObs.Reset();
}


TSharedPtr<Ink::FCallStack> Ink::FVariableState::GetCallStack()
{
	return _callStack;
}


void Ink::FVariableState::SetCallStack(TSharedPtr<Ink::FCallStack> CallStack)
{
	_callStack = CallStack;
}


TSharedPtr<Ink::FObject> Ink::FVariableState::GetVariable(const FString& VariableName)
{
	TSharedPtr<Ink::FObject> global;
	if (patch.IsValid() && patch->TryGetGlobal(VariableName, global) && global.IsValid())
	{
		return global;
	}

	// Search main dictionary first.
	// If it's not found, it might be because the story content has changed,
	// and the original default value hasn't be instantiated.
	// Should really warn somehow, but it's difficult to see how...!
	TSharedPtr<Ink::FObject> varContents;
	TSharedPtr<Ink::FObject>* findResult = _globalVariables.Find(VariableName);
	if (findResult != nullptr && (*findResult).IsValid())
	{
		varContents = *findResult;
		return varContents;
	}

	findResult = _defaultGlobalVariables.Find(VariableName);
	if (findResult != nullptr && (*findResult).IsValid())
	{
		varContents = *findResult;
		return varContents;
	}

	return varContents;
}


void Ink::FVariableState::SetVariable(const FString& VariableName, const TSharedPtr<Ink::FObject>& Value)
{
	if (!_defaultGlobalVariables.Contains(VariableName))
	{
		FObject::StoryException.Throw("Cannot assign to a variable (" + VariableName + ") that hasn't been declared in the story");
		return;
	}

	if (!Value.IsValid())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Cannot pass null to VariableState"));
		return;
	}

	if (!FObject::DynamicCastTo<Ink::FValue>(Value).IsValid())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Invalid value passed to VariableState : %s"), *Value->ToString());
		return;
	}

	SetGlobal(VariableName, Value);
}


void Ink::FVariableState::ApplyPatch()
{
	if (patch.IsValid())
	{
		_globalVariables.Append(patch->GetGlobals());
		_changedVariablesForBatchObs.Append(patch->GetChangedVariables());
	}

	patch.Reset();
}


void Ink::FVariableState::SetJsonToken(const TMap<FString, TSharedPtr<FJsonValue>>& JsonToken)
{
	_globalVariables.Reset();

	for (const TPair<FString, TSharedPtr<Ink::FObject>>& variablePair : _defaultGlobalVariables)
	{
		const FString variableName = variablePair.Key;

		const TSharedPtr<FJsonValue>* loadedTokenPtrPtr = JsonToken.Find(variableName);
		if (loadedTokenPtrPtr != nullptr && (*loadedTokenPtrPtr).IsValid())
		{
			_globalVariables.Emplace( variableName, Ink::FJsonSerialisation::JsonTokenToRuntimeObject(**loadedTokenPtrPtr) );
		}
		else
		{
			_globalVariables.Emplace( variableName, variablePair.Value );
		}
	}
}


void Ink::FVariableState::WriteJson(TJsonWriter<>* Writer)
{
	Writer->WriteObjectStart();
	for (const auto& pair: _globalVariables)
	{
		const FString key = pair.Key;
		const TSharedPtr<FObject> value = pair.Value;

		if (_dontSaveDefaultValues)
		{
			// Don't write out values that are the same as the default global values
			TSharedPtr<Ink::FObject>* defaultValuePtr = _defaultGlobalVariables.Find(key);
			if (defaultValuePtr != nullptr && RuntimeObjectsEqual(value, *defaultValuePtr))
			{
				continue;
			}
		}

		Writer->WriteIdentifierPrefix( *key );
		Ink::FJsonSerialisation::WriteRuntimeObject(Writer, value);
	}
	Writer->WriteObjectEnd();
}


bool Ink::FVariableState::RuntimeObjectsEqual(TSharedPtr<Ink::FObject> Object1, TSharedPtr<Ink::FObject> Object2) const
{
	bool objectsAreEqual = false;

	if (Object1.IsValid() && Object2.IsValid())
	{
		TSharedPtr<FValue> object1AsFValue = FObject::DynamicCastTo<Ink::FValue>(Object1);
		TSharedPtr<FValue> object2AsFValue = FObject::DynamicCastTo<Ink::FValue>(Object2);
		if (object1AsFValue.IsValid() && object2AsFValue.IsValid())
		{
			objectsAreEqual = object1AsFValue->ToString() == object2AsFValue->ToString();
		}
	}
	return objectsAreEqual;
}

const TMap<FString, TSharedPtr<Ink::FObject>>& Ink::FVariableState::GetVariables() const
{
	return _globalVariables;
}

TSharedPtr<Ink::FObject> Ink::FVariableState::GetVariableWithName(const FString& Name)
{
	return GetVariableWithName(Name, -1);
}


TSharedPtr<Ink::FObject> Ink::FVariableState::TryGetDefaultVariableName(const FString& Name)
{
	TSharedPtr<Ink::FObject> returnValue;
	TSharedPtr<Ink::FObject>* findResult = _defaultGlobalVariables.Find(Name);
	if (findResult != nullptr)
	{
		returnValue = *findResult;
	}
	return returnValue;
}


bool Ink::FVariableState::GlobalVariableExistsWithName(const FString& Name)
{
	const bool isGlobalVariable = _globalVariables.Contains(Name) || _defaultGlobalVariables.Contains(Name);
	return isGlobalVariable;
}


TSharedPtr<Ink::FObject> Ink::FVariableState::GetVariableWithName(const FString& Name, int ContextIndex)
{
	TSharedPtr<Ink::FObject> varValue = GetRawVariableWithName(Name, ContextIndex);

	if (varValue.IsValid())
	{
		// Get value from pointer?
		if (const TSharedPtr<Ink::FValueVariablePointer> varPointer = FObject::DynamicCastTo<Ink::FValueVariablePointer>(varValue))
		{
			varValue = GetVariableWithName(varPointer->GetVariableName(), varPointer->GetContextIndex()); // recursive!
		}
	}

	return varValue;
}


TSharedPtr<Ink::FObject> Ink::FVariableState::GetRawVariableWithName(const FString& Name, int ContextIndex)
{
	TSharedPtr<Ink::FObject> varValue;

	// 0 context = global
	if (ContextIndex == 0 || ContextIndex == -1)
	{
		if (patch.IsValid() && patch->TryGetGlobal(Name, varValue))
		{
			return varValue;
		}

		TSharedPtr<Ink::FObject>* findResult = _globalVariables.Find(Name);
		if (findResult != nullptr)
		{
			varValue = *findResult;
			return varValue;
		}

		// <C# comment from original codebase so take with pinch of salt>
		// Getting variables can actually happen during globals set up since you can do
		//  VAR x = A_LIST_ITEM
		// So _defaultGlobalVariables may be null
		// We need to do this check though in case a new global is added, so we need to
		// revert to the default globals dictionary since an initial value hasn't yet been set.
		findResult = _defaultGlobalVariables.Find(Name);
		if (findResult != nullptr)
		{
			varValue = *findResult;
			return varValue;
		}

		TSharedPtr<Ink::FValueList> listItemValue = _listDefsOrigin->FindSingleItemListWithName(Name);
		if (listItemValue.IsValid())
		{
			return listItemValue;
		}
	}

	// Temporary
	varValue = _callStack->GetTemporaryVariableWithName(Name, ContextIndex);
	return varValue;
}


void Ink::FVariableState::Assign(TSharedPtr<Ink::FVariableAssignment> VarAss, TSharedPtr<Ink::FObject> Value)
{
	FString name = VarAss->GetVariableName();
	int contextIndex = -1;

	// Are we assigning to a global variable?
	bool setGlobal = false;
	if (VarAss->IsNewDeclaration())
	{
		setGlobal = VarAss->IsGlobal();
	}
	else
	{
		setGlobal = GlobalVariableExistsWithName(name);
	}

	// Constructing new variable pointer reference
	if (VarAss->IsNewDeclaration())
	{
		TSharedPtr<Ink::FValueVariablePointer> varPointer = FObject::DynamicCastTo<Ink::FValueVariablePointer>(Value);
		if (varPointer.IsValid())
		{
			TSharedPtr< Ink::FValueVariablePointer> fullyResolvedVariablePointer = ResolveVariablePointer(varPointer);
			Value = fullyResolvedVariablePointer;
		}
	}
	else
	{
		// Assign to existing variable pointer?
		// Then assign to the variable that the pointer is pointing to by name.
		// De-reference variable reference to point to
		TSharedPtr<Ink::FValueVariablePointer> existingPointer;
		do
		{
			existingPointer = FObject::DynamicCastTo<Ink::FValueVariablePointer>(GetRawVariableWithName(name, contextIndex));
			if (existingPointer.IsValid())
			{
				name = existingPointer->GetVariableName();
				contextIndex = existingPointer->GetContextIndex();
				setGlobal = (contextIndex == 0);
			}
			
		} while (existingPointer.IsValid());
	}

	if (setGlobal)
	{
		SetGlobal(name, Value);
	}
	else
	{
		_callStack->SetTemporaryVariable(name, Value, VarAss->IsNewDeclaration(), contextIndex);
	}
}


void Ink::FVariableState::SnapshotDefaultGlobals()
{
	_defaultGlobalVariables = _globalVariables;
}


void Ink::FVariableState::SetGlobal(const FString& VariableName, TSharedPtr<Ink::FObject> Value)
{
	TSharedPtr<Ink::FObject> oldValue;
	if (!patch.IsValid() || !patch->TryGetGlobal(VariableName, Value))
	{
		TSharedPtr<Ink::FObject>* findValue = _globalVariables.Find(VariableName);
		if (findValue != nullptr)
		{
			oldValue = *findValue;
		}
	}

	Ink::FValueList::RetainListOriginsForAssignment(oldValue, Value);

	if (patch.IsValid())
		patch->SetGlobal(VariableName, Value);
	else
		_globalVariables.Add(VariableName, Value);

	if (Value != oldValue)
	{
		if (_batchObservingVariableChanges)
		{
			if (patch.IsValid())
				patch->AddChangedVariable(VariableName);
			else
				_changedVariablesForBatchObs.Add(VariableName);
		}
		else
		{
			VariableChanged.Broadcast(VariableName, Value);
		}
	}
}


TSharedPtr<Ink::FValueVariablePointer> Ink::FVariableState::ResolveVariablePointer(TSharedPtr<Ink::FValueVariablePointer> VariablePointer)
{
	if (!VariablePointer.IsValid())
		return VariablePointer;
	
	int contextIndex = VariablePointer->GetContextIndex();
	const FString variableName = VariablePointer->GetVariableName();
	
	if (contextIndex == -1)
	{
		contextIndex = GetContextIndexOfVariableNamed(variableName);
	}

	// Extra layer of indirection:
	// When accessing a pointer to a pointer (e.g. when calling nested or 
	// recursive functions that take a variable references, ensure we don't create
	// a chain of indirection by just returning the final target.
	const TSharedPtr<Ink::FObject> valueOfVariablePointedTo = GetRawVariableWithName(variableName, contextIndex);
	if (valueOfVariablePointedTo.IsValid())
	{
		const TSharedPtr<FValueVariablePointer> doubleRedirectionPointer = FObject::DynamicCastTo<FValueVariablePointer>(valueOfVariablePointedTo);
		if (doubleRedirectionPointer.IsValid())
		{
			TSharedPtr< Ink::FValueVariablePointer> result = StaticCastSharedPtr<Ink::FValueVariablePointer>(valueOfVariablePointedTo);
			return result;
		}
	}
	TSharedPtr< Ink::FValueVariablePointer> result = MakeShared<Ink::FValueVariablePointer>(variableName, contextIndex);
	return result;
}


int Ink::FVariableState::GetContextIndexOfVariableNamed(const FString& varName)
{
	int contextIndex;
	if (GlobalVariableExistsWithName(varName))
	{
		contextIndex = 0;
	}
	else
	{
		contextIndex = _callStack->GetCurrentElementIndex();
	}
	return contextIndex;
}
