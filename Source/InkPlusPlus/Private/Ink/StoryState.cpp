
#include "Ink/StoryState.h"
#include "Utility/InkPlusPlusLog.h"
#include "Ink/Flow.h"
#include "Ink/CallStack.h"
#include "Ink/DebugMetadata.h"
#include "Ink/Path.h"
#include "Ink/Pointer.h"
#include "Ink/Story.h"
#include "Ink/SearchResult.h"
#include "Math/UnrealMathUtility.h"
#include "Ink/JsonExtension.h"
#include "Ink/Tag.h"
#include "Ink/JsonSerialisation.h"
#include "Ink/Glue.h"
#include "Ink/ControlCommand.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Algo/Reverse.h"
#include "Ink/Void.h"


Ink::FStoryState::FStoryState(Ink::FStory* InStory)
	: InkSaveStateVersion(9)
	, MinCompatibleLoadVersion(8)
	, DefaultFlowName("DEFAULT_FLOW")
	, DivertedPointer(Ink::FPointer())
	, CurrentTurnIndex(-1)
	, StorySeed(FMath::RandRange(0, 100))
	, PreviousRandom(0)
	, DidSafeExit(false)
	, Story(InStory)
	, OutputStreamTextDirty(true)
	, OutputStreamTagsDirty(true)
	, CurrentFlow(MakeShared<FFlow>(DefaultFlowName, InStory))
	, VariableState(MakeShared<FVariableState>(CallStack(), Story->GetListDefinitions()))
{
	SetOutputStreamDirty();
	GoToStart();
}

void Ink::FStoryState::GoToStart() const
{
	const FPointer startPointer = FPointer::StartOf(Story->GetMainContentContainer());
	CallStack()->CurrentElement()->SetCurrentPointer(startPointer);
}

FString Ink::FStoryState::ToJson() const
{
	FString jsonString;
	TSharedPtr<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&jsonString);
	WriteJson(writer.Get());
	writer->Close();
	return jsonString;
}

void Ink::FStoryState::ToJson(FArchive* const Stream) const
{
	TSharedPtr<TJsonWriter<>> writer = TJsonWriter<>::Create(Stream);
	WriteJson(writer.Get());
	writer->Close();
}

void Ink::FStoryState::LoadJSON(const FString& InJSON)
{
	TSharedPtr<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(InJSON);

	if (!FJsonSerializer::Deserialize(reader, jsonObject))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("Deserialisation of the JSON failed"));
		return;
	}

	LoadJSONObj(*jsonObject);
	onDidLoadState.Broadcast();
}

void Ink::FStoryState::LoadJSONObj(const FJsonObject& InJSONObj)
{
	const TSharedPtr<FJsonValue> jSaveVersion = InJSONObj.TryGetField(TEXT("inkSaveVersion"));
	checkf(jSaveVersion.IsValid(), TEXT("ink save format incorrect, can't load!"));

	const int32 saveVersion = static_cast<int32>(jSaveVersion->AsNumber());
	checkf(saveVersion > MinCompatibleLoadVersion, TEXT("Ink save format isn't compatible with the current version (saw '%d', but minimum is '%d'), so can't load."), saveVersion, MinCompatibleLoadVersion);

	/*	Flows: Always exists in latest format (even if there's just one default)
		but this map doesn't exist in previous format */
	const TSharedPtr<FJsonObject>* flowsObject = nullptr;
	if (InJSONObj.TryGetObjectField(TEXT("flows"), flowsObject))
	{
		TMap<FString, TSharedPtr<FJsonValue>> flowsObjectMap = flowsObject->Get()->Values;

		if (flowsObjectMap.Num() == 1) // Single default flow
		{
			NamedFlows.Reset(); // null the pointer
		}
		else if (!NamedFlows.IsValid()) // Multi-flow, need to create flows dict
		{
			NamedFlows = MakeShared<TMap<FString, TSharedPtr<FFlow>>>();
		}
		else // Multi-flow, already have a flows dict
		{
			NamedFlows->Reset(); // reset the map that the pointer points to
		}

		// Load up each flow (there may only be one)
		for (const TPair<FString, TSharedPtr<FJsonValue>>& flowPair : flowsObjectMap)
		{
			FString name = flowPair.Key;
			const TSharedPtr<FJsonObject> flowObject = flowPair.Value->AsObject();

			// Load up this flow using JSON data
			TSharedPtr<FFlow> flow = MakeShared<FFlow>(name, Story, flowObject);

			if (flowsObjectMap.Num() == 1)
			{
				CurrentFlow = MakeShared<FFlow>(name, Story, flowObject);
			}
			else
			{
				NamedFlows->Add(name, flow);
			}
		}

		if (NamedFlows.IsValid() && NamedFlows->Num() > 1)
		{
			FString currentFlowName;
			checkf(InJSONObj.TryGetStringField(TEXT("currentFlowName"), currentFlowName), TEXT("failed to get current flow name!"));
			CurrentFlow = NamedFlows->operator[](currentFlowName);
		}
	}

	// this section of c# code believed not to be necessary -- appears to be backwards compatibility. Omitted
	// Old format: individually load up callstack, output stream, choices in current/default flow
	/*
	else
	{
	}
	*/

	SetOutputStreamDirty();

	const TSharedPtr<FJsonObject>* variableStateObjectField = nullptr;
	if (InJSONObj.TryGetObjectField(TEXT("variablesState"), variableStateObjectField))
	{
		const TMap<FString, TSharedPtr<FJsonValue>> variableStateObjectMap = variableStateObjectField->Get()->Values;
		VariableState->SetJsonToken(variableStateObjectMap);
		VariableState->SetCallStack(CurrentFlow->GetCallStack());
	}

	const TArray<TSharedPtr<FJsonValue>> evalStackArrayField = InJSONObj.GetArrayField("evalStack");
	EvaluationStack = Ink::FJsonExtension::JSONValueToInkObject(evalStackArrayField);

	
	TSharedPtr<FJsonValue> currentDivertTargetPath = InJSONObj.TryGetField("currentDivertTarget");
	if (currentDivertTargetPath.IsValid())
	{
		const FString currentDivertTargetPathString = currentDivertTargetPath->AsString();
		TSharedPtr<Ink::FPath> divertPath = MakeShared<Ink::FPath>(currentDivertTargetPathString);
		DivertedPointer = Story->PointerAtPath(divertPath);
	}

	TMap<FString, TSharedPtr<FJsonValue>> visitCountsDictionary = InJSONObj.GetObjectField("visitCounts").Get()->Values;
	VisitCounts = Ink::FJsonExtension::JSONObjectToIntDictionary(visitCountsDictionary);

	TMap<FString, TSharedPtr<FJsonValue>> turnIndicesDictionary = InJSONObj.GetObjectField("turnIndices").Get()->Values;
	TurnIndices = Ink::FJsonExtension::JSONObjectToIntDictionary(turnIndicesDictionary);

	CurrentTurnIndex = InJSONObj.GetIntegerField("turnIdx");
	StorySeed = InJSONObj.GetIntegerField("storySeed");

	PreviousRandom = 0;
	InJSONObj.TryGetNumberField("previousRandom", PreviousRandom);
}

int Ink::FStoryState::VisitCountAtPathString(const FString& PathString)
{
	int visitCount = 0;

	if (Patch.IsValid())
	{
		const TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>(PathString);
		const Ink::FSearchResult searchResult = Story->ContentAtPath(path);
		const TSharedPtr<Ink::FContainer> container = searchResult.GetObjectAsContainer();

		if (Patch->TryGetVisitCount(container, visitCount))
		{
			return visitCount;
		}
	}

	int* result = VisitCounts.Find(PathString);
	if (result != nullptr)
	{
		visitCount = *result;
	}
	return visitCount;
}

int Ink::FStoryState::VisitCountForContainer(TSharedPtr<Ink::FContainer> Container)
{
	int count = 0;

	check(Container.IsValid());

	if (!Container->GetVisitsShouldBeCounted())
	{
		if (Container->GetDebugMetadata().IsValid())
		{
			UE_LOG(InkPlusPlus, Error, TEXT("Read count for target (%s - on %s) unknown"), *Container->GetName(), *Container->GetDebugMetadata()->ToString());
		}
		else
		{
			UE_LOG(InkPlusPlus, Error, TEXT("Read count for target (%s) unknown"), *Container->GetName());
		}
		return 0;
	}

	if (Patch.IsValid() && Patch->TryGetVisitCount(Container, count))
		return count;

	FString containerPathStr = Container->GetPath()->ToString();
	int* findResult = VisitCounts.Find(containerPathStr);
	if (findResult != nullptr)
		count = *findResult;

	return count;
}

void Ink::FStoryState::IncrementVisitCountForContainer(TSharedPtr<Ink::FContainer> Container)
{
	if (Patch.IsValid())
	{
		int currentCount = VisitCountForContainer(Container);
		++currentCount;
		Patch->SetVisitCount(Container, currentCount);
		return;
	}

	int count = 0;
	FString containerPathStr = Container->GetPath()->ToString();
	int* findResult = VisitCounts.Find(containerPathStr);
	if (findResult != nullptr)
	{
		count = *findResult;
	}
	++count;
	VisitCounts.Add(containerPathStr, count);
}

void Ink::FStoryState::RecordTurnIndexVisitToContainer(TSharedPtr<Ink::FContainer> Container)
{
	if (Patch.IsValid())
	{
		Patch->SetTurnIndex(Container, CurrentTurnIndex);
		return;
	}

	FString containerPathStr = Container->GetPath()->ToString();
	TurnIndices.Add(containerPathStr, CurrentTurnIndex);
}

int Ink::FStoryState::TurnsSinceForContainer(TSharedPtr<Ink::FContainer> Container)
{
	if (!Container->GetTurnIndexShouldBeCounted())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("TURNS_SINCE() for target (%s - on %s) unknown"), *Container->GetName(), *Container->GetDebugMetadata()->ToString());
	}

	int index = 0;
	if (Patch.IsValid() && Patch->TryGetTurnIndex(Container, index))
	{
		const int turnsSince = CurrentTurnIndex - index;
		return turnsSince;
	}

	FString containerPathStr = Container->GetPath()->ToString();
	int* findResult = TurnIndices.Find(containerPathStr);
	if (findResult != nullptr)
	{
		index = *findResult;
		const int turnsSince = CurrentTurnIndex - index;
		return turnsSince;
	}

	return -1;
}

int Ink::FStoryState::GetCallstackDepth() const
{
	return CallStack()->GetDepth();
}

TArray<TSharedPtr<Ink::FObject>>& Ink::FStoryState::OutputStream() const
{
	return CurrentFlow->GetOutputStream();
}

TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> Ink::FStoryState::CurrentChoices() const
{
	if (CanContinue())
	{
		return MakeShared<TArray<TSharedPtr<Ink::FChoice>>>();
	}
	else
	{
		return CurrentFlow->GetCurrentChoices();
	}
}

TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> Ink::FStoryState::GeneratedChoices() const
{
	return CurrentFlow->GetCurrentChoices();
}

void Ink::FStoryState::ForceEnd()
{
	CallStack().Reset();
	CurrentFlow->GetCurrentChoices()->Empty();
	
	SetCurrentPointer(Ink::FPointer::Null());
	SetPreviousPointer(Ink::FPointer::Null());

	DidSafeExit = true;
}

FString Ink::FStoryState::getCurrentPathString() const
{
	const Ink::FPointer currentPointer = GetCurrentPointer();
	const FString currentPathString = currentPointer.IsNull() ? "" : currentPointer.GetPath()->ToString();
	return currentPathString;
}

Ink::FPointer Ink::FStoryState::GetCurrentPointer() const
{
	return CallStack()->CurrentElement()->GetCurrentPointer();
}

void Ink::FStoryState::SetCurrentPointer(Ink::FPointer InCurrentPointer)
{
	//const FString ptrStr = InCurrentPointer.ToString();
	//UE_LOG(InkPlusPlus, Display, TEXT("%s"), *ptrStr); // Ink Pointer ->  debug
	CallStack()->CurrentElement()->SetCurrentPointer(InCurrentPointer);
}

Ink::FPointer Ink::FStoryState::GetPreviousPointer() const
{
	return CallStack()->GetCurrentThread()->GetPreviousPointer();
}

void Ink::FStoryState::SetPreviousPointer(Ink::FPointer InPreviousPointer)
{
	CallStack()->GetCurrentThread()->SetPreviousPointer(InPreviousPointer);
}

TSharedPtr<Ink::FCallStack> Ink::FStoryState::CallStack() const
{
	return CurrentFlow->GetCallStack();
}

void Ink::FStoryState::AddError(const FString& InMessage, bool InIsWarning)
{
	if (InIsWarning)
		CurrentWarnings.Emplace(InMessage);
	else
		CurrentErrors.Emplace(InMessage);
}

const TArray<FString>& Ink::FStoryState::GetCurrentErrors() const
{
	return CurrentErrors;
}

const TArray<FString>& Ink::FStoryState::GetCurrentWarnings() const
{
	return CurrentWarnings;
}

TSharedPtr<Ink::FVariableState> Ink::FStoryState::GetVariableState()
{
	return VariableState;
}

bool Ink::FStoryState::CanContinue() const
{
	const bool currentPointerIsNull = GetCurrentPointer().IsNull();
	const bool hasError = HasError();
	const bool canContinue = !currentPointerIsNull && !hasError;
	return canContinue;
}

bool Ink::FStoryState::HasError() const
{
	const bool hasError = CurrentErrors.Num() > 0;
	return hasError;
}

bool Ink::FStoryState::HasWarning() const
{
	const bool hasWarning = CurrentWarnings.Num() > 0;
	return hasWarning;
}

FString Ink::FStoryState::GetCurrentText()
{
	if (OutputStreamTextDirty)
	{
		FString newCurrentText;
		for (TSharedPtr<Ink::FObject> outputObj : OutputStream())
		{
			if (!outputObj.IsValid())
				continue;

			TSharedPtr<Ink::FValueString> textObject = FObject::DynamicCastTo<Ink::FValueString>(outputObj);
			if (textObject.IsValid())
			{
				newCurrentText.Append(textObject->GetValue());
			}
		}

		CurrentText = CleanOutputWhitespace(newCurrentText);
		OutputStreamTextDirty = false;
	}
	
	return CurrentText;
}

// Cleans inline whitespace in the following way:
//  - Removes all whitespace from the start and end of line (including just before a \n)
//  - Turns all consecutive space and tab runs into single spaces (HTML style)
FString Ink::FStoryState::CleanOutputWhitespace(const FString& Text)
{
	FString cleanedOutputStr;
	
	int currentWhitespaceStart = -1;
	int startOfLine = 0;

	for (int i = 0; i < Text.Len(); i++) {
		auto c = Text[i];

		const bool isInlineWhitespace = c == ' ' || c == '\t';

		if (isInlineWhitespace && currentWhitespaceStart == -1)
			currentWhitespaceStart = i;

		if (!isInlineWhitespace) {
			if (c != '\n' && currentWhitespaceStart > 0 && currentWhitespaceStart != startOfLine) {
				cleanedOutputStr.Append(" ");
			}
			currentWhitespaceStart = -1;
		}

		if (c == '\n')
			startOfLine = i + 1;

		if (!isInlineWhitespace)
			cleanedOutputStr.AppendChar(c);
	}

	return cleanedOutputStr;
}

TArray<FString>& Ink::FStoryState::GetCurrentTags()
{
	if (OutputStreamTagsDirty)
	{
		CurrentTags.Empty();
		for (TSharedPtr< Ink::FObject> outputObj : OutputStream())
		{
			TSharedPtr<Ink::FTag> tag = FObject::DynamicCastTo<Ink::FTag>(outputObj);
			if (tag.IsValid())
			{
				CurrentTags.Add(tag->GetText());
			}
		}
		OutputStreamTagsDirty = false;
	}

	return CurrentTags;
}

FString Ink::FStoryState::GetCurrentFlowName() const
{
	return CurrentFlow->GetName();
}

TSharedPtr<TMap<FString, TSharedPtr<Ink::FFlow>>> Ink::FStoryState::GetNamedFlows()
{
	return NamedFlows;
}

bool Ink::FStoryState::GetInExpressionEvaluation() const
{
	return CallStack()->CurrentElement()->GetInExpressionEvaluation();
}

void Ink::FStoryState::SetInExpressionEvaluation(bool InExpressionEvaluation)
{
	CallStack()->CurrentElement()->SetInExpressionEvaluation(InExpressionEvaluation);
}

void Ink::FStoryState::SwitchFlow_Internal(const FString& FlowName)
{
	if (!NamedFlows.IsValid())
	{
		NamedFlows = MakeShared<TMap<FString, TSharedPtr<FFlow>>>();
		NamedFlows->Add(DefaultFlowName, CurrentFlow);
	}

	if (FlowName.Equals(CurrentFlow->GetName()))
		return;

	TSharedPtr<FFlow> flow;
	TSharedPtr<FFlow>* findResult = NamedFlows->Find(FlowName);
	if (findResult == nullptr)
	{
		flow = MakeShared<FFlow>(FlowName, Story);
		NamedFlows->Add(FlowName, flow);
	}
	else
	{
		flow = *findResult;
	}

	CurrentFlow = flow;
	VariableState->SetCallStack(CurrentFlow->GetCallStack());

	// Cause text to be regenerated from output stream if necessary
	SetOutputStreamDirty();
}

void Ink::FStoryState::SwitchToDefaultFlow_Internal()
{
	if (NamedFlows.IsValid())
		SwitchFlow_Internal(DefaultFlowName);
}

void Ink::FStoryState::RemoveFlow_Internal(const FString& FlowName)
{
	if (FlowName.Equals(DefaultFlowName))
		return; // Cannot destroy default flow

	// If we're currently in the flow that's being removed, switch back to default
	if (CurrentFlow->GetName() == FlowName) {
		SwitchToDefaultFlow_Internal();
	}

	NamedFlows->Remove(FlowName);
}

TSharedPtr<Ink::FStoryState> Ink::FStoryState::CopyAndStartPatching()
{
	TSharedPtr<Ink::FStoryState> copy = MakeShared<Ink::FStoryState>(Story);
	copy->Patch = MakeShared<FStatePatch>();
	if (Patch != nullptr)
		*copy->Patch = *Patch;

	// Hijack the new default flow to become a copy of our current one
	// If the patch is applied, then this new flow will replace the old one in _namedFlows
	copy->CurrentFlow->SetName(CurrentFlow->GetName());
	copy->CurrentFlow->SetCallStack(*CurrentFlow->GetCallStack());
	copy->CurrentFlow->GetCurrentChoices()->Append(*CurrentFlow->GetCurrentChoices());
	copy->CurrentFlow->GetOutputStream().Append(CurrentFlow->GetOutputStream());
	copy->SetOutputStreamDirty();

	// The copy of the state has its own copy of the named flows dictionary,
	// except with the current flow replaced with the copy above
	// (Assuming we're in multi-flow mode at all. If we're not then
	// the above copy is simply the default flow copy and we're done)
	if (NamedFlows.IsValid()) {
		copy->NamedFlows = MakeShared<TMap<FString, TSharedPtr<FFlow>>>();
		for (const TTuple<FString, TSharedPtr<FFlow>>& flow : *NamedFlows)
		{
			copy->NamedFlows->Add(flow.Key, flow.Value);
		}
		copy->NamedFlows->Add(CurrentFlow->GetName(), copy->CurrentFlow);
	}

	if (HasError()) {
		copy->CurrentErrors = CurrentErrors;
	}
	if (HasWarning()) {
		copy->CurrentWarnings = CurrentWarnings;
	}

	// ref copy - exactly the same variables state!
	// we're expecting not to read it only while in patch mode
	// (though the callstack will be modified)
	copy->VariableState = VariableState;
	copy->VariableState->SetCallStack(copy->CallStack());
	copy->VariableState->SetStatePatch(copy->Patch);

	copy->EvaluationStack = EvaluationStack;

	if (!DivertedPointer.IsNull())
		copy->DivertedPointer = DivertedPointer;

	copy->SetPreviousPointer(GetPreviousPointer());

	// visit counts and turn indicies will be read only, not modified
	// while in patch mode
	copy->VisitCounts = VisitCounts;
	copy->TurnIndices = TurnIndices;

	copy->CurrentTurnIndex = CurrentTurnIndex;
	copy->StorySeed = StorySeed;
	copy->PreviousRandom = PreviousRandom;

	copy->DidSafeExit = DidSafeExit;

	return copy;
}

void Ink::FStoryState::RestoreAfterPatch()
{
	// VariablesState was being borrowed by the patched
	// state, so restore it with our own callstack.
	// _patch will be null normally, but if you're in the
	// middle of a save, it may contain a _patch for save purpsoes.
	VariableState->SetCallStack(CallStack());
	VariableState->SetStatePatch(Patch); // usually null
}

void Ink::FStoryState::ApplyAnyPatch()
{
	if (!Patch.IsValid())
		return;

	VariableState->ApplyPatch();


	for (const TTuple<TSharedPtr<FContainer>, int>& pathToCount : Patch->GetVisitCounts())
	{
		VisitCounts.Add(pathToCount.Key->GetPath()->ToString(), pathToCount.Value);
	}

	for (const TTuple<TSharedPtr<FContainer>, int>& pathToCount : Patch->GetTurnIndices())
	{
		TurnIndices.Add(pathToCount.Key->GetPath()->ToString(), pathToCount.Value);
	}

	Patch.Reset();
}

void Ink::FStoryState::WriteJson(TJsonWriter<>* Writer) const
{
	Writer->WriteObjectStart();

	// Flows
	Writer->WriteIdentifierPrefix("flows");
	Writer->WriteObjectStart();

	if (NamedFlows.IsValid()) {
		// Multi-flow
		for (const TTuple<FString, TSharedPtr<FFlow>>& namedFlow : *NamedFlows)
		{
			Writer->WriteIdentifierPrefix( namedFlow.Key );
			namedFlow.Value->WriteJSON(Writer);
		}
	}
	else {
		// Single flow
		Writer->WriteIdentifierPrefix( CurrentFlow->GetName() );
		CurrentFlow->WriteJSON(Writer);
	}

	Writer->WriteObjectEnd(); // end of flows

	Writer->WriteValue(TEXT("currentFlowName"), CurrentFlow->GetName());

	Writer->WriteIdentifierPrefix("variablesState");
	VariableState->WriteJson(Writer);

	Writer->WriteIdentifierPrefix("evalStack");
	Writer->WriteArrayStart();
	for (TSharedPtr<Ink::FObject> obj : EvaluationStack)
	{
		Ink::FJsonSerialisation::WriteRuntimeObject(Writer, obj);
	}
	Writer->WriteArrayEnd();

	if (!DivertedPointer.IsNull())
		WriteProperty(Writer, "currentDivertTarget", DivertedPointer.GetPath()->GetComponentsString());

	Writer->WriteIdentifierPrefix("visitCounts");
	Writer->WriteObjectStart();
	for (const TTuple<FString, int>& pair: VisitCounts)
	{
		WriteProperty(Writer, pair.Key, pair.Value);
	}
	Writer->WriteObjectEnd();

	Writer->WriteIdentifierPrefix("turnIndices");
	Writer->WriteObjectStart();
	for (const TTuple<FString, int>& pair : TurnIndices)
	{
		WriteProperty(Writer, pair.Key, pair.Value);
	}
	Writer->WriteObjectEnd();

	Writer->WriteValue(TEXT("turnIdx"), CurrentTurnIndex);
	Writer->WriteValue(TEXT("storySeed"), StorySeed);
	Writer->WriteValue(TEXT("previousRandom"), PreviousRandom);
	Writer->WriteValue(TEXT("inkSaveVersion"), InkSaveStateVersion);

	// Not using this right now, but could do in future.
	Writer->WriteValue(TEXT("inkFormatVersion"), Story->inkVersionCurrent );
	Writer->WriteObjectEnd();
}

template<typename T>
void Ink::FStoryState::WriteProperty(TJsonWriter<>* Writer, const FString& PropertyName, T PropertyValue) const
{
	Writer->WriteIdentifierPrefix(PropertyName);
	Writer->WriteObjectStart();
	Writer->WriteValue(PropertyValue);
	Writer->WriteObjectEnd();
}

void Ink::FStoryState::ResetErrors()
{
	CurrentErrors.Reset();
	CurrentWarnings.Reset();
}

void Ink::FStoryState::ResetOutput()
{
	OutputStream().Empty();
	SetOutputStreamDirty();
}

void Ink::FStoryState::ResetOutput(TArray<TSharedPtr<Ink::FObject>>& objs)
{
	TArray<TSharedPtr<Ink::FObject>>& outputStream = OutputStream();
	outputStream = objs;
	SetOutputStreamDirty();
}

// Push to output stream, but split out newlines in text for consistency
// in dealing with them later.
void Ink::FStoryState::PushToOutputStream(TSharedPtr<Ink::FObject> RuntimeObject)
{
	TSharedPtr<Ink::FValueString> text = FObject::DynamicCastTo<FValueString>(RuntimeObject);

	if (text.IsValid()) 
	{
		TArray<TSharedPtr<Ink::FValueString>> listText = TrySplittingHeadTailWhitespace(text);
		if (listText.Num() > 0)
		{
			for (TSharedPtr<Ink::FValueString> textObj : listText)
			{
				PushToOutputStreamIndividual(textObj);
			}
			SetOutputStreamDirty();
			return;
		}
	}
	PushToOutputStreamIndividual(RuntimeObject);
	SetOutputStreamDirty();
}

void Ink::FStoryState::PopFromOutputStream(int Count)
{
	OutputStream().RemoveAt(OutputStream().Num() - Count, Count);
	SetOutputStreamDirty();
}

TArray<TSharedPtr<Ink::FValueString>> Ink::FStoryState::TrySplittingHeadTailWhitespace(TSharedPtr<Ink::FValueString> Single)
{
	TArray<TSharedPtr<Ink::FValueString>> listTexts;
	FString str = Single->GetValue();

	int headFirstNewlineIdx = -1;
	int headLastNewlineIdx = -1;
	for (int i = 0; i < str.Len(); i++) {
		char c = str[i];
		if (c == '\n') {
			if (headFirstNewlineIdx == -1)
				headFirstNewlineIdx = i;
			headLastNewlineIdx = i;
		}
		else if (c == ' ' || c == '\t')
			continue;
		else
			break;
	}

	int tailLastNewlineIdx = -1;
	int tailFirstNewlineIdx = -1;
	for (int i = str.Len() - 1; i >= 0; i--) {
		char c = str[i];
		if (c == '\n') {
			if (tailLastNewlineIdx == -1)
				tailLastNewlineIdx = i;
			tailFirstNewlineIdx = i;
		}
		else if (c == ' ' || c == '\t')
			continue;
		else
			break;
	}

	// No splitting to be done?
	if (headFirstNewlineIdx == -1 && tailLastNewlineIdx == -1)
		return listTexts;


	int innerStrStart = 0;
	int innerStrEnd = str.Len();

	if (headFirstNewlineIdx != -1) {
		if (headFirstNewlineIdx > 0) {
			TSharedPtr<Ink::FValueString> leadingSpaces = MakeShared<Ink::FValueString>(str.Mid(0, headFirstNewlineIdx));
			listTexts.Add(leadingSpaces);
		}
		listTexts.Add(MakeShared<Ink::FValueString>("\n"));
		innerStrStart = headLastNewlineIdx + 1;
	}

	if (tailLastNewlineIdx != -1) {
		innerStrEnd = tailFirstNewlineIdx;
	}

	if (innerStrEnd > innerStrStart) {
		FString innerStrText = str.Mid(innerStrStart, innerStrEnd - innerStrStart);
		listTexts.Add(MakeShared<Ink::FValueString>(innerStrText));
	}

	if (tailLastNewlineIdx != -1 && tailFirstNewlineIdx > headLastNewlineIdx) {
		listTexts.Add(MakeShared<Ink::FValueString>("\n"));
		if (tailLastNewlineIdx < str.Len() - 1) {
			int numSpaces = (str.Len() - tailLastNewlineIdx) - 1;
			TSharedPtr< Ink::FValueString> trailingSpaces = MakeShared<Ink::FValueString>(str.Mid(tailLastNewlineIdx + 1, numSpaces));
			listTexts.Add(trailingSpaces);
		}
	}

	return listTexts;
}

void Ink::FStoryState::PushToOutputStreamIndividual(TSharedPtr<Ink::FObject> RuntimeObject)
{
	TSharedPtr<Ink::FGlue> glue = FObject::DynamicCastTo<Ink::FGlue>(RuntimeObject);
	TSharedPtr<Ink::FValueString> text = FObject::DynamicCastTo<Ink::FValueString>(RuntimeObject);

	bool includeInOutput = true;

	// New glue, so chomp away any whitespace from the end of the stream
	if (glue.IsValid()) {
		TrimNewlinesFromOutputStream();
		includeInOutput = true;
	}

	// New text: do we really want to append it, if it's whitespace?
	// Two different reasons for whitespace to be thrown away:
	//   - Function start/end trimming
	//   - User defined glue: <>
	// We also need to know when to stop trimming, when there's non-whitespace.
	else if (text.IsValid()) {

		// Where does the current function call begin?
		int functionTrimIndex = -1;
		auto currEl = CallStack()->CurrentElement();
		if (currEl->GetType() == Ink::EPushPopType::Function) {
			functionTrimIndex = currEl->FunctionStartInOutputStream;
		}

		// Do 2 things:
		//  - Find latest glue
		//  - Check whether we're in the middle of string evaluation
		// If we're in string eval within the current function, we
		// don't want to trim back further than the length of the current string.
		int glueTrimIndex = -1;
		for (int i = OutputStream().Num() - 1; i >= 0; i--) {
			TSharedPtr<Ink::FObject> o = OutputStream()[i];
			TSharedPtr<Ink::FControlCommand> c = FObject::DynamicCastTo< Ink::FControlCommand>(o);
			TSharedPtr<Ink::FGlue> g = FObject::DynamicCastTo<Ink::FGlue>(o);

			// Find latest glue
			if (g.IsValid()) {
				glueTrimIndex = i;
				break;
			}

			// Don't function-trim past the start of a string evaluation section
			else if (c.IsValid() && c->GetCommandType() == Ink::ECommandType::BeginString) {
				if (i >= functionTrimIndex) {
					functionTrimIndex = -1;
				}
				break;
			}
		}

		// Where is the most agressive (earliest) trim point?
		int trimIndex = -1;
		if (glueTrimIndex != -1 && functionTrimIndex != -1)
			trimIndex = FGenericPlatformMath::Min(functionTrimIndex, glueTrimIndex);
		else if (glueTrimIndex != -1)
			trimIndex = glueTrimIndex;
		else
			trimIndex = functionTrimIndex;

		// So, are we trimming then?
		if (trimIndex != -1) 
		{

			// While trimming, we want to throw all newlines away,
			// whether due to glue or the start of a function
			if (text->IsNewLine()) 
			{
				includeInOutput = false;
			}

			// Able to completely reset when normal text is pushed
			else if (text->IsNonWhitespace()) 
			{

				if (glueTrimIndex > -1)
					RemoveExistingGlue();

				// Tell all functions in callstack that we have seen proper text,
				// so trimming whitespace at the start is done.
				if (functionTrimIndex > -1) 
				{
					auto callstackElements = CallStack()->GetElements();
					for (int i = callstackElements.Num() - 1; i >= 0; i--) 
					{
						auto el = callstackElements[i];
						if (el->GetType() == EPushPopType::Function) 
							el->FunctionStartInOutputStream = -1;
						else 
							break;
					}
				}
			}
		}

		// De-duplicate newlines, and don't ever lead with a newline
		else if (text->IsNewLine()) {
			if (OutputStreamEndsInNewline() || !OutputStreamContainsContent())
				includeInOutput = false;
		}
	}

	if (includeInOutput) {
		OutputStream().Add(RuntimeObject);
		SetOutputStreamDirty();
	}
}

void Ink::FStoryState::TrimNewlinesFromOutputStream()
{
	int removeWhitespaceFrom = -1;

	// Work back from the end, and try to find the point where
	// we need to start removing content.
	//  - Simply work backwards to find the first newline in a string of whitespace
	// e.g. This is the content   \n   \n\n
	//                            ^---------^ whitespace to remove
	//                        ^--- first while loop stops here
	int i = OutputStream().Num() - 1;
	while (i >= 0) 
	{
		TSharedPtr<Ink::FObject> obj = OutputStream()[i];
		TSharedPtr<Ink::FControlCommand> cmd = FObject::DynamicCastTo<Ink::FControlCommand>(obj);
		TSharedPtr<Ink::FValueString> txt = FObject::DynamicCastTo<Ink::FValueString>(obj);

		if (cmd.IsValid() || (txt.IsValid() && txt->IsNonWhitespace())) 
		{
			break;
		}
		else if (txt.IsValid() && txt->IsNewLine()) 
		{
			removeWhitespaceFrom = i;
		}
		i--;
	}

	// Remove the whitespace
	if (removeWhitespaceFrom >= 0)
	{
		i = removeWhitespaceFrom;
		while (i < OutputStream().Num()) 
		{
			TSharedPtr<Ink::FValueString> text = FObject::DynamicCastTo<Ink::FValueString>(OutputStream()[i]);
			if (text.IsValid())
			{
				OutputStream().RemoveAt(i);
			}
			else
			{
				i++;
			}
		}
	}

	SetOutputStreamDirty();
}

void Ink::FStoryState::RemoveExistingGlue()
{
	for (int i = OutputStream().Num() - 1; i >= 0; i--) 
	{
		TSharedPtr<Ink::FGlue> c = FObject::DynamicCastTo<Ink::FGlue>(OutputStream()[i]);

		if (c.IsValid()) 
		{
			OutputStream().RemoveAt(i);
		}
		else if (FObject::DynamicCastTo<FControlCommand>(c).IsValid())
		{ // e.g. BeginString
			break;
		}
	}

	SetOutputStreamDirty();
}

bool Ink::FStoryState::OutputStreamEndsInNewline() const
{
	if (OutputStream().Num() > 0) {

		for (int i = OutputStream().Num() - 1; i >= 0; i--) 
		{
			const TSharedPtr<FObject> obj = OutputStream()[i];
			if (FObject::DynamicCastTo<FControlCommand>(obj)) // e.g. BeginString
				break;

			const TSharedPtr<FValueString> text = FObject::DynamicCastTo<FValueString>(obj);
			if (text) {
				if (text->IsNewLine())
					return true;
				else if (text->IsNonWhitespace())
					break;
			}
		}
	}

	return false;
}

bool Ink::FStoryState::OutputStreamContainsContent() const
{
	for (TSharedPtr<Ink::FObject> content : OutputStream())
	{
		TSharedPtr<FValueString> castedContent = FObject::DynamicCastTo<FValueString>(content);
		if (castedContent.IsValid())
			return true;
	}
	return false;
}

bool Ink::FStoryState::InStringEvaluation() const
{
	for (int i = OutputStream().Num() - 1; i >= 0; i--) {
		auto obj = OutputStream()[i];
		const TSharedPtr<FControlCommand> cmd = FObject::DynamicCastTo<FControlCommand>(obj);
		if (cmd.IsValid() && cmd->GetCommandType() == Ink::ECommandType::BeginString) {
			return true;
		}
	}

	return false;
}

void Ink::FStoryState::PushEvaluationStack(TSharedPtr<Ink::FObject> obj)
{
	// Include metadata about the origin List for list values when
	// they're used, so that lower level functions can make use
	// of the origin list to get related items, or make comparisons
	// with the integer values etc.
	TSharedPtr<FValueList> listValue = FObject::DynamicCastTo<FValueList>(obj);
	if (listValue.IsValid()) {

		// Update origin when list is has something to indicate the list origin
		FInkList& rawList = listValue->GetValue();
		if (rawList.GetOriginNames().IsValid())
		{
			if (!rawList.GetOrigins().IsValid())
				rawList.GetOrigins() = MakeShared<TArray<TSharedPtr<Ink::FListDefinition>>>();

			rawList.GetOrigins()->Reset();

			for(FString n : *rawList.GetOriginNames()) {
				TSharedPtr<FListDefinition> def;
				Story->GetListDefinitions()->TryListGetDefinition(n, def);
				if (!rawList.GetOrigins()->Contains(def))
					rawList.GetOrigins()->Add(def);
			}
		}
	}

	EvaluationStack.Add(obj);
}

TSharedPtr<Ink::FObject> Ink::FStoryState::PopEvaluationStack()
{
	if (EvaluationStack.Num() > 0)
		return EvaluationStack.Pop();
	else
		return nullptr;
}

TArray<TSharedPtr<Ink::FObject>> Ink::FStoryState::PopEvaluationStack(int NumberOfObjects)
{
	TArray<TSharedPtr<Ink::FObject>> popped;
	popped.Reserve(NumberOfObjects);
	for (int i = 0; i < NumberOfObjects; ++i)
	{
		popped.Add(EvaluationStack.Pop());
	}
	Algo::Reverse(popped);
	return popped;
}

TSharedPtr<Ink::FObject> Ink::FStoryState::PeekEvaluationStack()
{
	return EvaluationStack.Last();
}

TArray<TSharedPtr<Ink::FObject>>& Ink::FStoryState::GetEvaluationStack()
{
	return EvaluationStack;
}

Ink::FPointer Ink::FStoryState::GetDivertedPointer()
{
	return DivertedPointer;
}

void Ink::FStoryState::SetDivertedPointer(Ink::FPointer InDivertedPointer)
{
	DivertedPointer = InDivertedPointer;
}

int Ink::FStoryState::GetCurrentTurnIndex() const
{
	return CurrentTurnIndex;
}

int Ink::FStoryState::GetStorySeed() const
{
	return StorySeed;
}

void Ink::FStoryState::SetStorySeed(int InStorySeed)
{
	StorySeed = InStorySeed;
}

int Ink::FStoryState::GetPreviousRandom() const
{
	return PreviousRandom;
}

void Ink::FStoryState::SetPreviousRandom(int InPreviousRandom)
{
	PreviousRandom = InPreviousRandom;
}

bool Ink::FStoryState::GetDidSafeExit() const
{
	return DidSafeExit;
}

void Ink::FStoryState::SetDidSafeExit(bool InDidSafeExit)
{
	DidSafeExit = InDidSafeExit;
}

Ink::FStory* Ink::FStoryState::GetStory() const
{
	return Story;
}

void Ink::FStoryState::SetStory(Ink::FStory* InStory)
{
	Story = InStory;
}

void Ink::FStoryState::SetOutputStreamDirty()
{
	OutputStreamTextDirty = true;
	OutputStreamTagsDirty = true;
}

// Add the end of a function call, trim any whitespace from the end.
// We always trim the start and end of the text that a function produces.
// The start whitespace is discard as it is generated, and the end
// whitespace is trimmed in one go here when we pop the function.
void Ink::FStoryState::TrimWhitespaceFromFunctionEnd()
{
	if (CallStack()->CurrentElement()->GetType() != Ink::EPushPopType::Function)
		return;

	int32 functionStartPoint = CallStack()->CurrentElement()->FunctionStartInOutputStream;

	// If the start point has become -1, it means that some non-whitespace
	// text has been pushed, so it's safe to go as far back as we're able.
	if (functionStartPoint == -1) {
		functionStartPoint = 0;
	}

	// Trim whitespace from END of function call
	for (int i = OutputStream().Num() - 1; i >= functionStartPoint; i--) {
		TSharedPtr<Ink::FObject> obj = OutputStream()[i];
		TSharedPtr<Ink::FValueString> txt = FObject::DynamicCastTo<FValueString>(obj);
		TSharedPtr<Ink::FControlCommand> cmd = FObject::DynamicCastTo<FControlCommand>(obj);
		if (!txt.IsValid())
			continue;

		if (cmd.IsValid())
			break;

		if (txt->IsNewLine() || txt->IsInlineWhitespace()) 
		{
			OutputStream().RemoveAt(i);
			SetOutputStreamDirty();
		}
		else
			break;
	}
}

void Ink::FStoryState::PopCallstack(Ink::EPushPopType PopType)
{
	// Add the end of a function call, trim any whitespace from the end.
	if (CallStack()->CurrentElement()->GetType() == EPushPopType::Function)
		TrimWhitespaceFromFunctionEnd();

	CallStack()->Pop(PopType);
}

void Ink::FStoryState::SetChosenPath(TSharedPtr<FPath> Path, bool incrementingTurnIndex)
{
	// Changing direction, assume we need to clear current set of choices
	CurrentFlow->GetCurrentChoices()->Empty();

	FPointer newPointer = Story->PointerAtPath(Path);
	if (!newPointer.IsNull() && newPointer.GetIndex() == -1)
		newPointer.SetIndex(0);

	SetCurrentPointer(newPointer);

	if (incrementingTurnIndex)
		++CurrentTurnIndex;
}

void Ink::FStoryState::StartFunctionEvaluationFromGame(TSharedPtr<Ink::FContainer> FuncContainer, const TArray<TSharedPtr<Ink::FValueType>>& Arguments)
{
	CallStack()->Push(Ink::EPushPopType::FunctionEvaluationFromGame, EvaluationStack.Num());
	CallStack()->CurrentElement()->SetCurrentPointer(FPointer::StartOf(FuncContainer));

	PassArgumentsToEvaluationStack(Arguments);
}

void Ink::FStoryState::PassArgumentsToEvaluationStack(const TArray<TSharedPtr<Ink::FValueType>>& Arguments)
{
	// Pass arguments onto the evaluation stack
	for (int i = 0; i < Arguments.Num(); i++) {
		// ink arguments when calling EvaluateFunction / ChoosePathStringWithParameters must be int, float, string or InkList
		TSharedPtr<Ink::FValueType> argument = Arguments[i];
		PushEvaluationStack(FValue::Create(*argument));
	}
}

bool Ink::FStoryState::TryExitFunctionEvaluationFromGame()
{
	if (CallStack()->CurrentElement()->GetType() == Ink::EPushPopType::FunctionEvaluationFromGame)
	{
		SetCurrentPointer(FPointer::Null());
		DidSafeExit = true;
		return true;
	}
	return false;
}

TSharedPtr<Ink::FValueType> Ink::FStoryState::CompleteFunctionEvaluationFromGame()
{
	if (CallStack()->CurrentElement()->GetType() != Ink::EPushPopType::FunctionEvaluationFromGame)
	{
		// throw new Exception("Expected external function evaluation to be complete. Stack trace: " + callStack.callStackTrace);
		return nullptr;
	}

	int originalEvaluationStackHeight = CallStack()->CurrentElement()->EvaluationStackHeightWhenPushed;

	// Do we have a returned value?
	// Potentially pop multiple values off the stack, in case we need
	// to clean up after ourselves (e.g. caller of EvaluateFunction may 
	// have passed too many arguments, and we currently have no way to check for that)
	TSharedPtr<Ink::FObject> returnedObj;
	while (EvaluationStack.Num() > originalEvaluationStackHeight) {
		TSharedPtr<Ink::FObject> poppedObj = PopEvaluationStack();
		if (!returnedObj.IsValid())
			returnedObj = poppedObj;
	}

	// Finally, pop the external function evaluation
	PopCallstack(EPushPopType::FunctionEvaluationFromGame);

	// What did we get back?
	if (returnedObj.IsValid()) {
		if (FObject::DynamicCastTo<Ink::FVoid>(returnedObj))
			return nullptr;

		// Some kind of value, if not void
		const TSharedPtr<Ink::FValue> returnVal = FObject::DynamicCastTo<Ink::FValue>(returnedObj);
		if (!returnVal.IsValid())
			return nullptr;

		// DivertTargets get returned as the string of components
		// (rather than a Path, which isn't public)
		if (returnVal->GetType() == Ink::EValueType::DivertTarget) {
			return MakeShared<Ink::FValueType>(returnVal->GetValueObject().GetSubtype<TSharedPtr<FPath>>()->ToString());
		}

		// Other types can just have their exact object type:
		// int, float, string. VariablePointers get returned as strings.
		return MakeShared<FValueType>(returnVal->GetValueObject());
	}

	return nullptr;
}
