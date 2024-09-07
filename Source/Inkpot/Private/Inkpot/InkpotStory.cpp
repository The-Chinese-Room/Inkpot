#include "Inkpot/InkpotStory.h"
#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotValue.h"
#include "Inkpot/Inkpot.h"
#include "Ink/Choice.h"
#include "Ink/StoryState.h"
#include "Ink/SearchResult.h"
#include "Ink/Path.h"
#include "Ink/Inklist.h"
#include "Utility/InkpotLog.h"


void UInkpotStory::Initialise( TSharedPtr<FInkpotStoryInternal>  InInkpotStory )
{
	StoryInternal = InInkpotStory;
	StoryInternal->OnDidContinue().AddUObject(this, &UInkpotStory::OnContinueInternal );
	StoryInternal->OnMakeChoice().AddUObject(this, &UInkpotStory::OnMakeChoiceInternal );
	StoryInternal->OnEvaluateFunction().AddUObject(this, &UInkpotStory::OnEvaluateFunctionInternal );
	StoryInternal->OnCompleteEvaluateFunction().AddUObject(this, &UInkpotStory::OnCompleteEvaluateFunctionInternal );
	StoryInternal->OnChoosePathString().AddUObject(this, &UInkpotStory::OnChoosePathStringInternal );
	BindOnVariableStateChangeEvent();
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
	FString currentText = StoryInternal->GetCurrentText();
	currentText.TrimEndInline();
	return currentText;
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
	ChoosePathInternal(InPath);
}

void UInkpotStory::ChoosePathInternal( const FString &InPath )
{
	StoryInternal->ChoosePathString( InPath, true, TArray<TSharedPtr<Ink::FValueType>>() );
}

void UInkpotStory::ChoosePathString( const FString &InPath, const TArray<FInkpotValue> &InValues )
{
	ChoosePathStringInternal( InPath, InValues );
}

TSharedPtr<Ink::FListDefinition> UInkpotStory::GetListOrigin(const FString& InOriginName, const FString& InItemName)
{
	TSharedPtr<Ink::FListDefinitionsOrigin> definitions = StoryInternal->GetListDefinitions();
	TSharedPtr<Ink::FListDefinition> origin;
	bool gotDefintion = definitions->TryListGetDefinition(InOriginName, origin);
	if (!gotDefintion)
	{
		INKPOT_ERROR("Failed to find List definition '%s'", *InOriginName);
		return nullptr;
	}

	bool gotItem = origin->ContainsItemWithName(InItemName);
	if (!gotItem)
	{
		INKPOT_ERROR("Failed to find entry '%s' in List definition '%s'", *InItemName, *InOriginName);
		return nullptr;
	}
	return origin;
}

bool UInkpotStory::ValidateInkListOrigin( const FInkpotValue &InValue )
{
	// only interested if the value is actually a list, return true for other types 
	if (!(*InValue)->HasSubtype<Ink::FInkList>())
		return true;

	Ink::FInkList &list = (*InValue)->GetSubtype<Ink::FInkList>();
	TSharedPtr<TArray<TSharedPtr<Ink::FListDefinition>> >&origins = list.GetOrigins();
	if( !origins.IsValid() )
		origins = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>();

	TSharedPtr<Ink::FListDefinitionsOrigin> definitions = StoryInternal->GetListDefinitions();

	TArray<Ink::FInkListItem> listItems;
	list.GetKeys(listItems);
	for (Ink::FInkListItem& item : listItems)
	{
		TSharedPtr<Ink::FListDefinition> origin = GetListOrigin(item.OriginName, item.ItemName);
		if (!origin.IsValid())
			return false;

		origins->AddUnique(origin);
	}

	return true;
}

bool UInkpotStory::CreateInkValues( const TArray<FInkpotValue>& InValues, TArray<TSharedPtr<Ink::FValueType>> &OutValues)
{
	OutValues.Reserve( InValues.Num() );
	for( const FInkpotValue &inValue : InValues )
	{
		if ( !ValidateInkListOrigin( inValue ) )
			return false;
		OutValues.Emplace( *inValue );
	}
	return true;
}

void UInkpotStory::ChoosePathStringInternal( const FString& InPath, const TArray<FInkpotValue>& InValues )
{
	TArray<TSharedPtr<Ink::FValueType>> values;
	if(!CreateInkValues( InValues, values ))
		return;
	StoryInternal->ChoosePathString( InPath, true, values );
}

FString UInkpotStory::Continue()
{
	return ContinueInternal();
}

FString UInkpotStory::ContinueInternal()
{
	return StoryInternal->Continue();
}

FString UInkpotStory::ContinueMaximally()
{
	return ContinueMaximallyInternal();
}

FString UInkpotStory::ContinueMaximallyInternal()
{
	return StoryInternal->ContinueMaximally();
}

FString UInkpotStory::ContinueMaximallyAtPath(const FString& InPath)
{
	FString ret; 
	ChoosePath(InPath);
	if (CanContinue())
		ret = ContinueMaximally();
	return ret;
}

bool UInkpotStory::CanContinue()
{
	return CanContinueInternal();
}

bool UInkpotStory::CanContinueInternal()
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
		choice->Initialise( inkChoice );
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
	OnFlowChangeInternal();
	return !isFlowAlive; 
}

void UInkpotStory::RemoveFlow(const FString& InFlowName)
{
	FString currentFlow = GetCurrentFlowName();
	StoryInternal->RemoveFlow( InFlowName );
	if( !InFlowName.Equals( currentFlow ) )
		return;
	OnFlowChangeInternal();
}

FString UInkpotStory::GetCurrentFlowName()
{
	return StoryInternal->GetCurrentFlowName();
}

void UInkpotStory::SwitchToDefaultFlow()
{
	StoryInternal->SwitchToDefaultFlow();
	OnFlowChangeInternal();
}

void UInkpotStory::OnFlowChangeInternal()
{
	UpdateChoices();
	BroadcastFlowChange();
	DebugRefresh();
}

void UInkpotStory::BroadcastFlowChange()
{
	if(!EventOnSwitchFlow.IsBound())
		return;
	FString currentFlow = GetCurrentFlowName();
	EventOnSwitchFlow.Broadcast( this, currentFlow );
}

bool UInkpotStory::IsFlowAlive( const FString &InFlowName )
{
	TSharedPtr<Ink::FStoryState> state = StoryInternal->GetStoryState();
	TSharedPtr<TMap<FString, TSharedPtr<Ink::FFlow>>> namedFlows = state->GetNamedFlows();
	if( namedFlows.IsValid() )
		return namedFlows->Contains( InFlowName );
	return false;
}

const TArray<FString> &UInkpotStory::GetAliveFlowNames()
{
	TSharedPtr<Ink::FStoryState> state = StoryInternal->GetStoryState();
	return state->GetAliveFlowNames();
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

void UInkpotStory::SetValue(const FString &InVariable, FInkpotValue InValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess = variableState->SetVariable( InVariable, Ink::FValue::Create(**InValue) );
}

void UInkpotStory::GetValue(const FString &InVariable, FInkpotValue &OutReturnValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	TSharedPtr<Ink::FObject> variableObj = variableState->GetVariable( InVariable );
	TSharedPtr<Ink::FValue> variable = StaticCastSharedPtr<Ink::FValue>( variableObj );
	Ink::FValueType value;
	if(variable.IsValid())
	{
		value = variable->GetValueObject();
		OutSuccess = true;
	}
	else
	{
		INKPOT_ERROR("Could not get value of variable '%s'", *InVariable );
		OutSuccess = false;
	}
	OutReturnValue = FInkpotValue(value);
}

void UInkpotStory::SetBool(const FString &InVariable, bool bInValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess = variableState->SetVariable( InVariable, MakeShared<Ink::FValueBool>( bInValue ) );
}

void UInkpotStory::GetBool(const FString &InVariable, bool &OutReturnValue, bool &OutSuccess )
{
	OutSuccess = GetVariable<bool, Ink::FValueBool>(InVariable, Ink::EValueType::Bool, OutReturnValue );
}

void UInkpotStory::SetInt(const FString &InVariable, int32 InValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess = variableState->SetVariable( InVariable, MakeShared<Ink::FValueInt>( InValue ) );
}

void UInkpotStory::GetInt(const FString &InVariable, int32 &OutValue ,  bool &OutSuccess )
{
	OutSuccess = GetVariable<int32, Ink::FValueInt>(InVariable, Ink::EValueType::Int, OutValue );
}

void UInkpotStory::SetFloat(const FString &InVariable, float InValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess = variableState->SetVariable( InVariable, MakeShared<Ink::FValueFloat>(InValue));
}

void UInkpotStory::GetFloat(const FString &InVariable, float &OutReturnValue, bool &OutSuccess )
{
	OutSuccess = GetVariable<float, Ink::FValueFloat>(InVariable, Ink::EValueType::Float, OutReturnValue );
}

void UInkpotStory::SetString(const FString &InVariable, const FString &InValue, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess = variableState->SetVariable( InVariable, MakeShared<Ink::FValueString>(InValue) );
}

void UInkpotStory::GetString( const FString& InVariable, FString &OutReturnValue, bool &OutSuccess )
{
	OutSuccess = GetVariable<FString, Ink::FValueString>(InVariable, Ink::EValueType::String, OutReturnValue );
}

void UInkpotStory::SetEmpty(const FString &InVariable, bool &OutSuccess )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	OutSuccess  = variableState->SetVariable( InVariable, MakeShared<Ink::FValue>() );
}

bool UInkpotStory::IsVariableDefined( const FString& InVariable )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	TSharedPtr<Ink::FObject> variable = variableState->GetVariable(  InVariable );
	return variable.IsValid();
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

void UInkpotStory::SetOnVariableChange( FOnInkpotVariableChange InDelegate, const FString &InVariable )
{
	TSharedPtr<FStoryVariableObserver> observer = MakeShared<FStoryVariableObserver>();
	observer->BindWeakLambda
	(
		InDelegate.GetUObject(), [this, InDelegate] (const FString& InVar, Ink::FValueType InType)
		{
			InDelegate.Execute( this, InVar, FInkpotValue(InType) );
		}
	);
	
	VariableObservers.AddUnique( InVariable, observer );
	StoryInternal->ObserveVariable( InVariable, observer );
}

bool UInkpotStory::ClearVariableChange( const UObject* Owner, const FString &Variable )
{
	if (!Owner || !VariableObservers.Contains(Variable))
		return false;

	bool bRemovedAtLeastOneEntry = false;
	
	for (auto Iterator = VariableObservers.CreateConstKeyIterator(Variable); Iterator; ++Iterator)
	{
		if (Iterator->Value.IsValid() && Owner == Iterator->Value->GetUObject())
		{
			StoryInternal->RemoveVariableObserver( Iterator->Value, Variable );
			VariableObservers.RemoveSingle( Variable, Iterator->Value );
			bRemovedAtLeastOneEntry = true;
		}
	}

	return bRemovedAtLeastOneEntry;
}

void UInkpotStory::ClearAllVariableObservers( const FString& Variable )
{
	if (!VariableObservers.Contains(Variable))
		return;
	
	for (auto Iterator = VariableObservers.CreateConstKeyIterator(Variable); Iterator; ++Iterator)
	{
		StoryInternal->RemoveVariableObserver( Iterator->Value, Variable );
	}

	VariableObservers.Remove(Variable);
}

void UInkpotStory::OnContinueInternal()
{
	if (!bIsInFunctionEvaluation)
	{
		UpdateChoices();
		DumpDebug();
		if (!EventOnContinue.IsBound())
			return;
		EventOnContinue.Broadcast(this);
	}
	DebugRefresh();
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
	//INKPOT_LOG("Starting function %s", *InFunctionName);
	bIsInFunctionEvaluation = true;
}

void UInkpotStory::OnCompleteEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms, const FString& OutParmName, TSharedPtr<Ink::FValueType> OutParmType)
{
	//INKPOT_LOG("Completing function %s", *InFunctionName);
	bIsInFunctionEvaluation = false;
}

void UInkpotStory::OnChoosePathStringInternal(const FString& InPath, const TArray<TSharedPtr<Ink::FValueType>>& InPathType )
{
	UpdateChoices();
	if(EventOnChoosePath.IsBound())
		EventOnChoosePath.Broadcast( this, InPath ); 
	DebugRefresh();
}

void UInkpotStory::DumpDebug()
{
	FString currentFlow = GetCurrentFlowName();
	INKPOT_LOG("Flow         : %s", *currentFlow);

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
			INKPOT_LOG("Flows alive  : %s", *flowsAlive );
		}
	}

	FString currentText = GetCurrentText();
	INKPOT_LOG("Text         : %s", *currentText);

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
		INKPOT_LOG("CTags        : %s", *tagsSet);
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
		INKPOT_LOG("GTags        : %s", *tagsSet);
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
				INKPOT_LOG("Variables    : %s = %s", *key, *value);
			}
			else
			{
				INKPOT_LOG("             : %s = %s", *key, *value);
			}
		}
	}

	if(Choices.Num() > 0)
	{
		for( int32 i=0; i<Choices.Num(); ++i )
		{
			FString tagsSet;
			for( auto tag : Choices[i]->GetTags() )
			{
				tagsSet.Append( "'#" );
				tagsSet.Append( tag );
				tagsSet.Append( "' " );
			}
			if( i == 0 ) 
			{
				INKPOT_LOG( "Choice       : %d - %s  %s", Choices[ i ]->GetIndex(), *Choices[ i ]->GetString(), *tagsSet );
			}
			else
			{
				INKPOT_LOG( "             : %d - %s  %s", Choices[ i ]->GetIndex(), *Choices[ i ]->GetString(), *tagsSet );
			}
		}
	}
}

void UInkpotStory::DumpDebug(UInkpotChoice *InChoice)
{
	if(!InChoice)
		return;
	INKPOT_LOG("Chose        : %d - %s", InChoice->GetIndex(), *InChoice->GetString());
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
	TSet<TSharedPtr<Ink::FContainer>> containersDumped;

	for( TSharedPtr<Ink::FObject> obj : *contents)
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>(obj);
		if( container )
		{
			containersDumped.Add(container);
			FString name = container->GetName();
			if (name.IsEmpty())
				name = "inline";
			DumpContainer(name, container, Indent + 1 );
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
		if(containersDumped.Contains(container))
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

void UInkpotStory::GatherAllStrings( TMap<FString, FString> &OutStrings )
{
	TSharedPtr<Ink::FContainer> main = StoryInternal->GetMainContentContainer();
	GatherAllStrings( "", main, OutStrings);
}

void UInkpotStory::GatherAllStrings( const FString &InRootName, TSharedPtr<Ink::FContainer> InContainer, TMap<FString, FString> &OutStrings )
{
	if(!InContainer)
		return;

	FString rootName = InRootName;
	FString containerName = InContainer->GetName();
	if (rootName.IsEmpty())
		rootName = containerName;
	else if (containerName.Len() > 0)
		rootName += TEXT(".") + containerName;

	TArray<TSharedPtr<Ink::FObject>> *contents = InContainer->GetContent().Get();
	int contentIndex = 1;
	for( TSharedPtr<Ink::FObject> obj : *contents)
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>(obj);
		if( container )
		{
			GatherAllStrings( rootName, container, OutStrings  );
		}
		else
		{
			if(obj->CanCastTo(Ink::EInkObjectClass::FValueString))
			{
				FString entry = obj->ToString();
				entry.TrimEndInline();
				if ( entry.Len() )
				{
					FString key = FString::Printf(TEXT("%s.%02d"), *rootName, contentIndex);
					OutStrings.Add( key, entry );
					INKPOT_LOG( "%s : %s", *key, *entry );
					contentIndex++;
				}
			}
		}
	}

	TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> namedContentPtr = InContainer->GetNamedContent();
	for( auto pair : *namedContentPtr )
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>( pair.Value );
		if(!container)
			continue;
		GatherAllStrings( rootName, container, OutStrings );
	}
}

TArray<FString> UInkpotStory::GetNamedContent()
{
	TSharedPtr<Ink::FContainer> container = StoryInternal->GetMainContentContainer();
	return GetNamedContent(container);
}

TArray<FString> UInkpotStory::GetNamedContentForKnot(const FString& InKnotName)
{
	TSharedPtr<Ink::FContainer> knotContainer = StoryInternal->KnotContainerWithName(InKnotName);
	return GetNamedContent(knotContainer);
}

TArray<FString> UInkpotStory::GetNamedContentForPath(const FString& InPath)
{
	TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>(InPath);
	Ink::FSearchResult result = StoryInternal->ContentAtPath(path);
	return GetNamedContent(result.GetObjectAsContainer());
}

TArray<FString> UInkpotStory::GetNamedContent(TSharedPtr<Ink::FContainer> InContainer)
{
	TArray<FString> keys;
	if (!InContainer)
	{
		INKPOT_ERROR("Container is invalid, no content.");
		return keys;
	}

	TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> namedContentPtr = InContainer->GetNamedContent();
	for (auto pair : *namedContentPtr)
	{
		TSharedPtr<Ink::FContainer> container = Ink::FObject::DynamicCastTo<Ink::FContainer>(pair.Value);
		if (!container)
			continue;
		keys.Add(pair.Key);
	}

	return keys;
}

void UInkpotStory::BindExternalFunction( const FString &InFunctionName, FInkpotExternalFunction InFunction, bool bInLookAheadSafe  )
{
	if (StoryInternal->IsExternalFunctionBound(InFunctionName))
	{
		INKPOT_WARN("Function '%s' is already bound, use UnbindExternalFunction to clear.", *InFunctionName);
		return;
	}

	TSharedPtr<FStoryExternalFunction> function = MakeShared<FStoryExternalFunction>();
	function->BindWeakLambda
	(
		InFunction.GetUObject(), [this, InFunction] (const TArray<Ink::FValueType> &InArguments) -> TSharedPtr<Ink::FValueType>
		{
			TArray<FInkpotValue> arguments;
			for(auto &argument : InArguments )
				arguments.Add( argument );

			FInkpotValue value = InFunction.Execute( arguments );
			return *value;
		}
	);
	StoryInternal->BindExternalFunctionGeneral( InFunctionName, function, bInLookAheadSafe );
}

void UInkpotStory::UnbindExternalFunction( const FString &InFunctionName )
{
	if (!StoryInternal->IsExternalFunctionBound(InFunctionName))
	{
		INKPOT_WARN("Function '%s' does not exist.", *InFunctionName);
		return;
	}

	StoryInternal->UnbindExternalFunction( InFunctionName );
}

void UInkpotStory::EvaluateFunction(const FString& FunctionName, const TArray<FInkpotValue> &InValues)
{
	TArray<TSharedPtr<Ink::FValueType>> values;
	if (!CreateInkValues(InValues, values))
		return;
	StoryInternal->EvaluateFunction(FunctionName, values);
}

FOnStoryContinue& UInkpotStory::OnContinue()
{
	return EventOnContinue;
}

FOnMakeChoice& UInkpotStory::OnMakeChoice()
{
	return EventOnMakeChoice;
}

FOnChoosePath& UInkpotStory::OnChoosePath()
{
	return EventOnChoosePath;
}

FOnSwitchFlow& UInkpotStory::OnSwitchFlow()
{
	return EventOnSwitchFlow;
}

FOnStoryLoadJSON& UInkpotStory::OnStoryLoadJSON()
{
	return EventOnStoryLoadJSON;
}

UInkpotLine *UInkpotStory::GetCurrentLine()
{
	UInkpotLine* line = NewObject<UInkpotLine>( this );
	line->Initialise( GetCurrentText() );
	return line;
}

void UInkpotStory::ResetContent( TSharedPtr<FInkpotStoryInternal> InNewStoryContent )
{
	StoryInternal->ResetContent( InNewStoryContent );
}

void UInkpotStory::ResetState()
{
	StoryInternal->ResetState();
	BindOnVariableStateChangeEvent();
}

FString UInkpotStory::ToJSON()
{
	return StoryInternal->GetStoryState()->ToJson();
}

void UInkpotStory::LoadJSON(const FString &InJSON)
{
	StoryInternal->GetStoryState()->LoadJSON( InJSON );
	UpdateChoices();
	if(EventOnStoryLoadJSON.IsBound())
		EventOnStoryLoadJSON.Broadcast( this );
}

void UInkpotStory::GetListEntries(const FString& InVariable, TArray<FString>& ReturnValues, bool& bOutSuccess)
{
	FInkpotValue value;
	GetValue(InVariable, value, bOutSuccess);
	ReturnValues = UInkpotLibrary::InkpotValueAsList(value);
}

void UInkpotStory::SetListEntries(const FString& InVariable, const TArray<FString>& InValues, bool& bOutSuccess, bool bInAppend)
{
	if (!bInAppend)
	{
		SetEmpty(InVariable, bOutSuccess);
		if (!bOutSuccess)
			return;
	}

	for (const FString& value : InValues)
	{
		SetListEntry(InVariable, value, bOutSuccess, true);
		if (!bOutSuccess)
			return;
	}
}

FInkpotValue UInkpotStory::MakeSingleEntryList( const FString& InVariable, const FString& InValue, bool& bOutSuccess)
{
	FInkpotValue newvalue = UInkpotLibrary::MakeInkpotList(InVariable, { InValue });
	bOutSuccess = ValidateInkListOrigin(newvalue);
	return newvalue;
}

void UInkpotStory::SetListEntry(const FString& InVariable, const FString& InValue, bool& bOutSuccess, bool bInAppend)
{
	FInkpotValue newvalue = MakeSingleEntryList(InVariable, InValue, bOutSuccess );
	if (!bOutSuccess)
		return;

	if (bInAppend)
	{
		FInkpotValue currentValue;
		GetValue(InVariable, currentValue, bOutSuccess);
		if (!bOutSuccess)
			return;

		if ((*currentValue)->HasSubtype<Ink::FInkList>())
		{
			// TODO : a blueprinttype encapsulation of the list itself 
			const Ink::FInkList &currentlist = (*currentValue)->GetSubtype<Ink::FInkList>();
			Ink::FInkList &newlist = (*newvalue)->GetSubtype<Ink::FInkList>();
			newlist = newlist.Union( currentlist );
		}

		SetValue(InVariable, newvalue, bOutSuccess);
	}
	else
	{
		SetValue(InVariable, newvalue, bOutSuccess);
	}
}

void UInkpotStory::ClearListEntry(const FString& InVariable, const FString& InValue, bool& bOutSuccess)
{
	FInkpotValue currentValue;
	GetValue(InVariable, currentValue, bOutSuccess);
	if (!bOutSuccess)
		return;

	if (!(*currentValue)->HasSubtype<Ink::FInkList>())
	{
		INKPOT_ERROR("Variable '%s' is not a List.", *InVariable);
		bOutSuccess = false;
		return;
	}

	FInkpotValue removevalue = MakeSingleEntryList(InVariable, InValue, bOutSuccess);
	if (!bOutSuccess)
		return;

	Ink::FInkList& removelist = (*removevalue)->GetSubtype<Ink::FInkList>();
	Ink::FInkList& currentlist = (*currentValue)->GetSubtype<Ink::FInkList>();

	currentlist = currentlist.Without(removelist);
	SetValue( InVariable, currentValue, bOutSuccess);
}

void UInkpotStory::ClearListEntries(const FString& InVariable, const TArray<FString>& InValues, bool& bOutSuccess)
{
	for (const FString& value : InValues)
	{
		ClearListEntry(InVariable, value, bOutSuccess);
		if (!bOutSuccess)
			return;
	}
}

void UInkpotStory::CheckListEntry(const FString& InVariable, const FString& InValue, bool& ReturnValue, bool& bOutSuccess)
{
	FInkpotValue currentValue;
	GetValue(InVariable, currentValue, bOutSuccess);
	if (!bOutSuccess)
		return;

	if (!(*currentValue)->HasSubtype<Ink::FInkList>())
	{
		INKPOT_ERROR("Variable '%s' is not a List.", *InVariable);
		bOutSuccess = false;
		return;
	}

	FInkpotValue queryvalue = MakeSingleEntryList(InVariable, InValue, bOutSuccess);
	if (!bOutSuccess)
		return;

	Ink::FInkList& querylist = (*queryvalue)->GetSubtype<Ink::FInkList>();
	Ink::FInkList& currentlist = (*currentValue)->GetSubtype<Ink::FInkList>();
	ReturnValue = currentlist.HasIntersection( querylist );
}

int UInkpotStory::GetStorySeed() const
{
	return StoryInternal->GetStoryState()->GetStorySeed();
}

void UInkpotStory::SetStorySeed( int Seed )
{
	StoryInternal->GetStoryState()->SetStorySeed( Seed );
}

void UInkpotStory::BindOnVariableStateChangeEvent()
{
	StoryInternal->GetVariablesState()->VariableChanged.AddUObject(this, &UInkpotStory::OnVariableStateChangeEvent);
}

void UInkpotStory::OnVariableStateChangeEvent(const FString& VariableName, TSharedPtr<Ink::FObject> NewValueObj)
{
	DebugRefresh();
}

void UInkpotStory::DebugRefresh()
{
#if WITH_EDITOR
	if (!EventOnDebugRefresh.IsBound())
		return;
	EventOnDebugRefresh.Broadcast(this);
#endif
}

#if WITH_EDITOR
FOnStoryContinue& UInkpotStory::OnDebugRefresh()
{
	return EventOnDebugRefresh;
}
#endif




