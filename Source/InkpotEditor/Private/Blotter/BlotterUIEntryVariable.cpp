#include "Blotter/BlotterUIEntryVariable.h"
#include "Blotter/BlotterVariable.h"

void UBlotterUIEntryVariable::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	BlotterVariable = Cast<UBlotterVariable>(InListItemObject);
	if(!IsValid(BlotterVariable))
		return;
	BlotterVariable->SetDisplayWidget( this );
	ReceiveOnVariableUpdate();
}

void UBlotterUIEntryVariable::SetVariableFromText(const FText& Value)
{
	if(!IsValid(BlotterVariable))
		return;
	bool bSuccess = BlotterVariable->SetVariableFromString( Value.ToString() );
	if(!bSuccess)
		ReceiveOnVariableUpdate();
}

void UBlotterUIEntryVariable::Refresh()
{
	if(!IsValid(BlotterVariable))
		return;
	BlotterVariable->Refresh();
	ReceiveOnVariableUpdate();
}
