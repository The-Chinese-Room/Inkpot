#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotChoice.h"
#include "Inkpot/Inkpot.h"
#include "Ink/Choice.h"
#include "Ink/StoryState.h"
#include "Ink/SearchResult.h"
#include "Ink/Path.h"
#include "Utility/InkpotLog.h"

static const int32 dbgIndent{12};
#define INKPOT_DBG( section, msg, ... )   INKPOT_LOG( "%- *s: " msg, dbgIndent, TEXT(section), ##__VA_ARGS__ )

void UInkpotStory::Initialise( TSharedRef<FInkpotStoryInternal>  InInkpotStory )
{
	StoryInternal = InInkpotStory;
	StoryInternal->OnDidContinue().AddUObject(this, &UInkpotStory::OnContinueInternal );
	StoryInternal->OnMakeChoice().AddUObject(this, &UInkpotStory::OnMakeChoiceInternal );
	StoryInternal->OnEvaluateFunction().AddUObject(this, &UInkpotStory::OnEvaluateFunctionInternal );
	StoryInternal->OnCompleteEvaluateFunction().AddUObject(this, &UInkpotStory::OnCompleteEvaluateFunctionInternal );
	StoryInternal->OnChoosePathString().AddUObject(this, &UInkpotStory::OnChoosePathStringInternal );
}

TSharedPtr<FInkpotStoryInternal> UInkpotStory::GetStoryInternal()
{
	return StoryInternal;
}

int32 UInkpotStory::GetID() const
{
	if(!StoryInternal.IsValid())
		return FInkpotStoryInternal::BadStoryHandle;
	return StoryInternal->GetID();
}

bool UInkpotStory::IsValid() const
{
	if(!StoryInternal.IsValid())
		return false;
	return StoryInternal->IsValidStory();
}

FString UInkpotStory::GetCurrentText()
{
	return StoryInternal->GetCurrentText();
}

const TArray<FString>& UInkpotStory::GetCurrentTags()
{
	return StoryInternal->GetCurrentTags();
}

TArray<FString> UInkpotStory::GlobalTags()
{
	return StoryInternal->GlobalTags();
}

TArray<FString> UInkpotStory::TagsForContentAtPath( const FString &InPath)
{
	return StoryInternal->TagsForContentAtPath( InPath );
}

void UInkpotStory::ChoosePath( const FString &InPath )
{
	StoryInternal->ChoosePathString( InPath, true, TArray<TSharedPtr<Ink::FValueType>>() );
}

void UInkpotStory::ChoosePathString( const FString &InPath, const TArray<FInkpotValue> &InValues )
{
	TArray<TSharedPtr<Ink::FValueType>> values;
	values.Reserve( InValues.Num() );
	for( const FInkpotValue &inValue : InValues )
		values.Emplace( *inValue );
	StoryInternal->ChoosePathString( InPath, true, values );
}

FString UInkpotStory::Continue()
{
	return StoryInternal->Continue();
}

FString UInkpotStory::ContinueMaximally()
{
	return StoryInternal->ContinueMaximally();
}

bool UInkpotStory::CanContinue()
{
	return StoryInternal->CanContinue();
}

bool UInkpotStory::HasChoices()
{
	return StoryInternal->HasChoices();
}

const TArray<UInkpotChoice*>& UInkpotStory::GetCurrentChoices()
{
	return Choices;
}

void UInkpotStory::UpdateChoices()
{
	Choices.Empty();

	if(!StoryInternal->HasChoices())
		return;

	TArray<TSharedPtr<Ink::FChoice>> inkChoices =  StoryInternal->GetCurrentChoices();
	for( auto inkChoice : inkChoices  )
	{
		UInkpotChoice* choice = NewObject<UInkpotChoice>( this );
		choice->Initialise( inkChoice->GetIndex(), inkChoice->GetText() );
		Choices.Emplace( choice );
	}
}

void UInkpotStory::ChooseChoice( UInkpotChoice* InChoice )
{
	if(!InChoice)
		return;
	ChooseChoiceIndex( InChoice->GetIndex() );
}

void UInkpotStory::ChooseChoiceIndex( int32 InChoiceIndex )
{
	StoryInternal->ChooseChoiceIndex( InChoiceIndex );
}

bool UInkpotStory::SwitchFlow( const FString &InFlowName )
{
	bool isFlowAlive = IsFlowAlive( InFlowName );
	StoryInternal->SwitchFlow( InFlowName );
	UpdateChoices();
	if(EventOnSwitchFlow.IsBound())
		EventOnSwitchFlow.Broadcast( this, StoryInternal->GetCurrentFlowName() );
	return !isFlowAlive; 
}

void UInkpotStory::SwitchToDefautFlow()
{
	StoryInternal->SwitchToDefaultFlow();
	UpdateChoices();
	if(EventOnSwitchFlow.IsBound())
		EventOnSwitchFlow.Broadcast( this, StoryInternal->GetCurrentFlowName() );
}

bool UInkpotStory::IsFlowAlive( const FString &InFlowName )
{
	TSharedPtr<Ink::FStoryState> state = StoryInternal->GetStoryState();
	TSharedPtr<TMap<FString, TSharedPtr<Ink::FFlow>>> namedFlows = state->GetNamedFlows();
	if( namedFlows.IsValid() )
		return namedFlows->Contains( InFlowName );
	return false;
}

TArray<FString> UInkpotStory::GetAliveFlowNames()
{
	TSharedPtr<Ink::FStoryState> state = StoryInternal->GetStoryState();
	TSharedPtr<TMap<FString, TSharedPtr<Ink::FFlow>>> namedFlows = state->GetNamedFlows();

	TArray<FString> flowNames;
	if( namedFlows.IsValid() )
		namedFlows->GetKeys( flowNames );
	return flowNames;
}

int32 UInkpotStory::GetAliveFlowCount()
{
	TSharedPtr<Ink::FStoryState> state = StoryInternal->GetStoryState();
	TSharedPtr<TMap<FString, TSharedPtr<Ink::FFlow>>> namedFlows = state->GetNamedFlows();

	int32 count = 0;
	if( namedFlows.IsValid() )
		count = namedFlows->Num();
	return count; 
}

void UInkpotStory::SetValue(const FString &InVariable, FInkpotValue InValue)
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, Ink::FValue::Create(**InValue) );
}

FInkpotValue UInkpotStory::GetValue(const FString &InVariable)
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	TSharedPtr<Ink::FObject> variableObj = variableState->GetVariable( InVariable );
	TSharedPtr<Ink::FValue> variable = StaticCastSharedPtr<Ink::FValue>( variableObj );
	Ink::FValueType value = variable->GetValueObject();
	return FInkpotValue(value);
}

void UInkpotStory::SetBool(const FString &InVariable, bool bInValue)
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, MakeShared<Ink::FValueBool>( bInValue ) );
}

bool UInkpotStory::GetBool(const FString &InVariable)
{
	bool rval;
	bool success = GetVariable<bool, Ink::FValueBool>(InVariable, Ink::EValueType::Bool, rval );
	return rval;
}

void UInkpotStory::SetInt(const FString &InVariable, int32 InValue)
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, MakeShared<Ink::FValueInt>( InValue ) );
}

int32 UInkpotStory::GetInt(const FString &InVariable)
{
	int32 rval;
	bool success = GetVariable<int32, Ink::FValueInt>(InVariable, Ink::EValueType::Int, rval );
	return rval;
}

void UInkpotStory::SetFloat(const FString &InVariable, float InValue )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, MakeShared<Ink::FValueFloat>(InValue));
}

float UInkpotStory::GetFloat(const FString &InVariable)
{
	float rval;
	bool success = GetVariable<float, Ink::FValueFloat>(InVariable, Ink::EValueType::Float, rval );
	return rval;
}

void UInkpotStory::SetString(const FString &InVariable, const FString &InValue )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, MakeShared<Ink::FValueString>(InValue) );
}

FString UInkpotStory::GetString( const FString& InVariable )
{
	FString rval;
	bool success = GetVariable<FString, Ink::FValueString>(InVariable, Ink::EValueType::String, rval );
	return rval;
}

void UInkpotStory::SetEmpty(const FString &InVariable)
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	variableState->SetVariable( InVariable, MakeShared<Ink::FValue>() );
}

int32 UInkpotStory::GetVariableKeys( TArray<FString> &OutKeys )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	const TMap<FString, TSharedPtr<Ink::FObject>>& variables = variableState->GetVariables();
	int32 rval = variables.GetKeys( OutKeys );
	return rval;
}

void UInkpotStory::ObserveVariable( const FString& InVariable, TSharedPtr<FStoryVariableObserver> InObserver )
{
	StoryInternal->ObserveVariable( InVariable, InObserver );
}

void UInkpotStory::SetOnVariableChange( FOnVariableChange InDelegate, const FString &InVariable )
{
	TSharedPtr<FStoryVariableObserver> observer = MakeShared<FStoryVariableObserver>();
	observer->BindLambda
	(
		[this, InDelegate](const FString& InVar, Ink::FValueType InType)
		{
	        InDelegate.Execute( this, InVar, FInkpotValue(InType) );
		}
	);
	VariableObservers.Emplace( InVariable, observer );
	StoryInternal->ObserveVariable( InVariable, observer );
}

void UInkpotStory::ClearVariableChange( const FString &InVariable )
{
	if(!VariableObservers.Contains(InVariable))
		return;
	TSharedPtr<FStoryVariableObserver> observer = VariableObservers[InVariable];
	StoryInternal->RemoveVariableObserver( observer, InVariable );
	VariableObservers.Remove( InVariable );
}

void UInkpotStory::OnContinueInternal()
{
	UpdateChoices();
	DumpDebug();
	if(!EventOnContinue.IsBound())
		return;
	EventOnContinue.Broadcast( this );
}

void UInkpotStory::OnMakeChoiceInternal(TSharedPtr<Ink::FChoice> InChoice)
{
	if(!EventOnContinue.IsBound())
		return;
	int32 key = InChoice->GetIndex();
	UInkpotChoice ** choicePtr = Choices.FindByPredicate( [key]( UInkpotChoice *pChoice ) { return *pChoice == key; } );
	if(!choicePtr)
		return;
	DumpDebug( *choicePtr );
	EventOnMakeChoice.Broadcast( this, *choicePtr );
}

void UInkpotStory::OnEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms)
{

}

void UInkpotStory::OnCompleteEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms, const FString& OutParmName, TSharedPtr<Ink::FValueType> OutParmType)
{

}

void UInkpotStory::OnChoosePathStringInternal(const FString& InPath, const TArray<TSharedPtr<Ink::FValueType>>& InPathType )
{
	UpdateChoices();
	if(!EventOnChoosePath.IsBound())
		return;
	EventOnChoosePath.Broadcast( this, InPath ); 
}

void UInkpotStory::DumpDebug()
{
	FString currentFlow = StoryInternal->GetCurrentFlowName();
	INKPOT_DBG("Flow", "%s", *currentFlow);

	if( GetAliveFlowCount() > 0 )
	{
		TArray<FString> flows = GetAliveFlowNames();
		if ( flows.Num() > 0 )
		{
			FString flowsAlive;
			for ( auto flow : flows )
			{
				flowsAlive.Append( flow );
				flowsAlive.Append( ", " );
			}
			INKPOT_DBG( "Flows alive", "%s", *flowsAlive );
		}
	}

	FString currentText = GetCurrentText();
	INKPOT_DBG("Text", "%s", *currentText);

	const TArray<FString>& tags = GetCurrentTags();
	if( tags.Num() > 0 )
	{
		FString tagsSet;
		for( auto tag : tags )
		{
			tagsSet.Append( "'#" );
			tagsSet.Append( tag );
			tagsSet.Append( "' " );
		}
		INKPOT_DBG( "CTags", "%s", *tagsSet );
	}

	TArray<FString> gtags = GlobalTags();
	if( gtags.Num() > 0 )
	{
		FString tagsSet;
		for( auto tag : gtags )
		{
			tagsSet.Append( "'#" );
			tagsSet.Append( tag );
			tagsSet.Append( "' " );
		}
		INKPOT_DBG( "GTags", "%s", *tagsSet );
	}

	TArray<FString> keys;
	if(int num = GetVariableKeys( keys ))
	{
		for( int32 i=0; i<num; ++i )
		{
			const FString& key = keys[i];
			TSharedPtr<Ink::FObject> obj = GetVariable( key );
			const FString& value = obj->ToString();
			if(i==0)
			{
				INKPOT_DBG( "Variables", "%s = %s", *key, *value );
			}
			else
			{
				INKPOT_DBG( " ", "%s = %s", *key, *value );
			}
		}
	}

	if(Choices.Num() > 0)
	{
		INKPOT_DBG( "Choice", "%d - %s", Choices[0]->GetIndex(), *Choices[0]->GetString() );
		for( int32 i=1; i<Choices.Num(); ++i )
		{
			INKPOT_DBG( " ", "%d - %s", Choices[i]->GetIndex(), *Choices[i]->GetString() );
		}
	}
}

void UInkpotStory::DumpDebug(UInkpotChoice *InChoice)
{
	if(!InChoice)
		return;
	INKPOT_DBG( "Chose", "%d - %s", InChoice->GetIndex(), *InChoice->GetString() );
}


void UInkpotStory::DumpContainer(const FString &InName, TSharedPtr<Ink::FContainer> InContainer, int Indent)
{
	if(!InContainer)
		return;

	FString pad; 
	for(int i=0;i<Indent;++i)
		pad += '\t';

	INKPOT_LOG("%s** container(%s) - Begin", *pad,*InName );
	pad += '\t';

	TArray<TSharedPtr<Ink::FObject>> *contents = InContainer->GetContent().Get();
	for( TSharedPtr<Ink::FObject> obj : *contents)
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>(obj);
		if( container )
		{
			DumpContainer( "inline", container, Indent + 1 );
		}
		else
		{
			FString entry = obj->ToString();
			entry.TrimEndInline();
			if ( entry.Len() )
				INKPOT_LOG( "%s%s", *pad, *entry );
		}
	}

	TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> namedContentPtr = InContainer->GetNamedContent();
	for( auto pair : *namedContentPtr )
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>( pair.Value );
		if(!container)
			continue;
		DumpContainer( pair.Key, container, Indent + 1 );
	}

	pad.LeftChopInline(1);
	INKPOT_LOG("%s** container(%s) - End", *pad, *InName );
}


void UInkpotStory::DumpMainContent()
{
	TSharedPtr<Ink::FContainer> main = StoryInternal->GetMainContentContainer();
	DumpContainer( "<root>", main );
}

void UInkpotStory::DumpContentAtPath( const FString& InName )
{
	TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>( InName );
	Ink::FSearchResult result = StoryInternal->ContentAtPath(path);
	DumpContainer(InName, result.GetObjectAsContainer() );
}

void UInkpotStory::DumpContentAtKnot( const FString& InName )
{
	TSharedPtr<Ink::FContainer> knotContainer = StoryInternal->KnotContainerWithName(InName);
	if(!knotContainer)
	{
		INKPOT_ERROR("%s is not a valid knot in this story", *InName );
		return;
	}
	DumpContainer(InName, knotContainer  );
}

