#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blotter/BlotterString.h"
#include "BlotterContainer.generated.h"

UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterContainer : public UBlotterString
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
	int32 GetLevel() const;

	void SetLevel( int32 Level );

	void Add(UBlotterContainer *Container);

	UFUNCTION(BlueprintPure)
	const TArray<UBlotterContainer*> &GetChildren() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UBlotterContainer>> Children;

	UPROPERTY(VisibleAnywhere)
	int32 Level; 
};
