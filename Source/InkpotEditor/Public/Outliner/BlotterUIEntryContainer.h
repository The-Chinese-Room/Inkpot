#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Outliner/BlotterContainer.h"
#include "BlotterUIEntryContainer.generated.h"

UCLASS()
class INKPOTEDITOR_API UBlotterUIEntryContainer : public UEditorUtilityWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnContainerUpdate"))
	void ReceiveOnContainerUpdate(const UBlotterContainer* Container);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
