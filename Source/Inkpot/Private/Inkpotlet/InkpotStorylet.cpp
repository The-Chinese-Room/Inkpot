#include "Inkpotlet/InkpotStorylet.h"

FInkpotlet::FInkpotlet(FName InName, TSharedPtr<Ink::FContainer> InValidator)
: Name(InName)
, Validator(InValidator)
{
}

bool FInkpotlet::IsValid(UInkpotStory* Context) const
{
	if (!Context)
		return false;
	TSharedPtr<FInkpotStoryInternal> storyInternal = Context->GetStoryInternal();
	if (!storyInternal)
		return false;

	TSharedPtr<Ink::FObject> object = storyInternal->EvaluateExpression(Validator);
	TSharedPtr<Ink::FValueBool> value = Ink::FObject::DynamicCastTo<Ink::FValueBool>(object);
	if (!value.IsValid())
		return false;

	bool rval = value->GetValue();
	return rval;
}

const FName& FInkpotlet::GetName() const
{
	return Name;
}


