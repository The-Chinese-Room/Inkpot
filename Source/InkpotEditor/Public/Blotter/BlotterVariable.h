#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ink/Object.h"
#include "BlotterVariable.generated.h"


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

	//UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetValue(const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetValue() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetType() const;
	void SetType( TSharedPtr<Ink::FObject> InObj );
	void SetType( EBlotterVariableType InType );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	int32 GetIndex() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsIndexEven() const;

	void SetIndex(uint32 InIndex );

protected:
	EBlotterVariableType GetTypeFromObject(TSharedPtr<Ink::FObject> InObj);
	FText GetTypeText(EBlotterVariableType InType);

private:
	UPROPERTY(VisibleAnywhere)
	FText	Name;

	UPROPERTY(VisibleAnywhere)
	FText	Value;

	UPROPERTY(VisibleAnywhere)
	FText	TypeText;

	UPROPERTY(VisibleAnywhere)
	EBlotterVariableType Type;

	UPROPERTY(VisibleAnywhere)
	uint32	Index;
};
