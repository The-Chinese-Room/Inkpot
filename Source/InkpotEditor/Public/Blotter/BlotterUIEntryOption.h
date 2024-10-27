#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blotter/BlotterOption.h"
#include "BlotterUIEntryOption.generated.h"

UCLASS()
class INKPOTEDITOR_API UBlotterUIEntryOption : public UEditorUtilityWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnListUpdate"))
	void ReceiveOnListUpdate(const UBlotterOption* Option);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
