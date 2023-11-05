#include "Ink/Object.h"
#include "Ink/Path.h"
#include "Ink/Container.h"
#include "Ink/SearchResult.h"
#include "Ink/DebugMetadata.h"
#include "Utility/InkPlusPlusLog.h"


Ink::FStoryExceptionSubstitute Ink::FObject::StoryException;


Ink::FObject::FObject()
	: RootContentContainer(nullptr)
{
}

Ink::FObject::~FObject()
{
}

FString Ink::FObject::ToString()
{
	return FString();
}

int32 Ink::FObject::GetHashCode()
{
	return GetTypeHash(FString());
}

TSharedPtr<Ink::FObject> Ink::FObject::GetParent() const
{
	return Parent;
}

void Ink::FObject::SetParent(TSharedPtr<Ink::FObject> InParent)
{
	Parent = InParent;
}

TSharedPtr<Ink::FPath> Ink::FObject::GetPath()
{
	if (!_Path.IsValid())
	{
		if (!Parent.IsValid())
		{
			_Path = MakeShared<Ink::FPath>();
		}
		else
		{
			// We're iterating up the hierarchy from the leaves/children to the root.
			TArray<TSharedPtr<Ink::FComponent>> componentsStack;

			TSharedPtr<FObject> child = this->AsShared();
			TSharedPtr<FContainer> container = DynamicCastTo<FContainer>(child->GetParent());

			while (container.IsValid())
			{
				if (child.IsValid() && child->HasValidName())
				{
					componentsStack.Push(MakeShared<Ink::FComponent>(child->GetName()));
				}
				else
				{
					int32 index = INDEX_NONE;
					container->GetContent()->Find(child, index);
					componentsStack.Push(MakeShared<Ink::FComponent>(index));
				}

				child = container;
				container = DynamicCastTo<FContainer>(container->Parent);
			}
			Algo::Reverse(componentsStack); // explicitly reverse the array, instead of copying the c# code's implicit technique of handing over a stack to an array

			_Path = MakeShared<Ink::FPath>(componentsStack);
		}
	}

	return _Path;
}

Ink::FSearchResult Ink::FObject::ResolvePath(TSharedPtr<Ink::FPath> InPath)
{
	if (InPath->IsRelative())
	{
		TSharedPtr<Ink::FContainer> nearestContainer = DynamicCastTo<Ink::FContainer>(this->AsShared());
		if (!nearestContainer.IsValid())
		{
			ensureAlwaysMsgf(Parent != nullptr, TEXT("Can't resolve relative path because we don't have a parent"));
			nearestContainer = DynamicCastTo<Ink::FContainer>(Parent);
			ensureAlwaysMsgf(nearestContainer != nullptr, TEXT("Expected parent to be a container"));
			ensureAlways(InPath->GetComponent(0)->IsParent());
			InPath = InPath->GetTail();
		}

		return nearestContainer->GetContentAtPath(InPath);
	}
	else
	{
		return GetRootContentContainer()->GetContentAtPath(InPath);
	}
}

TSharedPtr<Ink::FContainer> Ink::FObject::GetRootContentContainer()
{
	TSharedPtr<Ink::FObject> ancestor = this->AsShared();
	while (ancestor->Parent.IsValid())
	{
		ancestor = ancestor->Parent;
	}

	return FObject::DynamicCastTo<FContainer>(ancestor);
}

TSharedPtr<Ink::FPath> Ink::FObject::ConvertPathToRelative(TSharedPtr<Ink::FPath> InGlobalPath)
{
	// 1. Find last shared ancestor
	// 2. Drill up using ".." style (actually represented as "^")
	// 3. Re-build downward chain from common ancestor

	TSharedPtr<Ink::FPath> ownPath = GetPath();

	const int32 minPathLength = FMath::Min<int32>(InGlobalPath->GetLength(), ownPath->GetLength());
	int32 lastSharedPathComponentIndex = -1;

	for(int32 i = 0; i < minPathLength; ++i)
	{
		Ink::FComponent* ownComponent = ownPath->GetComponent(i).Get();
		Ink::FComponent* otherComponent = InGlobalPath->GetComponent(i).Get();

		if (ownComponent->Equals(*otherComponent))
			lastSharedPathComponentIndex = i;
		else
			break;
	}

	// No shared path components, so just use global path
	if (lastSharedPathComponentIndex == -1)
		return InGlobalPath;

	const int32 numUpwardsMoves = (ownPath->GetLength() - 1) - lastSharedPathComponentIndex;
	TArray<TSharedPtr<Ink::FComponent>> newPathComponents;

	for (int32 up = 0; up < numUpwardsMoves; ++up)
	{
		newPathComponents.Add(FComponent::ToParent());
	}

	for (int32 down = lastSharedPathComponentIndex + 1; down < InGlobalPath->GetLength(); ++down)
	{
		newPathComponents.Add(InGlobalPath->GetComponent(down));
	}

	TSharedPtr<Ink::FPath> relativePath = MakeShared<Ink::FPath>(newPathComponents, true);
	return relativePath;
}

FString Ink::FObject::CompactPathString(TSharedPtr<Ink::FPath> InPath)
{
	FString globalPathString;
	FString relativePathString;

	if (InPath->IsRelative())
	{
		relativePathString = InPath->GetComponentsString();
		globalPathString = GetPath()->GetPathByAppendingPath(InPath)->GetComponentsString();
	}
	else
	{
		const TSharedPtr<Ink::FPath> relativePath = ConvertPathToRelative(InPath);
		relativePathString = relativePath->GetComponentsString();
		globalPathString = InPath->GetComponentsString();
	}

	if (relativePathString.Len() < globalPathString.Len())
		return relativePathString;
	else
		return globalPathString;
}

bool Ink::FObject::HasParent() const
{
	return Parent.IsValid();
}

FString Ink::FObject::GetName() const
{
	return Name;
}

bool Ink::FObject::HasValidName() const
{
	return Name.IsEmpty() == false;
}

void Ink::FObject::SetName(const FString& InName)
{
	Name = InName;
}

TSharedPtr<Ink::FDebugMetadata> Ink::FObject::GetDebugMetadata() const
{
	if (!DebugMetadata.IsValid())
	{
		if (Parent.IsValid())
		{
			return Parent->DebugMetadata;
		}
	}

	return DebugMetadata;
}

TSharedPtr<Ink::FDebugMetadata> Ink::FObject::GetOwnDebugMetadata() const
{
	return DebugMetadata;
}

void Ink::FObject::SetDebugMetadata(const Ink::FDebugMetadata& InDebugMetadata)
{
	DebugMetadata = MakeShared<Ink::FDebugMetadata>(InDebugMetadata);
}

int32 Ink::FObject::DebugLineNumberOfPath(TSharedPtr<Ink::FPath> InPath) const
{
	if (!InPath.IsValid())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Object : failed to get debug line number of the passed in path!"));
		return -1;
	}

	// Try to get a line number from debug metadata
	if (RootContentContainer != nullptr)
	{
		const TSharedPtr<Ink::FObject> targetContent = RootContentContainer->GetContentAtPath(InPath).GetObject();
		if (targetContent.IsValid())
		{
			const TSharedPtr<Ink::FDebugMetadata> debugMetadata = targetContent->GetDebugMetadata();
			if (debugMetadata.IsValid())
				return debugMetadata->StartLineNumber;
		}
	}

	//UE_LOG(InkPlusPlus, Error, TEXT("Object: failed to get a debug line :("))
	return -1;
}

