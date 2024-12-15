#include "Outliner/BlotterUIEntryContainer.h"
#include "Outliner/BlotterContainer.h"

void UBlotterUIEntryContainer::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	UBlotterContainer* var = Cast<UBlotterContainer>(InListItemObject);
	if (!var)
		return;
	ReceiveOnContainerUpdate(var);
}
