#include "Inkpotlet/InkpotStoryletDeckSet.h"
#include "Utility/InkpotUtility.h"
#include "Utility/InkPlusPlusUtility.h"
#include "Utility/InkpotLog.h"
#include "Inkpot/InkpotStory.h"
#include "Ink/ControlCommand.h"
#include "Ink/SearchResult.h"
#include "Inkpot/InkpotValue.h"


#define INKPOTLET_SET	TEXT("InkpotletSet")
#define INKPOTLET_DECK	TEXT("InkpotDeck")
#define INKPOTLET		TEXT("Inkpotlet")

#define INK_GLOBAL_DECL	TEXT("global decl")

void UInkpotletSet::Initialise(TSharedPtr<FInkpotStoryInternal> InInkpotStory)
{
	Super::Initialise(InInkpotStory);
	FindDecks();
	Dump();
	BindFunctions();
}

void UInkpotletSet::FindDecks()
{
	TArray<FString> knots = GetNamedContent();
	for (const FString& knot : knots)
	{
		if (knot.Equals(INK_GLOBAL_DECL))
			continue;
		ParseDeck(knot);
	}
}

void UInkpotletSet::ParseDeck(const FString& InKnotName)
{
	TSharedPtr<Ink::FContainer> exprContainer = GetVariableExpression( InKnotName, INKPOTLET_DECK);
	if (!exprContainer)
		return;

	//DumpContainer(INKPOTLET_DECK, exprContainer);

	FInkpotletDeck deck(*InKnotName, exprContainer);
	TArray<FString> stitches = GetNamedContentForKnot(InKnotName);
	for (const FString& stitch : stitches)
	{
		FString path = FString::Printf(TEXT("%s.%s"), *InKnotName, *stitch);
		exprContainer = GetVariableExpression(path, INKPOTLET);
		if (!exprContainer)
			continue;

		FInkpotlet potlet(*path, exprContainer);
		deck.Content.Add( potlet );
	}

	Decks.Add(deck);
}

// evaluates the operands int the container to find the expression block defining the variable
TSharedPtr<Ink::FContainer> UInkpotletSet::GetVariableExpression(const FString& InPath, const FString& InVariable)
{
	TSharedPtr<FInkpotStoryInternal> storyInternal = GetStoryInternal();

	TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>(InPath);
	Ink::FSearchResult result = StoryInternal->ContentAtPath(path);
	TSharedPtr<Ink::FContainer> container = result.GetObjectAsContainer();
	if (!container)
		return nullptr;

	DumpContainer(InPath, container);

	TSharedPtr<TArray<TSharedPtr<Ink::FObject>>> contentsPtr = container->GetContent();
	if (!contentsPtr)
		return nullptr;

	TArray<TSharedPtr<Ink::FObject>>& contents = *contentsPtr.Get();

	// Find the variable assignment command location in the operation list
	int varLocation = -1;
	for (int i = 0; i < contents.Num(); ++i)
	{
		TSharedPtr<Ink::FObject> obj = contents[i];
		if (!obj->CanCastTo(Ink::EInkObjectClass::FVariableAssignment))
			continue;

		TSharedPtr<Ink::FVariableAssignment> assignment = Ink::FObject::DynamicCastTo<Ink::FVariableAssignment>(obj);
		if (!assignment)
			continue;

		FString varName = assignment->GetVariableName();
		if (!InVariable.Equals(varName))
			continue;

		varLocation = i;
		break;
	}

	// if the variable was not found, return nothing 
	if (varLocation == -1)
		return nullptr;

	// find the beginning of the evaulation block for the variable assignment above 
	int evalBlock = 0;
	for (int i = varLocation; i >= 0; --i)
	{
		TSharedPtr<Ink::FObject> obj = contents[i];
		if (!obj->CanCastTo(Ink::EInkObjectClass::FControlCommand))
			continue;

		TSharedPtr<Ink::FControlCommand> command = Ink::FObject::DynamicCastTo<Ink::FControlCommand>(obj);
		if (!command)
			continue;

		switch (command->GetCommandType())
		{
		case Ink::ECommandType::EvalStart:
			if (--evalBlock)
				continue;
			else
				break;
		case Ink::ECommandType::EvalEnd:
			evalBlock++;
			continue;
		}
		evalBlock = i;
		break;
	}

	// copy out the evaluation block into a new container
	TSharedPtr<Ink::FContainer> exprContainer = MakeShared<Ink::FContainer>();
	TArray<TSharedPtr<Ink::FObject>> expr;
	for (int i = evalBlock; i < varLocation; ++i)
	{
		TSharedPtr<Ink::FObject> obj = contents[i];
		TSharedPtr<Ink::FObject> newObj = CopyInkObject(obj);
		newObj->SetParent(nullptr);
		expr.Add(newObj);
	}
	exprContainer->SetContent(expr);

	return exprContainer;
}

void UInkpotletSet::Dump()
{
	INKPOT_LOG("## --- Dumping inkpot decks");
	INKPOT_LOG("# state ");
	DumpDebug();

	INKPOT_LOG("# decks ");
	for (const FInkpotletDeck& deck : Decks )
	{
		FString deckname = deck.GetName().ToString();
		bool bDeckValid = deck.IsValid( this );
		INKPOT_LOG("Deck %s - %s", *deckname, bDeckValid ? TEXT("Valid") : TEXT("Invalid") );
		for (const FInkpotlet& card : deck.Content)
		{
			FString cardname = card.GetName().ToString();
			bool bCardValid = card.IsValid(this);

			INKPOT_LOG("\tCard %s - %s", *cardname, bCardValid ? TEXT("Valid") : TEXT("Invalid"));
		}
	}
	INKPOT_LOG("## --- Dumping inkpot decks - done");
}

void UInkpotletSet::BindFunctions()
{
	FInkpotExternalFunction func;
	func.BindDynamic(this, &UInkpotletSet::ChoosePotlet);
//	BindExternalFunction( TEXT("ChoosePotlet"), func, false);
}

FInkpotValue UInkpotletSet::ChoosePotlet(const TArray<FInkpotValue>& Values)
{
	//if (CanContinue())
	//	Continue();

	Dump();

	TArray<FInkpotlet*> playdeck;
	BuildPlayDeck(playdeck);
	if ( playdeck.Num() > 0 )
	{
		int choice = FMath::RandHelper(playdeck.Num());

		FString name = playdeck[choice]->GetName().ToString();
		ChoosePath(name);
//		if (CanContinue())
//			Continue();
	}
	return FInkpotValue();
}

void UInkpotletSet::BuildPlayDeck(TArray<FInkpotlet*>& PlayDeck)
{
	for (FInkpotletDeck& deck : Decks)
	{
		if (!deck.IsValid(this))
			continue;
		for (FInkpotlet& card : deck.Content)
		{
			if (!card.IsValid(this))
				continue;

			int count = 1;
			for(int i=0 ; i<count ;++i)
				PlayDeck.Add( &card );
		}
	}
}

void UInkpotletSet::ChoosePathInternal( const FString &iInString )
{
	Dump();

	TArray<FInkpotlet*> playdeck;
	BuildPlayDeck(playdeck);
	if (playdeck.Num() > 0)
	{
		int choice = FMath::RandHelper(playdeck.Num());

		FString name = playdeck[choice]->GetName().ToString();
		Super::ChoosePathInternal( name );
	}
}
