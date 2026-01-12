#include "Asset/InkpotStoryMetaDataSimple.h"


UInkpotStoryMetaDataSimple* UInkpotStoryMetaDataSimple::MakeInkpotStoryMetaDataSimple(FString InSomeExtraData)
{
	UInkpotStoryMetaDataSimple* object = NewObject<UInkpotStoryMetaDataSimple>();
	object->SomeExtraData = InSomeExtraData;
	return object;
}

void UInkpotStoryMetaDataSimple::BreakInkpotStoryMetaDataSimple(UInkpotStoryMetaData* InTarget, FString& OutSomeExtraData)
{
	UInkpotStoryMetaDataSimple* target = Cast<UInkpotStoryMetaDataSimple>(InTarget);
	if (!IsValid(target))
		return;
	OutSomeExtraData = target->SomeExtraData;
}
