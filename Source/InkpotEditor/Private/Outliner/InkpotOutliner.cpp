#include "Outliner/InkpotOutliner.h"
#include "Outliner/BlotterContainer.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"

bool UInkpotOutliner::Initialize()
{
	Super::Initialize();
	BindInkpotStoryBegin();
	return true;
}

void UInkpotOutliner::BindInkpotStoryBegin()
{
	INKPOT_BIND_TO_STORY_BEGIN(this, &UInkpotOutliner::OnStoryBegin);
}

void UInkpotOutliner::OnStoryBegin(UInkpotStory* InStory)
{
	UBlotterContainer *root = BuildContainers( InStory );
	ReceiveOnStoryDisplay( root );
}

UBlotterContainer *UInkpotOutliner::BuildContainers(UInkpotStory* InStory)
{
	TSharedPtr<FInkpotStoryInternal>  story = InStory->GetStoryInternal();
	UBlotterContainer *root = NewObject<UBlotterContainer>(this);
	BuildContainers( root, "", story->GetMainContentContainer(), 0 );
	return root;
}

void UInkpotOutliner::BuildContainers(UBlotterContainer *View, const FString &RootName, TSharedPtr<Ink::FContainer> Source, int32 InLevel )
{
	if(!Source)
		return;

	FString rootName = RootName;
	FString containerName = Source->GetName();
	if (rootName.IsEmpty())
		rootName = containerName;
	else if (containerName.Len() > 0)
		rootName += TEXT(".") + containerName;

	View->SetText( rootName );
	View->SetLevel( InLevel );

	TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> namedContentPtr = Source->GetNamedContent();
	for( auto pair : *namedContentPtr )
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>( pair.Value );
		if(!container)
			continue;
		UBlotterContainer* childView = NewObject<UBlotterContainer>(this);
		View->Add( childView );
		BuildContainers( childView, rootName, container, InLevel + 1 );
	}
}
