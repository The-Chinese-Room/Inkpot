#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStories.h"
#include "Utility/InkpotLog.h"
#include "Settings/InkpotCVars.h"
#include "Asset/InkpotStoryAsset.h"

static FDelayedAutoRegisterHelper DelayedAutoRegister(
EDelayedRegisterRunPhase::EndOfEngineInit, 
	[]{
	    if ( UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>() )
		    inkpot->Register();
	}
);

UInkpot::UInkpot()
: Super()
{
	Stories = CreateDefaultSubobject<UInkpotStories>(TEXT("InkpotStories"));
}

void UInkpot::Initialize(FSubsystemCollectionBase &InCollection )
{ 
	Super::Initialize( InCollection );
	Stories->Initialise();
}

void UInkpot::InitializeStoryFactory()
{
	Stories->InitialiseStoryFactory();
}

UInkpotStory* UInkpot::BeginStory( UInkpotStoryAsset* InStory )
{
	UInkpotStory* story = Stories->BeginStory( InStory );
	if(EventOnStoryBegin.IsBound())
		EventOnStoryBegin.Broadcast( story );
	return story;
}

void UInkpot::EndStory(UInkpotStory *InStory)
{
	if(EventOnStoryEnd.IsBound())
		EventOnStoryEnd.Broadcast( InStory );
	Stories->EndStory( InStory );
}

void UInkpot::Register()
{
	BindPostImport();
	FWorldDelegates::OnStartGameInstance.AddUObject( this, &UInkpot::OnStartGameInstance );
}

void UInkpot::BindPostImport()
{
#if WITH_EDITOR
	if (IsValid(GEditor))
	{
		UImportSubsystem* imports = GEditor->GetEditorSubsystem<UImportSubsystem>();
		if (!imports)
			return;
		imports->OnAssetPostImport.AddUObject(this, &UInkpot::OnAssetPostImport);
	}
#endif 
}

#if WITH_EDITOR
void UInkpot::OnAssetPostImport(UFactory* InFactory, UObject* InObject)
{
	if (!GameInstance.IsValid())
		return;
	if (!GameInstance->GetWorld())
		return;
	if (GameInstance->GetWorld()->WorldType != EWorldType::PIE)
		return;

	if(!FInkpotCVars::bReloadIfAssetChanged)
		return;
	UInkpotStoryAsset *newAsset = Cast<UInkpotStoryAsset>(InObject);
	if(!newAsset)
		return;
	UInkpotStory* story = Stories->Reload( newAsset );
	if(!story)
		return;

	if (EventOnStoryBegin.IsBound())
		EventOnStoryBegin.Broadcast(story);
	if (FInkpotCVars::bReplayIfReloaded)
		Stories->Replay(story, false);
}
#endif 

void UInkpot::OnStartGameInstance( UGameInstance *InInstance )
{
	Stories->Reset();
	GameInstance = InInstance;
}

UInkpotStory* UInkpot::GetStory( TSoftObjectPtr<UInkpotStoryAsset> InkpotStoryAssetPath)
{
	return Stories->GetStory(InkpotStoryAssetPath);
}

UInkpotStoryAsset* UInkpot::GetStoryAsset(UInkpotStory* InkpotStory)
{
	return Stories->GetStoryAsset(InkpotStory);
}

TArray<UInkpotStory*> UInkpot::GetStories()
{
	return Stories->GetStories();
}
