#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blotter/BlotterVariable.h"
#include "BlotterUIEntryVariable.generated.h"

UCLASS()
class INKPOTEDITOR_API UBlotterUIEntryVariable : public UEditorUtilityWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnVariableUpdate"))
	void ReceiveOnVariableUpdate(const UBlotterVariable* Variable);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};


