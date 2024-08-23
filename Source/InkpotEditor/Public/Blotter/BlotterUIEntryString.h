#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blotter/BlotterString.h"
#include "BlotterUIEntryString.generated.h"

UCLASS()
class INKPOTEDITOR_API UBlotterUIEntryString : public UEditorUtilityWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTextUpdate"))
	void ReceiveOnTextUpdate(const UBlotterString* Variable);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
