#pragma once

#include "Coreminimal.h"
#include "Ink/Story.h"

class INKPOT_API FInkpotStoryInternal : public Ink::FStory
{
public:
	FInkpotStoryInternal( const FString &InJSON, int32 InStoryID );

	int32 GetID() const;
	bool IsValidStory() const;

	bool HasChoices() const;

public:
	static const int32 BadStoryHandle {-1};

protected:
	int32	StoryID;
};
