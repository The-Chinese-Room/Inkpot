#pragma once

#include "Coreminimal.h"
#include "Inkpot/InkpotStory.h"

struct FInkpotlet
{
public:
	FInkpotlet(FName Name, TSharedPtr<Ink::FContainer> Validator);
	bool IsValid(UInkpotStory* StoryContext) const;
	const FName& GetName() const;

protected:
	FName Name;
	TSharedPtr<Ink::FContainer> Validator;
};


