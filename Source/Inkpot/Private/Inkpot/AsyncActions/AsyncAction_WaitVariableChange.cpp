#include "Inkpot/AsyncActions/AsyncAction_WaitVariableChange.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"

UAsyncAction_WaitVariableChange* UAsyncAction_WaitVariableChange::ListenForVariableChange(UObject* WorldContextObject, const FString& VariableToListen, const bool bTriggerOnce)
{
	UAsyncAction_WaitVariableChange* AsyncTask = NewObject<UAsyncAction_WaitVariableChange>();
	AsyncTask->VariableToListen = VariableToListen;
	AsyncTask->bTriggerOnce = bTriggerOnce;
	AsyncTask->World = WorldContextObject->GetWorld();
	AsyncTask->RegisterWithGameInstance(WorldContextObject);
	return AsyncTask;
}

void UAsyncAction_WaitVariableChange::Activate()
{
	if (!GEngine || !World || VariableToListen.IsEmpty())
	{
		EndTask();
		return;
	}

	Inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	check(Inkpot);
	Inkpot->EventOnStoryBegin.AddUniqueDynamic(this, &UAsyncAction_WaitVariableChange::OnBeginStory);
}

void UAsyncAction_WaitVariableChange::OnBeginStory(UInkpotStory* InStory)
{
	CurrentStory = InStory;
	
	check(Inkpot);
	Inkpot->EventOnStoryEnd.AddUniqueDynamic(this, &UAsyncAction_WaitVariableChange::OnEndStory);
	
	FOnVariableChange Delegate;
	Delegate.BindDynamic(this, &UAsyncAction_WaitVariableChange::OnVariableChange);
	InStory->SetOnVariableChange(Delegate, VariableToListen);
}

void UAsyncAction_WaitVariableChange::OnVariableChange(UInkpotStory* InStory, const FString& InVariable, const FInkpotValue& NewValue)
{
	OnVariableChangeDelegate.Broadcast(InStory, InVariable, NewValue);

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UAsyncAction_WaitVariableChange::OnEndStory(UInkpotStory* InStory)
{
	check(Inkpot);
	Inkpot->EventOnStoryEnd.RemoveAll(this);
	InStory->ClearVariableChange(this, VariableToListen);
	CurrentStory = nullptr;
}

void UAsyncAction_WaitVariableChange::EndTask()
{
	check(Inkpot);
	Inkpot->EventOnStoryBegin.RemoveAll(this);
	Inkpot->EventOnStoryEnd.RemoveAll(this);
	
	if (CurrentStory && World)
	{
		// Story still in progress but the user wants to force End this Task
		// If we Clear Delegate Variable in the same tick while broadcasting all Delegates
		// It will trigger ensure "Array has changed during ranged-for iteration!"
		// We don't want to do that. So let's do clean up next tick, just to be safe
		World->GetTimerManager().SetTimerForNextTick([this]
		{
			CurrentStory->ClearVariableChange(this, VariableToListen);
			CurrentStory = nullptr;
		});
	}
	
	SetReadyToDestroy();
}
