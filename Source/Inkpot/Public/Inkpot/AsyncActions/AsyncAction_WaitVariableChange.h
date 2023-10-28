#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_WaitVariableChange.generated.h"

class UInkpot;
class UInkpotStory;
struct FInkpotValue;

UCLASS(BlueprintType, Meta = (ExposedAsyncProxy = "AsyncTask"))
class INKPOT_API UAsyncAction_WaitVariableChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInkpotVariableChangeSignature, UInkpotStory*, Story, const FString&, Variable, const FInkpotValue&, Value);
	
public:
	virtual void Activate() override;
	
	/**
	* This Task will listen for any Variable changes in the current Active Story
	* @param VariableToListen Name of the Variable to listen for changes
	* @param bTriggerOnce This Task will be automatically terminated upon the first Variable Change
	*/
	UFUNCTION(BlueprintCallable, Meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = true, AutoCreateRefTerm = "VariableToListen"), Category = "Inkpot|Async Actions")
	static UAsyncAction_WaitVariableChange* ListenForVariableChange(UObject* WorldContextObject, const FString& VariableToListen, const bool bTriggerOnce);

	/**
	* You must call this function manually when you want the AsyncTask to End
	* For UMG Widgets, you would call it in the Widget's Destruct event
	*/
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Async Actions")
	void EndTask();

	UPROPERTY(BlueprintAssignable, Transient, DuplicateTransient, DisplayName = "Variable Changed", Category = "Inkpot|Async Actions")
	FInkpotVariableChangeSignature OnVariableChangeDelegate;

private:
	UFUNCTION()
	void OnBeginStory(UInkpotStory* InStory);

	UFUNCTION()
	void OnEndStory(UInkpotStory* InStory);

	UFUNCTION()
	void OnVariableChange(UInkpotStory* InStory, const FString& InVariable, const FInkpotValue& NewValue);

	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<UInkpotStory> CurrentStory;

	UPROPERTY()
	TObjectPtr<UInkpot> Inkpot;
	
	FString VariableToListen;

	bool bTriggerOnce = false;
};
