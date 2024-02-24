#pragma once

#include "Coreminimal.h"
#include "Inkpot/InkpotStory.h"
#include "Inkpotlet/InkpotStoryletDeck.h"
#include "InkpotStoryletDeckSet.generated.h"

UCLASS(BlueprintType)
class INKPOT_API UInkpotletSet : public UInkpotStory
{
	GENERATED_BODY()

public:
	virtual void Initialise(TSharedPtr<FInkpotStoryInternal>  InkpotStory);
	void Dump();

	UFUNCTION()
	FInkpotValue ChoosePotlet(const TArray<FInkpotValue>& Values);

protected:
	virtual void ChoosePathInternal(const FString &Path) override;

private:
	void BindFunctions();
	void BuildPlayDeck(TArray<FInkpotlet*> &PlayDeck);

	void FindDecks();
	void ParseDeck(const FString& Knot);
	TSharedPtr<Ink::FContainer> GetVariableExpression(const FString& InPath, const FString& InVariable);

private:
	TArray<FInkpotletDeck> Decks;
};
