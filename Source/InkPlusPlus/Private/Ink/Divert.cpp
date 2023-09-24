#include "Ink/Divert.h"
#include "Ink/Path.h"
#include "Ink/Pointer.h"
#include "Ink/Container.h"
#include "Ink/SearchResult.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FDivert::FDivert()
	: VariableDivertName(FString()),
	bPushesToStack(false),
	bIsExternal(false),
	bIsConditional(false),
	ExternalArgs(0),
	StackPushType(Ink::EPushPopType::None)
{
}

Ink::FDivert::FDivert(const Ink::EPushPopType& InStackPushType)
	: VariableDivertName(FString()),
	bPushesToStack(true),
	bIsExternal(false),
	bIsConditional(false),
	ExternalArgs(0),
	StackPushType(InStackPushType)
{
}

FString Ink::FDivert::ToString()
{
	if (HasVariableTarget())
	{
		return FString(TEXT("Divert(variable: " + VariableDivertName + ")"));
	}
	else if (!GetTargetPath().IsValid())
	{
		return FString(TEXT("Divert(null)"));
	}
	else
	{
		FString outputString;
		FString targetString = GetTargetPath()->ToString();
		const int32 targetLineNum = DebugLineNumberOfPath(GetTargetPath());
		if (targetLineNum > -1)
		{
			targetString += FString::Printf(TEXT("line %d"), targetLineNum);
		}

		outputString += TEXT("Divert");

		if (bIsConditional)
			outputString += TEXT("?");

		if (bPushesToStack)
		{
			if (StackPushType == Ink::EPushPopType::Function)
				outputString += TEXT(" function");
			else
				outputString += TEXT(" tunnel");
		}

		outputString += TEXT(" -> ");
		outputString += GetTargetPathString();

		outputString += TEXT(" (");
		outputString += targetString;
		outputString += TEXT(")");

		return outputString;
	}
}

bool Ink::FDivert::Equals(Ink::FDivert& InOtherDivert)
{
	if (HasVariableTarget() == InOtherDivert.HasVariableTarget())
	{
		if (HasVariableTarget())
			return VariableDivertName == InOtherDivert.VariableDivertName;
		else
			return GetTargetPath()->Equals(InOtherDivert.GetTargetPath());
	}
	return false;
}

int32 Ink::FDivert::GetHashCode()
{
	if (HasVariableTarget())
	{
		const uint32 variableTargetSalt = 12345;
		return GetTypeHash(VariableDivertName) + variableTargetSalt;
	}
	else
	{
		const uint32 pathTargetSalt = 54321;
		return GetTargetPath()->GetHashCode() + pathTargetSalt;
	}
}

TSharedPtr<Ink::FPath> Ink::FDivert::GetTargetPath()
{
	// Resolve any relative paths to global ones as we come across them
	if (_TargetPath.IsValid() && _TargetPath->IsRelative())
	{
		TSharedPtr<Ink::FObject> targetObject = GetTargetPointer().Resolve();
		if (targetObject.IsValid())
			_TargetPath = targetObject->GetPath();
	}
	return _TargetPath;
}

void Ink::FDivert::SetTargetPath(TSharedPtr<Ink::FPath> InTargetPath)
{
	_TargetPath = InTargetPath;
	_TargetPointer = Ink::FPointer::Null();
}

FString Ink::FDivert::GetTargetPathString()
{
	TSharedPtr<Ink::FPath> targetPath = GetTargetPath();
	if (!targetPath.IsValid())
		return FString();

	return CompactPathString(targetPath);
}

void Ink::FDivert::SetTargetPathString(const FString& InTargetPathString)
{
	if (InTargetPathString.IsEmpty())
		SetTargetPath(nullptr);
	else
		SetTargetPath(MakeShared<Ink::FPath>(InTargetPathString));
}

Ink::FPointer Ink::FDivert::GetTargetPointer()
{
	if (_TargetPointer.IsNull())
	{
		const TSharedPtr<Ink::FObject> targetObject = ResolvePath(_TargetPath).GetObject();

		if (_TargetPath->GetLastComponent()->IsIndex())
		{
			const TSharedPtr<Ink::FContainer> targetContainerParent = DynamicCastTo<Ink::FContainer>(targetObject->GetParent());
			_TargetPointer.SetContainer(targetContainerParent);

			const int32 targetIndex = _TargetPath->GetLastComponent()->GetIndex();
			_TargetPointer.SetIndex(targetIndex);
		}
		else
		{
			const TSharedPtr<Ink::FContainer> targetContainer = DynamicCastTo<Ink::FContainer>(targetObject);
			_TargetPointer = Ink::FPointer::StartOf(targetContainer);
		}
	}
	
	return _TargetPointer;
}

bool Ink::FDivert::HasVariableTarget() const
{
	return !VariableDivertName.IsEmpty();
}

FString Ink::FDivert::GetVariableDivertName() const
{
	return VariableDivertName;
}

void Ink::FDivert::SetVariableDivertName(const FString& InVariableDivertName)
{
	if (InVariableDivertName.IsEmpty())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Diver : failed top set variable diver name, passed in name is empty!"));
		return;
	}
	VariableDivertName = InVariableDivertName;
}

bool Ink::FDivert::GetPushesToStack() const
{
	return bPushesToStack;
}

void Ink::FDivert::SetPushesToStack(bool InPushesToStack)
{
	bPushesToStack = InPushesToStack;
}

bool Ink::FDivert::IsExternal() const
{
	return bIsExternal;
}

void Ink::FDivert::SetIsExternal(bool InIsExternal)
{
	bIsExternal = InIsExternal;
}

bool Ink::FDivert::IsConditional() const
{
	return bIsConditional;
}

void Ink::FDivert::SetIsConditional(bool InIsConditional)
{
	bIsConditional = InIsConditional;
}

Ink::EPushPopType Ink::FDivert::GetStackPushType() const
{
	return StackPushType;
}

void Ink::FDivert::SetStackPushType(const Ink::EPushPopType& InStackPushType)
{
	StackPushType = InStackPushType;
}

int32 Ink::FDivert::GetExternalsArgs() const
{
	return ExternalArgs;
}

void Ink::FDivert::SetExternalArgs(int32 InExternalArgs)
{
	ExternalArgs = InExternalArgs;
}
