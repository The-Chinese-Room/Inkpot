#include "Blotter/BlotterUIEntryVariable.h"
#include "Blotter/BlotterVariable.h"

void UBlotterUIEntryVariable::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	BlotterVariable = Cast<UBlotterVariable>(InListItemObject);
	if (!BlotterVariable)
		return;
	ReceiveOnVariableUpdate( BlotterVariable );

}

void UBlotterUIEntryVariable::SetVariableFromText(const FText& Value)
{
	if(!IsValid(BlotterVariable))
		return;

	bool bSuccess = BlotterVariable->SetVariableFromString( Value.ToString() );
	if(!bSuccess)
		ReceiveOnVariableUpdate( BlotterVariable );
}
