#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
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
	UPROPERTY(BlueprintAssignable, Category="Inkpot|Watch", meta=(DisplayName="OnVariableChange", EditCondition = "bUseGameplayTagLookup==false", EditConditionHides) )
	FOnVariableChangeMulti EventOnVariableChange; 

	/* use gameplay tag to as variable lookup */
	UPROPERTY(EditAnywhere, Category = "Inkpot|Watch")
	bool bUseGameplayTagLookup{ false };

	/* the name of the Ink declared variable that we want to get change notifications for */
	UPROPERTY( EditAnywhere, Category="Inkpot|Watch", meta = (EditCondition = "bUseGameplayTagLookup==false", EditConditionHides) )
	FString VariableWatch;

	/* the gameplay tag for the Ink declared variable that we want to get change notifications for */
	UPROPERTY(EditAnywhere, Category = "Inkpot|Watch", meta = (Categories = "Ink.Variable", EditCondition = "bUseGameplayTagLookup", EditConditionHides))
	FGameplayTag VariableWatchGT;
};
