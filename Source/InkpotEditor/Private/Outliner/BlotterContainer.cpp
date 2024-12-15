#include "Outliner/BlotterContainer.h"

void UBlotterContainer::Add(UBlotterContainer *InContainer)
{
	Children.Add( InContainer );
}

int32 UBlotterContainer::GetLevel() const
{
	return Level;
}

void UBlotterContainer::SetLevel( int32 InLevel )
{
	Level = InLevel;
}

const TArray<UBlotterContainer*> &UBlotterContainer::GetChildren() const
{
	return Children;
}
