#include "Blotter/InkpotBlotter.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"

bool UInkpotBlotter::Initialize()
{
	Super::Initialize();
	BindInkpotStoryBegin();
	return true;
}

void UInkpotBlotter::BindInkpotStoryBegin()
{
	UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	inkpot->EventOnStoryBegin.AddDynamic(this, &UInkpotBlotter::OnStoryBegin);
}

void UInkpotBlotter::OnStoryBegin(UInkpotStory *Story)
{
	Story->OnDebugRefresh().AddDynamic(this, &UInkpotBlotter::OnDebugRefresh);
}

void UInkpotBlotter::OnDebugRefresh(UInkpotStory* InStory)
{
	ReceiveOnDebugRefresh(InStory);
}

TArray<UBlotterVariable*> UInkpotBlotter::GetVariables(UInkpotStory* Story)
{
	TArray<UBlotterVariable*> variables;
	TArray<FString> keys;
	if (int num = Story->GetVariableKeys(keys))
	{
		variables.Reserve(num);
		for (int32 i = 0; i < num; ++i)
		{
			UBlotterVariable* bv = NewObject<UBlotterVariable>(this);

			const FString& key = keys[i];
			TSharedPtr<Ink::FObject> obj = Story->GetVariable(key);
			const FString& value = obj->ToString();

			bv->SetName(key);
			bv->SetValue(value);
			bv->SetType(obj);

			variables.Add(  bv );
		}
	}
	else
	{
		UBlotterVariable* bv = NewObject<UBlotterVariable>(this);

		bv->SetName(TEXT("[EMPTY]"));
		bv->SetValue(TEXT(""));
		bv->SetType(nullptr);

		variables.Add(bv);
	}
	return variables; 
}

TArray<UBlotterString*> UInkpotBlotter::MakeDisplayStrings(const TArray<FString>& InStrings)
{
	TArray<UBlotterString*> displayStrings;
	if(!InStrings.IsEmpty())
	{
		for (auto& string : InStrings)
		{
			UBlotterString* displayString = NewObject<UBlotterString>(this);
			displayString->Set(string);
			displayStrings.Add(displayString);
		}
	}
	else
	{
		UBlotterString* displayString = NewObject<UBlotterString>(this);
		displayString->Set(TEXT("[EMPTY]"));
		displayStrings.Add(displayString);
	}
	return displayStrings;

}

TArray<UBlotterString*> UInkpotBlotter::GetAliveFlowNames(UInkpotStory* InStory)
{
	TArray<FString> flowNames;
	if(IsValid(InStory))
		flowNames = InStory->GetAliveFlowNames();
	return MakeDisplayStrings(flowNames);
}

const TArray<UBlotterString*> UInkpotBlotter::GetCurrentTags(UInkpotStory* InStory)
{
	TArray<FString> ctags;
	if (IsValid(InStory))
		ctags = InStory->GetCurrentTags();
	return MakeDisplayStrings(ctags);
}

TArray<UBlotterString*> UInkpotBlotter::GlobalTags(UInkpotStory* InStory)
{
	TArray<FString> gtags;
	if (IsValid(InStory))
		gtags = InStory->GlobalTags();
	return MakeDisplayStrings(gtags);
}

TArray<UBlotterString*> UInkpotBlotter::GetChoices(UInkpotStory* InStory)
{
	TArray<FString> choices;
	if (IsValid(InStory))
	{
		TArray<TSharedPtr<Ink::FChoice>> inkchoices = InStory->GetStoryInternal()->GetCurrentChoices();
		for (auto& inkchoice : inkchoices)
			choices.Add(inkchoice->GetText());
	}
	return MakeDisplayStrings(choices);
}
