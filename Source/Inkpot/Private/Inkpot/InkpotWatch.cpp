#include "Inkpot/InkpotWatch.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"

void UInkpotWatch::BeginPlay()
{
	Super::BeginPlay();
	BindOnBeginStory();
}

void UInkpotWatch::BindOnBeginStory()
{
	UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	check(inkpot);
	inkpot->EventOnStoryBegin.AddDynamic( this, &UInkpotWatch::OnBeginStory );
}

void UInkpotWatch::OnBeginStory( UInkpotStory* InStory )
{
	FOnInkpotVariableChange delegate;
	delegate.BindDynamic(this, &UInkpotWatch::OnVariableChange );
	InStory->SetOnVariableChange( delegate, VariableWatch );
}

void UInkpotWatch::OnVariableChange( UInkpotStory* InStory, const FString & InVariable, const FInkpotValue & InNewValue )
{
	if(!EventOnVariableChange.IsBound())
		return;
	EventOnVariableChange.Broadcast( InStory, InVariable, InNewValue);
}
