#pragma once

#include "Coreminimal.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotLine.h"
#include "Inkpot/InkpotValue.h"
#include "Utility/InkpotLog.h"
#include "InkpotStory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryContinue, UInkpotStory*, Story );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMakeChoice, UInkpotStory*, Story, UInkpotChoice*, Choice );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoosePath, UInkpotStory*, Story, const FString&, Path );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchFlow, UInkpotStory*, Story, const FString&, Flow );
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnInkpotVariableChange, UInkpotStory*, Story, const FString &, Variable, const FInkpotValue &, NewValue );
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FInkpotValue, FInkpotExternalFunction, const TArray<FInkpotValue> & , Values );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryLoadJSON, UInkpotStory*, Story );

UCLASS(BlueprintType)
class INKPOT_API UInkpotStory : public UObject
{
	GENERATED_BODY()

public:
	void Initialise( TSharedPtr<FInkpotStoryInternal>  InInkpotStory );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString Continue();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString ContinueMaximally();

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	FString ContinueMaximallyAtPath(const FString &Path );

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool CanContinue();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetCurrentText();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	UInkpotLine *GetCurrentLine();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool HasChoices();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<UInkpotChoice*>& GetCurrentChoices();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChooseChoice( UInkpotChoice *Choice );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChooseChoiceIndex( int32 Choice );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	bool SwitchFlow( const FString &FlowName );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void RemoveFlow(const FString& FlowName);

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetCurrentFlowName();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SwitchToDefaultFlow();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool IsFlowAlive( const FString &FlowName );

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<FString> &GetAliveFlowNames();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	int32 GetAliveFlowCount();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<FString>& GetCurrentTags();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	TArray<FString> GlobalTags();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	TArray<FString> TagsForContentAtPath( const FString &Path );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChoosePath( const FString &Path );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChoosePathString( const FString &Path, const TArray<FInkpotValue> &Values );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetValue(const FString &Variable, FInkpotValue Value);

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FInkpotValue GetValue(const FString &Variable);

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetBool(const FString &Variable, bool bValue);

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool GetBool(const FString &Variable);

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetInt(const FString &Variable, int32 Value);

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	int32 GetInt(const FString &Variable);

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetFloat( const FString& Variable, float Value );

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	float GetFloat( const FString& Variable );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetString( const FString& Variable, const FString& Value );

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetString( const FString& Variable );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetEmpty( const FString& Variable );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetOnVariableChange( UPARAM(DisplayName="Event") FOnInkpotVariableChange Delegate, const FString &Variable );

	// @param Owner Object that initially called SetOnVariableChange
	UFUNCTION(BlueprintCallable, Meta = (DefaultToSelf = "Owner", ExpandBoolAsExecs = "ReturnValue"), Category="Inkpot|Story")
	bool ClearVariableChange( const UObject* Owner, const FString &Variable );
	
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ClearAllVariableObservers( const FString &Variable );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContent();

	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContentForKnot(const FString &KnotName);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContentForPath(const FString& Path);

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void BindExternalFunction( const FString &FunctionName, UPARAM(DisplayName="Function") FInkpotExternalFunction Function, bool bLookAheadSafe = true );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void UnbindExternalFunction( const FString &FunctionName );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString ToJSON();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void LoadJSON(const FString &InJSON);

	int GetStorySeed() const;
	void SetStorySeed( int Seed );

	int32 GetVariableKeys( TArray<FString> &OutKeys );

	TSharedPtr<Ink::FObject> GetVariable( const FString& InVariable );

	template<typename T, typename InkType> 
	bool GetVariable( const FString& InVariable, Ink::EValueType InType, T& OutValue );

	void ObserveVariable( const FString& Variable, TSharedPtr<FStoryVariableObserver> Observer );

	FOnStoryContinue& OnContinue(); 
	FOnMakeChoice& OnMakeChoice(); 
	FOnChoosePath& OnChoosePath(); 
	FOnSwitchFlow& OnSwitchFlow(); 
	FOnStoryLoadJSON& OnStoryLoadJSON(); 

	void ResetContent( TSharedPtr<FInkpotStoryInternal> InNewStoryContent ); 
	void ResetState();

	int32 GetID() const;
	bool IsValid() const;

	TSharedPtr<FInkpotStoryInternal> GetStoryInternal();

	void DumpMainContent();
	void DumpContentAtPath( const FString& InName );
	void DumpContentAtKnot( const FString& InName );
	void DumpContainer(const FString& InName, TSharedPtr<Ink::FContainer> InContainer, int Indent = 0);

	void GatherAllStrings( TMap<FString, FString> &OutStrings );

protected:
	void OnContinueInternal();
	void OnMakeChoiceInternal(TSharedPtr<Ink::FChoice> InChoice);
	void OnEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms);
	void OnCompleteEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms, const FString& OutParmName, TSharedPtr<Ink::FValueType> OutParmType);
	void OnChoosePathStringInternal(const FString& InPath, const TArray<TSharedPtr<Ink::FValueType>>& InPathType );
	void BroadcastFlowChange();

	void UpdateChoices();

	void DumpDebug();
	void DumpDebug(UInkpotChoice *Choice);
	
	TArray<FString> GetNamedContent( TSharedPtr<Ink::FContainer> Container );

	void GatherAllStrings(const FString &InRootName, TSharedPtr<Ink::FContainer> InContainer, TMap<FString, FString>& OutStrings);

protected:
	TSharedPtr<FInkpotStoryInternal> StoryInternal;
	
	TMultiMap<FString, TSharedPtr<FStoryVariableObserver>> VariableObservers;

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnContinue") )
	FOnStoryContinue EventOnContinue; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnMakeChoice") )
	FOnMakeChoice EventOnMakeChoice; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnChoosePath") )
	FOnChoosePath EventOnChoosePath; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnSwitchFlow") )
	FOnSwitchFlow EventOnSwitchFlow; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnStoryLoadJSON") )
	FOnStoryLoadJSON EventOnStoryLoadJSON;

private:
	UPROPERTY(Transient)
	TArray<UInkpotChoice*> Choices;
};


inline TSharedPtr<Ink::FObject> UInkpotStory::GetVariable( const FString& InVariable )
{
	TSharedPtr<Ink::FVariableState> variableState = StoryInternal->GetVariablesState();
	TSharedPtr<Ink::FObject> variableObj = variableState->GetVariable( InVariable );
	return variableObj;
}

template<typename T, typename InkType> 
bool UInkpotStory::GetVariable( const FString& InVariable, Ink::EValueType InType, T& OutValue )
{
	TSharedPtr<InkType> variable = StaticCastSharedPtr<InkType>( GetVariable( InVariable ) );
	bool success = false;
	if(variable.IsValid())
	{
		success = (variable->GetType() == InType);
		OutValue = variable->GetValue();
	}

	if(!success)
		INKPOT_ERROR("Could not get value of variable '%s'", *InVariable );

	return success;
}

 