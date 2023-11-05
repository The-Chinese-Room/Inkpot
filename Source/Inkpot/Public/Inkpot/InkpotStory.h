#pragma once

#include "Coreminimal.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotValue.h"
#include "InkpotStory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryContinue, UInkpotStory*, Story );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMakeChoice, UInkpotStory*, Story, UInkpotChoice*, Choice );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoosePath, UInkpotStory*, Story, const FString&, Path );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchFlow, UInkpotStory*, Story, const FString&, Flow );
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnVariableChange, UInkpotStory*, Story, const FString &, Variable, const FInkpotValue &, NewValue );

UCLASS(BlueprintType)
class INKPOT_API UInkpotStory : public UObject
{
	GENERATED_BODY()

public:
	void Initialise( TSharedRef<FInkpotStoryInternal>  InInkpotStory );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString Continue();

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString ContinueMaximally();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool CanContinue();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetCurrentText();

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
	void SwitchToDefautFlow();

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool IsFlowAlive( const FString &FlowName );

	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	TArray<FString> GetAliveFlowNames();

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
	void SetOnVariableChange( UPARAM(DisplayName="Event") FOnVariableChange Delegate, const FString &Variable );

	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ClearVariableChange( const FString &Variable );

	int32 GetVariableKeys( TArray<FString> &OutKeys );

	TSharedPtr<Ink::FObject> GetVariable( const FString& InVariable );

	template<typename T, typename InkType> 
	bool GetVariable( const FString& InVariable, Ink::EValueType InType, T& OutValue );

	void ObserveVariable( const FString& Variable, TSharedPtr<FStoryVariableObserver> Observer );

	int32 GetID() const;
	bool IsValid() const;

	TSharedPtr<FInkpotStoryInternal> GetStoryInternal();

	void DumpMainContent();
	void DumpContentAtPath( const FString& InName );
	void DumpContentAtKnot( const FString& InName );

private:
	void OnContinueInternal();
	void OnMakeChoiceInternal(TSharedPtr<Ink::FChoice> InChoice);
	void OnEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms);
	void OnCompleteEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms, const FString& OutParmName, TSharedPtr<Ink::FValueType> OutParmType);
	void OnChoosePathStringInternal(const FString& InPath, const TArray<TSharedPtr<Ink::FValueType>>& InPathType );

	void UpdateChoices();

	void DumpDebug();
	void DumpDebug(UInkpotChoice *Choice);

	void DumpContainer( const FString& InName, TSharedPtr<Ink::FContainer> InContainer, int Indent = 0 );

protected:
	TSharedPtr<FInkpotStoryInternal> StoryInternal;

	TMap<FString, TSharedPtr<FStoryVariableObserver>> VariableObservers;

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnContinue") )
	FOnStoryContinue EventOnContinue; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnMakeChoice") )
	FOnMakeChoice EventOnMakeChoice; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnChoosePath") )
	FOnChoosePath EventOnChoosePath; 

	UPROPERTY(BlueprintAssignable, Category="Inkpot|Story", meta=(DisplayName="OnSwitchFlow") )
	FOnSwitchFlow EventOnSwitchFlow; 

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
	bool success = (variable->GetType() == InType);
	OutValue = variable->GetValue();
	return success;
}

 