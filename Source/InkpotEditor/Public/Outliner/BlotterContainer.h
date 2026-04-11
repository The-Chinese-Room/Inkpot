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

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	int32 GetLevel() const;

	void SetLevel( int32 Level );

	void Add(UBlotterContainer *Container);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const TArray<UBlotterContainer*> &GetChildren() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inkpot|Blotter")
	TArray<TObjectPtr<UBlotterContainer>> Children;

	UPROPERTY(VisibleAnywhere, Category = "Inkpot|Blotter")
	int32 Level;
};
