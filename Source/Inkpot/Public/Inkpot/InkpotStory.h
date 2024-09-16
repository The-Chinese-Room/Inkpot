#pragma once

#include "CoreMinimal.h"
#include "Inkpot/InkpotStoryInternal.h"
#include "Inkpot/InkpotChoice.h"
#include "Inkpot/InkpotLine.h"
#include "Inkpot/InkpotValue.h"
#include "Inkpot/InkpotList.h"
#include "Utility/InkpotLog.h"
#include "InkpotStory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryContinue, UInkpotStory*, Story );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMakeChoice, UInkpotStory*, Story, UInkpotChoice*, Choice );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoosePath, UInkpotStory*, Story, const FString&, Path );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchFlow, UInkpotStory*, Story, const FString&, Flow );
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnInkpotVariableChange, UInkpotStory*, Story, const FString &, Variable, const FInkpotValue &, NewValue );
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FInkpotValue, FInkpotExternalFunction, const TArray<FInkpotValue> & , Values );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryLoadJSON, UInkpotStory*, Story );

// macro for binding functions in your derived story classes
#define BindInkFunction( NameInk, NameCPP ) \
	{\
		FInkpotExternalFunction func;\
		func.BindDynamic(this, NameCPP );\
		BindExternalFunction( TEXT(NameInk), func, false);\
	}\

UCLASS(BlueprintType)
class INKPOT_API UInkpotStory : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialise( TSharedPtr<FInkpotStoryInternal>  InInkpotStory );

	/**
	 * Continue
	 * Executes story script until choices or a newline are encountered.
	 *
	 * @returns The current line of text for the story.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString Continue();

	/**
	 * ContinueMaximally
	 * Executes story script until choice or no more content.
	 *
	 * @returns The collated lines of text up to the point of termination.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString ContinueMaximally();

	/**
	 * ContinueMaximallyAtMath
	 * Executes story script until choice or no more content, from a specified point in the story.
	 *
	 * @param Path - the knot and\or the stitch to start the story at, 
	 * @returns The collated lines of text up to the point of termination.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	FString ContinueMaximallyAtPath(const FString &Path );

	/**
	 * CanContinue
	 * Tests if the story can continue - the story still has content, and no choices. 
	 *
	 * @returns True if there is content, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool CanContinue();

	/**
	 * GetCurrentText
	 * Gets the current line of text for the story. 
	 *
	 * @returns The current line of text.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetCurrentText();

	/**
	 * GetCurrentLine
	 * Gets the localised line of text for the story, with tags. ( localisation to be implemented ) 
	 * Mainly for use with UMG list types. 
	 *
	 * @returns The current line of text, wrapped as a UInkpotLine.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	UInkpotLine *GetCurrentLine();

	/**
	 * HasChoices
	 * Does the story currently have active choices to display. 
	 *
	 * @returns true is the story has choices to display, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool HasChoices();

	/**
	 * GetCurrentChoices
	 * Returns the current choices of the story as an array of UInkpotChoices. ( localisation to be implemented ) 
	 * UInkpotChoices has both the text for the choice and the tags associated with them. 
	 * Mainly for use with UMG list types.
	 *
	 * @returns true is the story has choices to display, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<UInkpotChoice*>& GetCurrentChoices();

	/**
	 * ChooseChoice
	 * Choose a choice at a decision point in the story.
	 *
	 * @param Choice - a choice - typiclaly from the results given by GetCurrentChoices. 
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChooseChoice( UInkpotChoice *Choice );

	/**
	 * ChooseChoiceIndex
	 * Choose a choice at a decision point in the story.
	 *
	 * @param Choice - an index into the array returned by GetCurrentChoices. 
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChooseChoiceIndex( int32 Choice );

	/**
	 * SwitchFlow
	 * Switches the flow of the story. A flow is a path of execution of the story. Simple stories have but one flow, but there can be many.
	 *
	 * @param FlowName - the name of the flow to switch to. If a flow of that name does not exist a new flow will be created. 
	 * @returns true if the switch was succesful.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	bool SwitchFlow( const FString &FlowName );

	/**
	 * RemoveFlow
	 * Removes a flow from the current set of flows .
	 *
	 * @param FlowName - the name of the flow to remove.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void RemoveFlow(const FString& FlowName);

	/**
	 * GetCurrentFlowName
	 * Gets the name of the currently active flow.
	 *
	 * @returns The flowname of the active flow.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	FString GetCurrentFlowName();

	/**
	 * SwitchToDefaultFlow
	 * Switch back to executing the default flow.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SwitchToDefaultFlow();

	/**
	 * IsFlowAlive
	 * Checks if a flow still has content to display.
	 *
	 * @param FlowName - the name of the flow to check.
	 * @returns True if the flow still has content.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool IsFlowAlive( const FString &FlowName );

	/**
	 * GetAliveFlowNames
	 * Gets a list of all the flows that are alive.
	 *
	 * @returns Array of flow names.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<FString> &GetAliveFlowNames();

	/**
	 * GetAliveFlowCount
	 * Gets a the number of flows that are alive.
	 *
	 * @returns alive flow count.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	int32 GetAliveFlowCount();

	/**
	 * GetCurrentTags
	 * Gets a tags that are present for the current state of the story.
	 *
	 * @returns Array of tags.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	const TArray<FString>& GetCurrentTags();

	/**
	 * GlobalTags
	 * Gets a tags that are set at the top of the story.
	 *
	 * @returns Array of tags.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	TArray<FString> GlobalTags();

	/**
	 * TagsForContentAtPath
	 * Gets a tags that are set at the top of the knot and\or stitch passed in.
	 *
	 * @param Path - knot and\or stitch of where to look for the tags.
	 * @returns Array of tags.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	TArray<FString> TagsForContentAtPath( const FString &Path );

	/**
	 * ChoosePath
	 * Choose a new point of execution for the current flow.
	 *
	 * @param Path - knot and\or stitch of where to start.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChoosePath( const FString &Path );

	/**
	 * ChoosePathString
	 * Choose a new point of execution for the current flow. Passing paramters to the story.
	 *
	 * @param Path - knot and\or stitch of where to start.
	 * @param Values - Array of values to pass to the knot\stitch.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ChoosePathString( const FString &Path, const TArray<FInkpotValue> &Values );

	/**
	 * SetValue
	 * Sets the value of a variable in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetValue(const FString &Variable, const FInkpotValue Value, bool &Success );

	/**
	 * GetValue
	 * Gets the value of a variable in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void GetValue(const FString &Variable, FInkpotValue &ReturnValue, bool &bSuccess );

	/**
	 * SetBool
	 * Sets the value of a boolean variable in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetBool(const FString &Variable, bool bValue, bool &bSuccess );

	/**
	 * GetBool
	 * Gets the value of a boolean variable in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	void GetBool(const FString &Variable, bool &ReturnValue, bool &bSuccess );

	/**
	 * SetInt
	 * Sets the value of an integer variable in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetInt(const FString &Variable, int32 Value, bool &bSuccess );

	/**
	 * GetInt
	 * Gets the value of an Integer variable in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	void GetInt(const FString &Variable, int32 &ReturnValue, bool &bSuccess );

	/**
	 * SetFloat
	 * Sets the value of a floating point variable in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetFloat( const FString& Variable, float Value, bool &bSuccess );

	/**
	 * GetFloat
	 * Gets the value of a floating variable in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	void GetFloat( const FString& Variable, float &ReturnValue, bool &bSuccess );

	/**
	 * SetString
	 * Sets the value of a character string in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetString( const FString& Variable, const FString& Value, bool &bSuccess );

	/**
	 * GetString
	 * Gets the value of a character string in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess, returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	void GetString( const FString& Variable, FString &ReturnValue, bool &bSuccess );

	/**
	 * SetList
	 * Sets the value of an Ink List in the story.
	 *
	 * @param Variable - name of the variable to set.
	 * @param Value - the value to set the variable to.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetList( const FString& Variable, const FInkpotList &Value, bool &bSuccess );

	/**
	 * GetList
	 * Gets the value of an Ink List in the story.
	 *
	 * @param Variable - name of the variable to get.
	 * @param ReturnValue - the value of the variable.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	void GetList( const FString& Variable, FInkpotList &ReturnValue, bool &bSuccess );

	/**
	 * SetEmpty
	 * Clears the value of a variable in the story.
	 *
	 * @param Variable - name of the variable to clear.
	 * @param bSuccess - returns if the operation was a succes. False typically means the variable does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetEmpty( const FString& Variable, bool &bSuccess );

	/**
	 * IsVariableDefined
	 * Checks if a variable exists in the story.
	 *
	 * @param Variable - name of the variable to clear.
	 * @returns true if the variable exists, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Inkpot|Story")
	bool IsVariableDefined( const FString& Variable );

	/**
	 * SetOnVariableChange
	 * Binds a delegate to when the value of the stated variable changes.
	 *
	 * @param Delegate - the delegate to call when the variable changes.
	 * @param Variable - the name of the variable to watch.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void SetOnVariableChange( UPARAM(DisplayName="Event") FOnInkpotVariableChange Delegate, const FString &Variable );

	/**
	 * ClearVariableChange
	 * Clears delegate binding created by SetOnVariableChange.
	 *
	 * @param Owner - Object that initially called SetOnVariableChange
	 * @param Variable - the name of the variable.
	 */
	UFUNCTION(BlueprintCallable, Meta = (DefaultToSelf = "Owner", ExpandBoolAsExecs = "ReturnValue"), Category="Inkpot|Story")
	bool ClearVariableChange( const UObject* Owner, const FString &Variable );
	
	/**
	 * ClearAllVariableObservers
	 * Clears all delegate bindings for the named variable.
	 *
	 * @param Variable - the name of the variable.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void ClearAllVariableObservers( const FString &Variable );

	/**
	 * GetNamedContent
	 * Gets all named sections within a story. 
	 * This is essentially all the top level knots of the story.
	 *
	 * @returns List of all named knots.
	 */
	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContent();

	/**
	 * GetNamedContentForKnot
	 * Gets all named stitches within a knot. 
	 *
	 * @returns List of all named stitches within knot.
	 */
	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContentForKnot(const FString &KnotName);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Story")
	TArray<FString> GetNamedContentForPath(const FString& Path);

	/**
	 * BindExternalFunction
	 * Binds a blueprint or C++ function as an external call from Ink. 
	 *
	 * @param FunctionName - name of the function as it will be called from the context of the Ink script.
	 * @param Function - function, event or delegate to be called.
	 * @param bLookAheadSafe - whether the ink VM can safely call this function when parsing future content. 
	 *                        If your function is being called twice, set this to false.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void BindExternalFunction( const FString &FunctionName, UPARAM(DisplayName="Function") FInkpotExternalFunction Function, bool bLookAheadSafe = true );

	/**
	 * UnbindExternalFunction
	 * Unbinds a function that was bound by BindExternalFunction. 
	 *
	 * @param FunctionName - name of the function to unbind.
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	void UnbindExternalFunction( const FString &FunctionName );

	/**
	 * EvaluateFunction
	 * Evaluates a function defined in Ink script from Blueprint or C++. 
	 *
	 * @param FunctionName - name of the function to call in the script.
	 * @param InValues - the parameters to pass into the function
	 * @param ReturnValue - the to return from the function.
	 * @param CapturedText - any text that the function may generated in the normal ink output way
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	void EvaluateFunction(const FString& FunctionName, const TArray<FInkpotValue>& InValues, FInkpotValue &ReturnValue, FString &CapturedText);
	
	/**
	 * ToJSON
	 * Serialises the state of the Ink VM to a JSON string. 
	 *
	 * @returns String - the serialised state of the Ink VM 
	 */
	UFUNCTION(BlueprintCallable, Category="Inkpot|Story")
	FString ToJSON();

	/**
	 * LoadJSON
	 * Unserialises the state of the Ink VM from a JSON string. 
	 *
	 * @param InJSON - the serialised state to set the Ink VM to.
	 */
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

#if WITH_EDITOR 
	FOnStoryContinue& OnDebugRefresh();
#endif 

	virtual void ResetContent( TSharedPtr<FInkpotStoryInternal> InNewStoryContent ); 
	void ResetState();

	int32 GetID() const;
	bool IsValid() const;

	TSharedPtr<FInkpotStoryInternal> GetStoryInternal();

	/**
	 * GatherAllStrings
	 * Walks the whole of the story and finds all string definitions. 
	 * Write result to map, key=id, value=string found. id generated from knot and stitch name
	 *
	 * @param OutStrings - map to write the strings found into.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	void GatherAllStrings( TMap<FString, FString> &OutStrings );

	/**
	 * DumpDebug
	 * Writes current state of InkVM to debug log. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	void DumpDebug();

	/**
	 * DumpMainContent
	 * Writes all Ink opcodes for content of current story to debug log. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	virtual void DumpMainContent();

	/**
	 * DumpContentAtPath
	 * Writes all Ink opcodes for content at specified Knot & Stitch to debug log. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	void DumpContentAtPath( const FString& InName );

	/**
	 * DumpContentAtKnot
	 * Writes all Ink opcodes for content at specified Knot to debug log. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	void DumpContentAtKnot( const FString& InName );

	void DumpContainer(const FString& InName, TSharedPtr<Ink::FContainer> InContainer, int Indent = 0);

	TSharedPtr<Ink::FListDefinition> GetListOrigin(const FString& InOriginName, const FString& InItemName);

protected:
	virtual bool CanContinueInternal();
	virtual FString ContinueInternal();
	virtual FString ContinueMaximallyInternal();

	void OnContinueInternal();
	void OnMakeChoiceInternal(TSharedPtr<Ink::FChoice> InChoice);
	void OnEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms);
	void OnCompleteEvaluateFunctionInternal(const FString& InFunctionName, const TArray<TSharedPtr<Ink::FValueType>>& InFunctionParms, const FString& OutParmName, TSharedPtr<Ink::FValueType> OutParmType);
	void OnChoosePathStringInternal(const FString& InPath, const TArray<TSharedPtr<Ink::FValueType>>& InPathType );

	virtual void OnFlowChangeInternal();
	void BroadcastFlowChange();
	void UpdateChoices();

	void DumpDebug(UInkpotChoice *Choice);
	
	TArray<FString> GetNamedContent( TSharedPtr<Ink::FContainer> Container );

	void GatherAllStrings(const FString &InRootName, TSharedPtr<Ink::FContainer> InContainer, TMap<FString, FString>& OutStrings);
	
	virtual void ChoosePathInternal(const FString &InPath);
	virtual void ChoosePathStringInternal( const FString& InPath, const TArray<FInkpotValue>& InValues );

	void DebugRefresh();
	bool CreateInkValues( const TArray<FInkpotValue>& InValues, TArray<TSharedPtr<Ink::FValueType>>& OutValues );

	void OnVariableStateChangeEvent(const FString& VariableName, TSharedPtr<Ink::FObject> NewValueObj);

	void BindOnVariableStateChangeEvent();

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

#if WITH_EDITORONLY_DATA 
	UPROPERTY(BlueprintAssignable, Category = "Inkpot|Story", meta = (DisplayName = "OnDebugRefresh"))
	FOnStoryContinue EventOnDebugRefresh;
#endif 

private:
	UPROPERTY(Transient)
	TArray<UInkpotChoice*> Choices;

	UPROPERTY(Transient)
	bool bIsInFunctionEvaluation{ false };
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

 