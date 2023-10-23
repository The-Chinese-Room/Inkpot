#pragma once

#include "Coreminimal.h"
#include "Components/ActorComponent.h"
#include "InkpotWatch.generated.h"

class UInkpotStory;
struct FInkpotValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVariableChangeMulti, UInkpotStory*, Story, const FString &, Variable, const FInkpotValue &, NewValue );

UCLASS(ClassGroup = (Inkpot), meta = (BlueprintSpawnableComponent))
class INKPOT_API UInkpotWatch : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	void BindOnBeginStory();

	UFUNCTION()
	void OnBeginStory( UInkpotStory* InStory );

	UFUNCTION()
	void OnVariableChange( UInkpotStory* Story, const FString & Variable, const FInkpotValue & NewValue );

private:
	UPROPERTY(BlueprintAssignable, Category="Inkpot|Watch", meta=(DisplayName="OnVariableChange") )
	FOnVariableChangeMulti EventOnVariableChange; 

	/* the name of the Ink declared variable that we want to get change notifications for */
	UPROPERTY( EditAnywhere, Category="Inkpot|Watch" )
	FString VariableWatch;
};
