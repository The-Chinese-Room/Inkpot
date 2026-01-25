#include "Asset/InkpotStoryMetaDataSimple.h"

UInkpotStoryMetaDataSimple* UInkpotStoryMetaDataSimple::MakeInkpotStoryMetaDataSimple(FString InSomeExtraData, FString InEvenMoreExtraData)
{
	UInkpotStoryMetaDataSimple* object = NewObject<UInkpotStoryMetaDataSimple>();
	object->SomeExtraData = InSomeExtraData;
	object->EvenMoreExtraData = InEvenMoreExtraData;
	return object;
}

void UInkpotStoryMetaDataSimple::BreakInkpotStoryMetaDataSimple(UAssetUserData* InTarget, FString& OutSomeExtraData, FString& OutEvenMoreExtraData)
{
	UInkpotStoryMetaDataSimple* target = Cast<UInkpotStoryMetaDataSimple>(InTarget);
	if (!IsValid(target))
		return;
	OutSomeExtraData = target->SomeExtraData;
	OutEvenMoreExtraData = target->EvenMoreExtraData;
}
