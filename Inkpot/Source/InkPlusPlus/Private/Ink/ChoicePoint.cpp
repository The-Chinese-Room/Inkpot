#include "Ink/ChoicePoint.h"
#include "Ink/Path.h"
#include "Ink/Container.h"
#include "Utility/InkPlusPlusLog.h"
#include "Ink/SearchResult.h"


Ink::FChoicePoint::FChoicePoint()
	:// Flags(0),
	bHasCondition( false ),
	bHasStartContent( false ),
	bHasChoiceOnlyContent( false ),
	bIsOnceOnly( true ),
	bIsInvisibleDefault( false )
{
}

Ink::FChoicePoint::FChoicePoint( bool InOnceOnly )
	: //Flags(0),
	bHasCondition( false ),
	bHasStartContent( false ),
	bHasChoiceOnlyContent(false),
	bIsOnceOnly(InOnceOnly),
	bIsInvisibleDefault(false)
{
}

/*
Ink::FChoicePoint::FChoicePoint(const FChoicePoint& InOtherChoicePoint)
{
}

Ink::FChoicePoint& Ink::FChoicePoint::operator=(const FChoicePoint& InOtherChoicePoint)
{
}

Ink::FChoicePoint::~FChoicePoint()
{
}
*/

FString Ink::FChoicePoint::ToString()
{
	const int32 targetLineNum = DebugLineNumberOfPath(GetPathOnChoice());
	FString targetString = GetPathOnChoice()->ToString();
	if ( targetLineNum > -1 )
	{
		targetString = FString::Printf( TEXT( " line %d(%s)" ), targetLineNum, *targetString );
	}

	return FString(TEXT("Choice: -> ") + targetString);
}

int32 Ink::FChoicePoint::GetFlags() const
{
	int32 flags = 0;
	if (bHasCondition)			flags |= 1;
	if (bHasStartContent)		flags |= 2;
	if (bHasChoiceOnlyContent)	flags |= 4;
	if (bIsInvisibleDefault)	flags |= 8;
	if (bIsOnceOnly)			flags |= 16;
	return flags;
}

void Ink::FChoicePoint::SetFlags(int32 InFlags)
{
	bHasCondition =			(InFlags & 1) > 0;
	bHasStartContent =		(InFlags & 2) > 0;
	bHasChoiceOnlyContent = (InFlags & 4) > 0;
	bIsInvisibleDefault =	(InFlags & 8) > 0;
	bIsOnceOnly =			(InFlags & 16) > 0;
}

TSharedPtr<Ink::FPath> Ink::FChoicePoint::GetPathOnChoice()
{
	// Resolve any relative paths to global ones as we come across them
	if (_PathOnChoice.IsValid() && _PathOnChoice->IsRelative())
	{
		auto choiceTarget = GetChoiceTarget();
		if (choiceTarget != nullptr)
		{
			_PathOnChoice = choiceTarget->GetPath();
		}
	}
	return _PathOnChoice;
}

void Ink::FChoicePoint::SetPathOnChoice(TSharedPtr<Ink::FPath> InPath)
{
	_PathOnChoice = InPath;
}

FString Ink::FChoicePoint::GetPathStringOnChoice()
{
	return CompactPathString(GetPathOnChoice());
}

void Ink::FChoicePoint::SetPathStringOnChoice(const FString& InPathStringOnChoice)
{
	SetPathOnChoice(MakeShared<Ink::FPath>(InPathStringOnChoice));
}

TSharedPtr<Ink::FContainer> Ink::FChoicePoint::GetChoiceTarget()
{
	return ResolvePath(_PathOnChoice).GetObjectAsContainer();
}

bool Ink::FChoicePoint::HasCondition() const
{
	return bHasCondition;
}

void Ink::FChoicePoint::SetHasCondition(bool InHasCondition)
{
	bHasCondition = InHasCondition;
}

bool Ink::FChoicePoint::HasStartContent() const
{
	return bHasStartContent;
}

void Ink::FChoicePoint::SetHasStartContent(bool InHasStartContent)
{
	bHasStartContent = InHasStartContent;
}

bool Ink::FChoicePoint::HasChoiceOnlyContent() const
{
	return bHasChoiceOnlyContent;
}

void Ink::FChoicePoint::SetHasChoiceOnlyContent(bool InHasChoiceOnlyContent)
{
	bHasChoiceOnlyContent = InHasChoiceOnlyContent;
}

bool Ink::FChoicePoint::IsOnceOnly() const
{
	return bIsOnceOnly;
}

void Ink::FChoicePoint::SetIsOnceOnly(bool InIsOnceOnly)
{
	bIsOnceOnly = InIsOnceOnly;
}

bool Ink::FChoicePoint::IsInvisibleDefault() const
{
	return bIsInvisibleDefault;
}

void Ink::FChoicePoint::SetIsInvisibleDefault(bool InIsInvisibleDefault)
{
	bIsInvisibleDefault = InIsInvisibleDefault;
}
