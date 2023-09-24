#include "Ink/Path.h"
#include "Utility/InkPlusPlusLog.h"
#include "Misc/DefaultValueHelper.h"

using namespace Ink;

#pragma region Path
FPath::FPath()
	: bIsRelative(false),
	_ComponentsString(FString())
{
}

FPath::FPath(const TArray<TSharedPtr<FComponent>>& InComponents, bool InIsRelative /*= false */)
	: bIsRelative(InIsRelative),
	_ComponentsString(FString())
{
	Components.Append(InComponents);
}

FPath::FPath(const FString& InComponentsString)
	: bIsRelative(false)
{
	SetComponentsString(InComponentsString);
}

bool FPath::Equals(TSharedPtr<FPath> InOtherPath) const
{
	if (!InOtherPath.IsValid())
		return false;
	
	if (InOtherPath->Components.Num() != Components.Num())
		return false;

	if (InOtherPath->IsRelative() != IsRelative())
		return false;

	// return otherPath._components.SequenceEqual (this._components); // c# source
	const FString myString = GetComponentsString();
	const FString otherString = InOtherPath->GetComponentsString();
	const bool equals = myString == otherString;
	return equals;
}


TSharedPtr<FComponent> FPath::GetComponent(int32 InIndex) const
{
	if (InIndex >= Components.Num() || InIndex < 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Failed to get Component, given index %d is out of components set bounds!"), InIndex);
		return nullptr;
	}
	
	return Components[InIndex];
}

TSharedPtr<FComponent> FPath::GetHead() const
{
	if (Components.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Failed to get Head, Components list is empty!"));
		return nullptr;
	}

	return Components[0];
}

TSharedPtr<FPath> FPath::GetTail() const
{
	if (Components.Num() >= 2)
	{
		TArray<TSharedPtr<FComponent>> tailComponents;
		for (int i = 1; i <= Components.Num() - 1; ++i)
		{
			tailComponents.Emplace(Components[i]);
		}
		return MakeShared<Ink::FPath>(tailComponents);
	}
	else
	{
		return FPath::Self();
	}
}

TSharedPtr<FComponent> FPath::GetLastComponent() const
{
	if (Components.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Failed to get last component, Component list is empty!"))
		return nullptr;
	}

	return Components.Last();
}

bool FPath::ContainsNamedComponent() const
{
	for (const TSharedPtr<FComponent>& component : Components)
	{
		if (!component->IsIndex())
			return true;
	}

	return false;
}

bool FPath::IsRelative() const
{
	return bIsRelative;
}

int32 FPath::GetLength() const
{
	return Components.Num();
}

TSharedPtr<FPath> FPath::GetPathByAppendingPath(TSharedPtr<FPath> InPathToAppend)
{
	TSharedPtr<FPath> path = MakeShared<Ink::FPath>();

	int upwardMoves = 0;
	for (int32 i = 0; i < InPathToAppend->Components.Num(); ++i)
	{
		if (InPathToAppend->Components[i]->IsParent())
			++upwardMoves;
		else
			break;
	}

	for (int32 i = 0; i < Components.Num() - upwardMoves; ++i)
	{
		path->Components.Add(Components[i]);
	}

	for (int32 i = upwardMoves; i < InPathToAppend->Components.Num(); ++i)
	{
		path->Components.Add(InPathToAppend->Components[i]);
	}

	return path;
}

TSharedPtr<FPath> FPath::GetPathByAppendingComponent(TSharedPtr<FComponent> InComponentToAppend) const
{
	TSharedPtr<FPath> path = MakeShared<Ink::FPath>();
	path->Components = Components;
	path->Components.Add(InComponentToAppend);
	return path;
}

FString FPath::GetComponentsString() const
{
	if (_ComponentsString.IsEmpty())
	{
		bool isFirst = true;
		for (const TSharedPtr<FComponent>& component : Components)
		{
			// Add separator
			if (!isFirst)
				_ComponentsString += TEXT(".");
			
			_ComponentsString += component->ToString();
			isFirst = false;
		}
		
		if (IsRelative())
		{
			_ComponentsString = TEXT(".") + _ComponentsString;
		}
	}

	return _ComponentsString;
}

void FPath::SetComponentsString(const FString& InComponentsString)
{
	Components.Empty();
	_ComponentsString = InComponentsString;

	// Empty path, empty components
	// (path is to root, like "/" in file system)
	if (_ComponentsString.IsEmpty())
		return;

	// When components start with ".", it indicates a relative path, e.g.
	//   .^.^.hello.5
	// is equivalent to file system style path:
	//  ../../hello/5
	if (_ComponentsString[0] == '.')
	{
		bIsRelative = true;
		_ComponentsString = _ComponentsString.Mid(1, _ComponentsString.Len() - 1);
	}
	else
	{
		bIsRelative = false;
	}

	TArray<FString> componentStrings;
	_ComponentsString.ParseIntoArray(componentStrings, TEXT("."));
	Components.Reserve(componentStrings.Num());
	
	for (const FString& string : componentStrings)
	{
		int32 index = -1;
		if (FDefaultValueHelper::ParseInt(string, index))
		{
			Components.Add(MakeShared<FComponent>(index));
		}
		else
		{
			Components.Add(MakeShared<FComponent>(string));
		}
	}
}

FString FPath::ToString() const
{
	return GetComponentsString();
}

uint32 FPath::GetHashCode() const
{
	return GetTypeHash(FString());
}

TSharedPtr<FPath> FPath::Self()
{
	TSharedPtr<FPath> path = MakeShared<FPath>();
	path->bIsRelative = true;
	return path;
}
#pragma endregion Path


#pragma region Component
FComponent::FComponent(int32 InIndex)
	: Index(InIndex),
	Name(FString())
{
	checkf(InIndex >= 0, TEXT("InIndex has to be 0 or greater!"));
}

FComponent::FComponent(const FString& InName)
	: Index(-1),
	Name(InName)
{
	checkf(InName != FString(), TEXT("InName cannot be default FString"));
}

FComponent::FComponent(const FComponent& InOtherComponent)
{
	Index = InOtherComponent.Index;
	Name = InOtherComponent.Name;
}

FComponent& FComponent::operator=(const FComponent& InOtherComponent)
{
	Index = InOtherComponent.Index;
	Name = InOtherComponent.Name;
	return *this;
}

FComponent::~FComponent()
{
}

bool FComponent::operator==(const FComponent& InOtherComponent) const
{
	return Equals(InOtherComponent);
}

int32 FComponent::GetIndex() const
{
	return Index;
}

FString FComponent::GetName() const
{
	return Name;
}

bool FComponent::IsIndex() const
{
	return Index >= 0;
}

bool FComponent::IsParent() const
{
	return Name == GParentID;
}

FString FComponent::ToString() const
{
	if (IsIndex())
		return FString::FromInt(Index);
	else
		return Name;
}

TSharedPtr<FComponent> FComponent::ToParent()
{
	return MakeShared<Ink::FComponent>(GParentID);
}

bool FComponent::Equals(const Ink::FComponent& InOtherComponent) const
{
	if (InOtherComponent.IsIndex() == IsIndex())
	{
		if (IsIndex())
			return Index == InOtherComponent.Index;
		else
			return Name == InOtherComponent.Name;
	}

	return false;
}
#pragma endregion Component
