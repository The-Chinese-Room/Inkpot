#include "Blotter/BlotterUIEntryString.h"
#include "Blotter/BlotterString.h"

void UBlotterUIEntryString::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	UBlotterString* var = Cast<UBlotterString>(InListItemObject);
	if (!var)
		return;
	ReceiveOnTextUpdate(var);
}
