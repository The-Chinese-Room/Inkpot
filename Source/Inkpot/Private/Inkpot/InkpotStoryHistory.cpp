#include "Inkpot/InkpotStoryHistory.h"
#include "Utility/InkpotLog.h"

FInkpotStoryHistoryEvent::FInkpotStoryHistoryEvent()
: Type( EInkpotStoryEventType::None )
, Choice( -1 )
, Data( "" )
{
}

FInkpotStoryHistoryEvent::FInkpotStoryHistoryEvent(EInkpotStoryEventType InType, const FString &InData )
: Type( InType )
, Data( InData )
{
}

FInkpotStoryHistoryEvent::FInkpotStoryHistoryEvent(EInkpotStoryEventType InType, int InChoice )
: Type( InType )
, Choice( InChoice )
{
}

void UInkpotStoryHistory::Initialise( UInkpotStory* InStory )
{
	RandSeed = InStory->GetStorySeed();
	InStory->OnContinue().AddDynamic( this, &UInkpotStoryHistory::OnStoryContinue );
	InStory->OnMakeChoice().AddDynamic( this, &UInkpotStoryHistory::OnStoryChoice );
	InStory->OnSwitchFlow().AddDynamic( this, &UInkpotStoryHistory::OnStorySwitchFlow );
	InStory->OnChoosePath().AddDynamic( this, &UInkpotStoryHistory::OnStoryChoosePath );
}

void UInkpotStoryHistory::OnStoryContinue( UInkpotStory* InStory )
{
	if(bInReplay)
		return;
	Events.Push( FInkpotStoryHistoryEvent(EInkpotStoryEventType::Continue, -1 ) );
}

void UInkpotStoryHistory::OnStoryChoice( UInkpotStory* InStory, UInkpotChoice* InChoice )
{
	if(bInReplay)
		return;
	Events.Push( FInkpotStoryHistoryEvent( EInkpotStoryEventType::Choice, InChoice->GetIndex() ) );
}

void UInkpotStoryHistory::OnStorySwitchFlow( UInkpotStory* InStory, const FString& InFlow )
{
	if(bInReplay)
		return;
	Events.Push( FInkpotStoryHistoryEvent(EInkpotStoryEventType::SwitchFlow, InFlow ) );
}

void UInkpotStoryHistory::OnStoryChoosePath( UInkpotStory* InStory, const FString& InPath )
{
	if(bInReplay)
		return;
	Events.Push( FInkpotStoryHistoryEvent(EInkpotStoryEventType::ChoosePath, InPath ) );
}
	
void UInkpotStoryHistory::Replay( bool bInResetState )
{
	UInkpotStory *story = Cast<UInkpotStory>( GetOuter() );
	if(!story)
		return;

	bInReplay = true;

	if(bInResetState)
		story->ResetState();

	story->SetStorySeed( RandSeed );

	for( const FInkpotStoryHistoryEvent &event : Events )
	{
		switch( event.Type )
		{
		case EInkpotStoryEventType::Continue:
			story->Continue();
			break;
		case EInkpotStoryEventType::Choice:
			story->ChooseChoiceIndex( event.Choice );
			break;
		case EInkpotStoryEventType::SwitchFlow:
			story->SwitchFlow( event.Data );
			break;
		case EInkpotStoryEventType::ChoosePath:
			story->ChoosePath( event.Data );
			break;
		default:
			INKPOT_ERROR("History Event is invalid!!");
			break;
		}
	}

	bInReplay = false;
}

