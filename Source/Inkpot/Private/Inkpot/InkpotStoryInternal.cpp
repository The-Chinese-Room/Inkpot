#include "Inkpot/InkpotStoryInternal.h"
#include "Ink/StoryState.h"

FInkpotStoryInternal::FInkpotStoryInternal( const FString &InJSON, int32 InStoryID )
: Ink::FStory( InJSON ),
  StoryID( InStoryID )  
{
}

int32 FInkpotStoryInternal::GetID() const
{
	return StoryID;
}

bool FInkpotStoryInternal::IsValidStory() const
{
	// during construction of the base class, the state is set only if all checks have passsed during contstruction
	// if the state is valid it means that the Ink VM is happy with the story
	return State.IsValid();
}

bool FInkpotStoryInternal::HasChoices() const
{
	TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> choices = State->CurrentChoices();
	if(!choices.IsValid())
		return false;
	if(choices->Num() == 0)
		return false;
	return true;
}
