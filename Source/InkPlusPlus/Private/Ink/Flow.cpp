#include "Ink/Flow.h"

#include "Utility/InkPlusPlusLog.h"
#include "Ink/Story.h"
#include "Ink/Object.h"
#include "Ink/Choice.h"
#include "Ink/CallStack.h"
#include "Ink/JsonSerialisation.h"


Ink::FFlow::FFlow(const FString& InName, Ink::FStory* InStory)
	: Name(InName)
{
	CallStack = MakeShared<Ink::FCallStack>(InStory);
	CurrentChoices = MakeShared<TArray<TSharedPtr<Ink::FChoice>>>();
}

Ink::FFlow::FFlow(const FString& InName, Ink::FStory* InStory, const TSharedPtr<FJsonObject> InJSONObject)
	: Name(InName)
{
	CallStack = MakeShared<Ink::FCallStack>(InStory);

	const TSharedPtr<FJsonObject>* callStackJsonToken;
	if (!InJSONObject->TryGetObjectField(TEXT("callstack"), callStackJsonToken))
		UE_LOG(InkPlusPlus, Error, TEXT("Flow : failed to get CallStack JSON token out of passed in JSON Object!"));
	
	CallStack->SetJsonToken(*callStackJsonToken->Get(), InStory);

	const TArray<TSharedPtr<FJsonValue>>* outputStreamJsonArray;
	if (!InJSONObject->TryGetArrayField(TEXT("outputStream"), outputStreamJsonArray))
		UE_LOG(InkPlusPlus, Error, TEXT("Flow : failed to get Output Stream JSON array out of passed in JSON Object!"));

	OutputStream = Ink::FJsonSerialisation::JsonArrayToRuntimeObjectList(*outputStreamJsonArray);

	const TArray<TSharedPtr<FJsonValue>>* currentChoiceJsonArray;
	if (!InJSONObject->TryGetArrayField(TEXT("currentChoices"), currentChoiceJsonArray))
		UE_LOG(InkPlusPlus, Error, TEXT("Flow : failed to get Current Choices JSON array out of passed in JSON Object!"));
	CurrentChoices = Ink::FJsonSerialisation::JsonArrayToChoiceList(*currentChoiceJsonArray);

	// Choice Threads is optional
	const TSharedPtr<FJsonObject>* jsonChoiceThreadsObject;
	if (InJSONObject->TryGetObjectField(TEXT("choiceThreads"), jsonChoiceThreadsObject))
		LoadFlowChoiceThreads(jsonChoiceThreadsObject->Get()->Values, *InStory);
}


void Ink::FFlow::WriteJSON(TJsonWriter<>* InJSONWriter)
{
	InJSONWriter->WriteObjectStart();

	InJSONWriter->WriteIdentifierPrefix(TEXT("callstack"));
	CallStack->WriteJson(InJSONWriter);

	InJSONWriter->WriteIdentifierPrefix(TEXT("outputStream"));
	Ink::FJsonSerialisation::WriteListRuntimeObjects(InJSONWriter, OutputStream);

	// choiceThreads: optional
	// Has to come BEFORE the choices themselves are written out
	// since the originalThreadIndex of each choice needs to be set
	bool hasChoiceThreads = false;
	for (const TSharedPtr<Ink::FChoice>& choice : *CurrentChoices)
	{
		choice->SetOriginalThreadIndex(choice->GetThreadAtGeneration()->GetIndex());
		if (!CallStack->ThreadWithIndex(choice->GetOriginalThreadIndex()).IsValid())
		{
			if (!hasChoiceThreads)
			{
				hasChoiceThreads = true;
				InJSONWriter->WriteIdentifierPrefix(TEXT("choiceThreads"));
				InJSONWriter->WriteObjectStart();
			}

			InJSONWriter->WriteIdentifierPrefix(FString::FromInt(choice->GetOriginalThreadIndex()));
			choice->GetThreadAtGeneration()->WriteJson(InJSONWriter);
		}
	}

	if (hasChoiceThreads)
	{
		InJSONWriter->WriteObjectEnd();
	}

	InJSONWriter->WriteIdentifierPrefix(TEXT("currentChoices"));
	InJSONWriter->WriteArrayStart();
	for (const TSharedPtr<Ink::FChoice>& currentChoice : *CurrentChoices)
	{
		Ink::FJsonSerialisation::WriteChoice(InJSONWriter, *currentChoice);
	}
	InJSONWriter->WriteArrayEnd();

	InJSONWriter->WriteObjectEnd();
}

void Ink::FFlow::LoadFlowChoiceThreads(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONChoiceThreads, const Ink::FStory& InStory)
{
	for (const TSharedPtr<Ink::FChoice>& choice : *CurrentChoices)
	{
		TSharedPtr<Ink::FThread> foundActiveThread = CallStack->ThreadWithIndex(choice->GetOriginalThreadIndex());
		if (foundActiveThread.IsValid())
		{
			choice->SetThreadAtGeneration(foundActiveThread->Copy());
		}
		else
		{
			const TSharedPtr<FJsonValue> jsonSavedChoiceThread = InJSONChoiceThreads[FString::FromInt(choice->GetOriginalThreadIndex())];
			const TSharedPtr<FJsonObject>* jsonSavedChoiceThreadObject;
			if (!jsonSavedChoiceThread->TryGetObject(jsonSavedChoiceThreadObject))
			{
				UE_LOG(InkPlusPlus, Error, TEXT("Flow : cannot get JSON Saved Choice Thread object from JSON Saved Choice Thread Value!"))
				return;
			}
			TSharedPtr<FThread> thread = MakeShared<Ink::FThread>(*jsonSavedChoiceThreadObject->Get(), InStory);
			choice->SetThreadAtGeneration(thread);
		}
	}
}

TSharedPtr<Ink::FCallStack> Ink::FFlow::GetCallStack() const
{
	return CallStack;
}

void Ink::FFlow::SetCallStack(const Ink::FCallStack& InCallStack)
{
	CallStack = MakeShared<Ink::FCallStack>(InCallStack);
}

TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> Ink::FFlow::GetCurrentChoices()
{
	return CurrentChoices;
}

TArray<TSharedPtr<Ink::FObject>>& Ink::FFlow::GetOutputStream()
{
	return OutputStream;
}

void Ink::FFlow::SetOutputStream(const TArray<TSharedPtr<Ink::FObject>>& InOutputStream)
{
	OutputStream = InOutputStream;
}

FString Ink::FFlow::GetName() const
{
	return Name;
}

void Ink::FFlow::SetName(const FString& InName)
{
	Name = InName;
}
