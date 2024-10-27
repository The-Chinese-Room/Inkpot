#include "Blotter/BlotterOption.h"
#include "Blotter/BlotterVariable.h"

void UBlotterOption::Set(const FString& Name, bool bInIsSelected )
{
	Super::Set(Name);
	bIsSelected = bInIsSelected;
}

ECheckBoxState UBlotterOption::GetState() const
{
	if(bIsSelected)
		return ECheckBoxState::Checked;
	else
		return ECheckBoxState::Unchecked;
}

bool UBlotterOption::IsSelected() const
{
	return bIsSelected;
}

void UBlotterOption::SetSelected( bool bInIsSelected )
{
	bIsSelected = bInIsSelected;
	UBlotterVariable *variable = Cast<UBlotterVariable>( GetOuter() );
	if(!variable)
		return;
	variable->SetListOption( this );
}
