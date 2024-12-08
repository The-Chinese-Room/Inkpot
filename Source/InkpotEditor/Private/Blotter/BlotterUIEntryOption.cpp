#include "Blotter/BlotterUIEntryOption.h"

void UBlotterUIEntryOption::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UBlotterOption *option = Cast<UBlotterOption>(ListItemObject);
	if(!option)
		return;
	ReceiveOnListUpdate( option );
}
