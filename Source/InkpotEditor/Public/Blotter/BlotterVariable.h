#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ink/Object.h"
#include "Blotter/BlotterOption.h"
#include "BlotterVariable.generated.h"

class UBlotterUIEntryVariable;
class UInkpotStory;
struct FInkpotList;

UENUM(BlueprintType)
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
	ListOrigin
};


UCLASS(BlueprintType)
class INKPOTEDITOR_API UBlotterVariable : public UObject
{
	GENERATED_BODY()
public:
	void SetStory( UInkpotStory *Story );
	void SetDisplayWidget(UBlotterUIEntryVariable *DisplayWidget);

	void SetName(const FString& Name);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetDisplayValue(const FString& Value);
	void SetValue( UInkpotStory *Story, const FString &VariableName );
	void SetEmpty();

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetDisplayValue() const;
	bool SetVariableFromString(const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	const FText& GetTypeName() const;

	void SetType( TSharedPtr<Ink::FObject> InObj );
	void SetType( EBlotterVariableType InType );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	EBlotterVariableType GetType() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsType(EBlotterVariableType Type) const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	int32 GetIndex() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsIndexEven() const;

	void SetIndex(uint32 InIndex );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsReadOnly() const;

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	void GetListOptions( TArray<UBlotterOption*> &ReturnValue, bool &bSuccess );
	void SetListOption( UBlotterOption* Option );

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	bool SetOptionsOpen(bool bIsOptionsOpen );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsOptionsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Blotter")
	void SetPinned(bool bIsPinned );

	UFUNCTION(BlueprintPure, Category = "Inkpot|Blotter")
	bool IsPinned() const;

	void Refresh();

	bool operator < (const UBlotterVariable& RHS);

protected:
	EBlotterVariableType GetTypeFromObject(TSharedPtr<Ink::FObject> InObj);
	FText GetTypeText(EBlotterVariableType InType);
	FString GetListAsDisplayString(const FInkpotList &InValue);

private:
	UPROPERTY(Transient)
	TObjectPtr<UInkpotStory> Story;

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

	UPROPERTY(Transient)
	bool bOptionsOpen {false};

  UPROPERTY(Transient)
	bool bPinned {false};

	UPROPERTY(Transient)
	TObjectPtr<UBlotterUIEntryVariable> DisplayWidget {nullptr};
};
