#include "Blotter/InkpotBlotter.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Blotter/BlotterString.h"
#include "Inkpot/InkpotValueLibrary.h"
#include "Inkpot/InkpotListLibrary.h"

bool UInkpotBlotter::Initialize()
{
	Super::Initialize();
	BindInkpotStoryBegin();
	FWorldDelegates::OnPIEEnded.AddUObject(this, &UInkpotBlotter::OnPIEEnd );
	return true;
}

void UInkpotBlotter::OnPIEEnd(UGameInstance* InGameInstance)
{
	ReceiveOnDebugEnd();
}

void UInkpotBlotter::BindInkpotStoryBegin()
{
	UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	inkpot->EventOnStoryBegin.AddDynamic(this, &UInkpotBlotter::OnStoryBegin);
}

void UInkpotBlotter::OnStoryBegin(UInkpotStory *InStory)
{
	InStory->OnDebugRefresh().AddDynamic(this, &UInkpotBlotter::OnDebugRefresh);
	ReceiveOnDebugRefresh(InStory);
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
			bv->SetValue( Story, key );
			bv->SetIndex(i);
			variables.Add(  bv );
		}
	}
	else
	{
		UBlotterVariable* bv = NewObject<UBlotterVariable>(this);
		bv->SetEmpty();
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

TArray<UBlotterVariable*> UInkpotBlotter::GetOrigins(UInkpotStory* Story)
{
	TArray<UBlotterVariable*> origins;

	int32 index = 0;

	TSharedPtr<Ink::FListDefinitionsOrigin> definitions = Story->GetStoryInternal()->GetListDefinitions();
	if (definitions.IsValid())
	{
		TArray<TSharedPtr<Ink::FListDefinition>> lists = definitions->GetLists();
		for (auto &list : lists)
		{
			FString sItems;
			const TMap<FString, int32>& items = list->GetItemNameToValues();
			bool first = true;
			for (auto& item : items)
			{
				if (!first)
					sItems.Append(TEXT(", "));
				sItems.Append( item.Key );
				first = false;
			}

			UBlotterVariable* bv = NewObject<UBlotterVariable>(this);

			bv->SetName( list->GetName() );
			bv->SetDisplayValue( sItems );
			bv->SetType(EBlotterVariableType::ListOrigin);
			bv->SetIndex(index++);

			origins.Add(bv);
		}
	}

	if(index == 0)
	{
		UBlotterVariable* bv = NewObject<UBlotterVariable>(this);
		bv->SetEmpty();
		origins.Add(bv);
	}

	return origins;
}



