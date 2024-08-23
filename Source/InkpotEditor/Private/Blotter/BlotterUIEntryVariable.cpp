#include "Blotter/BlotterUIEntryVariable.h"
#include "Blotter/BlotterVariable.h"

void UBlotterUIEntryVariable::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	UBlotterVariable* var = Cast<UBlotterVariable>(InListItemObject);
	if (!var)
		return;
	ReceiveOnVariableUpdate(var);
}
