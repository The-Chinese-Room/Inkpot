#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ink/Object.h"
#include "BlotterVariable.generated.h"

class UInkpotStory;

UENUM()
enum class EBlotterVariableType : uint8
{
	None,
	ChoicePoint,
	Container,
	ControlCommand,
	Divert,
	Glue,
	NativeFunctionCall,
	Tag,
	ValueBool,
	ValueDivertTarget,
	ValueFloat,
	ValueInt,
	ValueList,
	ValueString,
	ValueVariablePointer,
	Value,
	VariableAssignment,
	VariableReference,
	Void,
	Object,
	ListDefinition
};


UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterVariable : public UObject
{
	GENERATED_BODY()
public:
	void SetStory( UInkpotStory *Story );

	void SetName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetDisplayValue(const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetDisplayValue() const;
	bool SetVariableFromString(const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetType() const;
	void SetType( TSharedPtr<Ink::FObject> InObj );
	void SetType( EBlotterVariableType InType );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	int32 GetIndex() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsIndexEven() const;

	void SetIndex(uint32 InIndex );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsReadOnly() const;

protected:
	EBlotterVariableType GetTypeFromObject(TSharedPtr<Ink::FObject> InObj);
	FText GetTypeText(EBlotterVariableType InType);

private:
	UPROPERTY(Transient)
	UInkpotStory* Story;

	UPROPERTY(Transient)
	FText	Name;

	UPROPERTY(Transient)
	FText	Value;

	UPROPERTY(Transient)
	FText	TypeText;

	UPROPERTY(Transient)
	EBlotterVariableType Type;

	UPROPERTY(Transient)
	uint32	Index;
};
