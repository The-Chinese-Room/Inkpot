#include "Ink/Story.h"

#include "Ink/CallStack.h"
#include "Ink/Path.h"
#include "Ink/Container.h"
#include "Ink/SearchResult.h"
#include "Ink/Choice.h"
#include "Ink/DebugMetadata.h"
#include "Ink/Pointer.h"
#include "Ink/StoryState.h"
#include "Utility/InkPlusPlusLog.h"
#include "Serialization/JsonSerializer.h"
#include "Ink/JsonSerialisation.h"
#include "Ink/ChoicePoint.h"
#include "Ink/ControlCommand.h"
#include "Ink/Void.h"
#include "Ink/Divert.h"
#include "Ink/VariableReference.h"
#include "Ink/NativeFunctionCall.h"
#include "Ink/Tag.h"


Ink::FStory::FStory(const FString& jsonString)
{
	const TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonString);
	TSharedPtr<FJsonObject> rootObject = MakeShared<FJsonObject>();

	if (!FJsonSerializer::Deserialize(reader, rootObject))
	{
		Error("Ink:FStory : Failed to deserialize the JSON string");
		return;
	}

	auto versionObj = rootObject->TryGetField("inkVersion");
	if (!versionObj.IsValid())
	{
		Error("ink version number not found. Are you sure it's a valid .ink.json file?");
		return;
	}

	const int32 formatFromFile = static_cast<int32>(versionObj->AsNumber());
	if (formatFromFile > inkVersionCurrent) {
		Error("Version of ink used to build story was newer than the current version of the engine");
		return;
	}
	else if (formatFromFile < inkVersionMinimumCompatible) {
		Error("Version of ink used to build story is too old to be loaded by this version of the engine");
		return;
	}
	else if (formatFromFile != inkVersionCurrent) {
		UE_LOG(InkPlusPlus, Warning, TEXT("WARNING: Version of ink used to build story doesn't match current version of engine. Non-critical, but recommend synchronising."));
	}

	TSharedPtr<FJsonValue> rootToken = rootObject->TryGetField("root");
	if (!rootToken.IsValid())
	{
		Error("Root node for ink not found. Are you sure it's a valid .ink.json file?");
		return;
	}

	if (TSharedPtr<FJsonValue> listDefsObject = rootObject->TryGetField("listDefs"))
	{
		ListDefinitions = Ink::FJsonSerialisation::JsonTokenToListDefinitions(*listDefsObject);
	}

	TSharedPtr<Ink::FObject> rootTokenObject = Ink::FJsonSerialisation::JsonTokenToRuntimeObject(*rootToken);
	TSharedPtr<FContainer> rootTokenContainer = FObject::DynamicCastTo<Ink::FContainer>(rootTokenObject);
	if (rootTokenContainer.IsValid())
	{
		MainContentContainer = rootTokenContainer;
	}
	else
	{
		UE_LOG(InkPlusPlus, Warning, TEXT("Error: Story - Failed to obtain valid MainContentContainer."));
	}

	ResetState();
}

FString Ink::FStory::ToJson() const
{
	FString jsonString;
	const TSharedPtr<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&jsonString);
	ToJson(writer.Get());
	return jsonString;
}

void Ink::FStory::ToJson(FArchive* const Stream) const
{
	const TSharedPtr<TJsonWriter<>> writer = TJsonWriter<>::Create(Stream);
	ToJson(writer.Get());
}

void Ink::FStory::ToJson(TJsonWriter<>* InJSONWriter) const
{
	InJSONWriter->WriteObjectStart();

	InJSONWriter->WriteObjectStart("inkVersion");
	InJSONWriter->WriteValue(inkVersionCurrent);
	InJSONWriter->WriteObjectEnd();

	InJSONWriter->WriteObjectStart("root");
	FJsonSerialisation::WriteRuntimeContainer(InJSONWriter, *MainContentContainer);
	InJSONWriter->WriteObjectEnd();

	if (ListDefinitions.IsValid())
	{
		InJSONWriter->WriteObjectStart("listDefs");

		for (const TSharedPtr<FListDefinition>& def : ListDefinitions->GetLists())
		{
			InJSONWriter->WriteObjectStart(def->GetName());

			for (auto itemToVal : def->GetItems())
			{
				FInkListItem item = itemToVal.Key;
				int val = itemToVal.Value;
				InJSONWriter->WriteObjectStart(item.ItemName);
				InJSONWriter->WriteValue(val);
				InJSONWriter->WriteObjectEnd();
			}

			InJSONWriter->WriteObjectEnd();
		}

		InJSONWriter->WriteObjectEnd();
	}

	InJSONWriter->WriteObjectEnd();
}

void Ink::FStory::ResetState()
{
	IfAsyncWeCant("ResetState");

	State = MakeShared<FStoryState>(this);
	State->GetVariableState()->VariableChanged.AddRaw(this, &FStory::OnVariableStateDidChangeEvent);

	ResetGlobals();
}

void Ink::FStory::ResetErrors()
{
	State->ResetErrors();
}

void Ink::FStory::ResetCallstack()
{
	IfAsyncWeCant("ResetCallstack");
	State->ForceEnd();
}

void Ink::FStory::ResetGlobals()
{
	const FString globalDecl("global decl");
	if (MainContentContainer->GetNamedContent()->Contains(globalDecl))
	{
		FPointer originalPointer = State->GetCurrentPointer();
		ChoosePath(MakeShared<FPath>(globalDecl), false);

		// Continue, but without validating external bindings,
		// since we may be doing this reset at initialisation time.
		ContinueInternal();
		
		State->SetCurrentPointer(originalPointer);
	}

	State->GetVariableState()->SnapshotDefaultGlobals();
}

void Ink::FStory::SwitchFlow(const FString& FlowName) const
{
	IfAsyncWeCant("switch flow");
	if (_asyncSaving)
	{
		Error("Story is already in background saving mode, can't switch flow to " + FlowName);
		return;
	}

	State->SwitchFlow_Internal(FlowName);
}

void Ink::FStory::RemoveFlow(const FString& FlowName) const
{
	State->RemoveFlow_Internal(FlowName);
}

void Ink::FStory::SwitchToDefaultFlow() const
{
	State->SwitchToDefaultFlow_Internal();
}

/// <summary>
/// Continue the story for one line of content, if possible.
/// If you're not sure if there's more content available, for example if you
/// want to check whether you're at a choice point or at the end of the story,
/// you should call <c>canContinue</c> before calling this function.
/// </summary>
/// <returns>The line of text content.</returns>
FString Ink::FStory::Continue()
{
	constexpr float millisecondsLimitAsync = 2.0f;
	ContinueAsync(millisecondsLimitAsync);
	return GetCurrentText();
}

/// <summary>
/// Check whether more content is available if you were to call <c>Continue()</c> - i.e.
/// are we mid story rather than at a choice point or at the end.
/// </summary>
/// <value><c>true</c> if it's possible to call <c>Continue()</c>.</value>
bool Ink::FStory::CanContinue() const
{
	return State->CanContinue();
}

/// <summary>
/// If ContinueAsync was called (with milliseconds limit > 0) then this property
/// will return false if the ink evaluation isn't yet finished, and you need to call 
/// it again in order for the Continue to fully complete.
/// </summary>
bool Ink::FStory::AsyncContinueComplete() const
{
	return !_asyncContinueActive;
}

/// <summary>
/// An "asnychronous" version of Continue that only partially evaluates the ink,
/// with a budget of a certain time limit. It will exit ink evaluation early if
/// the evaluation isn't complete within the time limit, with the
/// asyncContinueComplete property being false.
/// This is useful if ink evaluation takes a long time, and you want to distribute
/// it over multiple game frames for smoother animation.
/// If you pass a limit of zero, then it will fully evaluate the ink in the same
/// way as calling Continue (and in fact, this exactly what Continue does internally).
/// </summary>
void Ink::FStory::ContinueAsync(float MillisecondsLimitAsync)
{
	//if (!_hasValidatedExternals) // EXTERNAL FUNCTIONS FUNCTIONALITY
	//	ValidateExternalBindings();

	ContinueInternal(MillisecondsLimitAsync);
}

void Ink::FStory::ContinueInternal(float MillisecondsLimitAsync)
{
	const bool isAsyncTimeLimited = MillisecondsLimitAsync > 0;

	_recursiveContinueCount++;

	// Doing either:
	//  - full run through non-async (so not active and don't want to be)
	//  - Starting async run-through
	if (!_asyncContinueActive) {
		_asyncContinueActive = isAsyncTimeLimited;

		if (!CanContinue()) {
			Error("Can't continue - should check canContinue before calling Continue");
			return;
		}

		State->SetDidSafeExit(false);
		State->ResetOutput();

		// It's possible for ink to call game to call ink to call game etc
		// In this case, we only want to batch observe variable changes
		// for the outermost call.
		if (_recursiveContinueCount == 1)
			State->GetVariableState()->SetBatchObservingVariableChanges(true);
	}

	// Start timing
	const FDateTime startTime = FDateTime::UtcNow();

	bool outputStreamEndsInNewline = false;
	_sawLookaheadUnsafeFunctionAfterNewline = false;
	int continueCount = 0;
	do {
		outputStreamEndsInNewline = ContinueSingleStep();
		++continueCount;
		//if (StoryException.hasException) // removed because StoryException being static results in error states incorrectly hanging around. Needs refactor
		//{
		//	const bool isWarning = false;
		//	AddError("STORY EXCEPTION: " + StoryException.message, isWarning, StoryException.useEndLineNumber);
		//	StoryException.Reset();
		//	break;
		//}

		if (outputStreamEndsInNewline)
			break;

		// Run out of async time?
		const FTimespan elapsedTime = FDateTime::UtcNow() - startTime;
		const double elapsedMilliseconds = elapsedTime.GetTotalMilliseconds();
		if (_asyncContinueActive && elapsedMilliseconds > MillisecondsLimitAsync) {
			UE_LOG(InkPlusPlus, Error, TEXT("Aborting Continue: ink did not complete the request after %sms (%sms limit). There were %s ContinueSingleStep iterations; a large number of iterations indicates a possible infinite loop in the ink story"), *LexToSanitizedString(elapsedMilliseconds), *LexToSanitizedString(MillisecondsLimitAsync), *FString::FormatAsNumber(continueCount));
			break;
		}
	} while (CanContinue());


	// 4 outcomes:
	//  - got newline (so finished this line of text)
	//  - can't continue (e.g. choices or ending)
	//  - ran out of time during evaluation
	//  - error
	//
	// Successfully finished evaluation in time (or in error)
	if (outputStreamEndsInNewline || !CanContinue()) {

		// Need to rewind, due to evaluating further than we should?
		if (_stateSnapshotAtLastNewline.IsValid()) {
			RestoreStateSnapshot();
		}

		// Finished a section of content / reached a choice point?
		if (!CanContinue()) {
			if (State->CallStack()->CanPopThread())
				AddError("Thread available to pop, threads should always be flat by the end of evaluation?");

			if (State->GeneratedChoices()->Num() == 0 && !State->GetDidSafeExit() && TemporaryEvaluationContainer == nullptr) {
				if (State->CallStack()->CanPop(EPushPopType::Tunnel))
					AddError("unexpectedly reached end of content. Do you need a '->->' to return from a tunnel?");
				else if (State->CallStack()->CanPop(EPushPopType::Function))
					AddError("unexpectedly reached end of content. Do you need a '~ return'?");
				else if (!State->CallStack()->CanPop())
					AddError("ran out of content. Do you need a '-> DONE' or '-> END'?");
				else
					AddError("unexpectedly reached end of content for unknown reason. Please debug compiler!");
			}
		}

		State->SetDidSafeExit(false);
		_sawLookaheadUnsafeFunctionAfterNewline = false;

		if (_recursiveContinueCount == 1)
			State->GetVariableState()->SetBatchObservingVariableChanges(false);

		_asyncContinueActive = false;

		if(TemporaryEvaluationContainer == nullptr)
			OnDidContinueEvent.Broadcast();
	}

	_recursiveContinueCount--;

	// Report any errors that occured during evaluation.
	// This may either have been StoryExceptions that were thrown
	// and caught during evaluation, or directly added with AddError.
	if (State->HasError() || State->HasWarning()) {
		if (OnErrorEvent.IsBound())
		{
			if (State->HasError()) {
				for (const FString& err : State->GetCurrentErrors())
				{
					OnErrorEvent.Broadcast(err, EErrorType::Error);
				}
			}
			if (State->HasWarning()) {
				for (const FString& err : State->GetCurrentWarnings())
				{
					OnErrorEvent.Broadcast(err, EErrorType::Warning);
				}
			}
			ResetErrors();
		}

		// Throw an exception since there's no error handler
		else {
			FString errorText;
			//errorText.Append("Ink had ");
			//if (State->HasError()) {
			//	errorText.Append(FString::FormatAsNumber(State->GetCurrentErrors().Num()));
			//	errorText.Append(State->GetCurrentErrors().Num() == 1 ? " error" : " errors");
			//	if (State->HasWarning())
			//		errorText.Append(" and ");
			//}
			//if (State->HasWarning()) {
			//	errorText.Append(FString::FormatAsNumber(State->GetCurrentWarnings().Num()));
			//	errorText.Append(State->GetCurrentWarnings().Num() == 1 ? " warning" : " warnings");
			//}
			//errorText.Append(". It is strongly suggested that you assign an error handler to story.onError.\n");
			if (State->HasError())
			{
				errorText.Append("Errors:\n");
				for (const FString& errTxt : State->GetCurrentErrors())
				{
					errorText.Append(errTxt + "\n");
				}
			}
			if (State->HasWarning())
			{
				errorText.Append("Warnings:\n");
				for (const FString& warningTxt : State->GetCurrentWarnings())
				{
					errorText.Append(warningTxt + "\n");
				}
			}
			State->ResetErrors();

			// If you get this exception, please assign an error handler to your story.
			// If you're using Unity, you can do something like this when you create
			// your story:
			//
			// var story = new Ink.Runtime.Story(jsonTxt);
			// story.onError = (errorMessage, errorType) => {
			//     if( errorType == ErrorType.Warning )
			//         Debug.LogWarning(errorMessage);
			//     else
			//         Debug.LogError(errorMessage);
			// };
			//
			//
			StoryException.Throw(errorText);
		}
	}
}

bool Ink::FStory::ContinueSingleStep()
{
	// Run main step function (walks through content)
	Step();

	// Run out of content and we have a default invisible choice that we can follow?
	if (!CanContinue() && !State->CallStack()->ElementIsEvaluatedFromGame()) {
		TryFollowDefaultInvisibleChoice();
	}

	// Don't save/rewind during string evaluation, which is e.g. used for choices
	if (!State->InStringEvaluation()) {

		// We previously found a newline, but were we just double checking that
		// it wouldn't immediately be removed by glue?
		if (_stateSnapshotAtLastNewline.IsValid()) {

			// Has proper text or a tag been added? Then we know that the newline
			// that was previously added is definitely the end of the line.
			EOutputStateChange change = CalculateNewlineOutputStateChange(
				_stateSnapshotAtLastNewline->GetCurrentText(), State->GetCurrentText(),
				_stateSnapshotAtLastNewline->GetCurrentTags().Num(), State->GetCurrentTags().Num()
			);

			// The last time we saw a newline, it was definitely the end of the line, so we
			// want to rewind to that point.
			if (change == EOutputStateChange::ExtendedBeyondNewline || _sawLookaheadUnsafeFunctionAfterNewline) {
				RestoreStateSnapshot();

				// Hit a newline for sure, we're done
				return true;
			}

			// Newline that previously existed is no longer valid - e.g.
			// glue was encounted that caused it to be removed.
			else if (change == EOutputStateChange::NewlineRemoved) {
				DiscardSnapshot();
			}
		}

		// Current content ends in a newline - approaching end of our evaluation
		if (State->OutputStreamEndsInNewline()) {

			// If we can continue evaluation for a bit:
			// Create a snapshot in case we need to rewind.
			// We're going to continue stepping in case we see glue or some
			// non-text content such as choices.
			if (CanContinue()) {

				// Don't bother to record the state beyond the current newline.
				// e.g.:
				// Hello world\n            // record state at the end of here
				// ~ complexCalculation()   // don't actually need this unless it generates text
				if (!_stateSnapshotAtLastNewline.IsValid())
					StateSnapshot();
			}

			// Can't continue, so we're about to exit - make sure we
			// don't have an old state hanging around.
			else {
				DiscardSnapshot();
			}

		}

	}

	return false;
}

// Assumption: prevText is the snapshot where we saw a newline, and we're checking whether we're really done
//             with that line. Therefore prevText will definitely end in a newline.
//
// We take tags into account too, so that a tag following a content line:
//   Content
//   # tag
// ... doesn't cause the tag to be wrongly associated with the content above.
Ink::FStory::EOutputStateChange Ink::FStory::CalculateNewlineOutputStateChange(const FString& prevText, const FString& currText, int prevTagCount, int currTagCount)
{
	// Simple case: nothing's changed, and we still have a newline
	// at the end of the current content
	const bool newlineStillExists = currText.Len() >= prevText.Len() && currText[prevText.Len() - 1] == '\n';
	if (prevTagCount == currTagCount && prevText.Len() == currText.Len()
		&& newlineStillExists)
		return EOutputStateChange::NoChange;

	// Old newline has been removed, it wasn't the end of the line after all
	if (!newlineStillExists) {
		return EOutputStateChange::NewlineRemoved;
	}

	// Tag added - definitely the start of a new line
	if (currTagCount > prevTagCount)
		return EOutputStateChange::ExtendedBeyondNewline;

	// There must be new content - check whether it's just whitespace
	for (int i = prevText.Len(); i < currText.Len(); i++) {
		auto c = currText[i];
		if (c != ' ' && c != '\t') {
			return EOutputStateChange::ExtendedBeyondNewline;
		}
	}

	// There's new text but it's just spaces and tabs, so there's still the potential
	// for glue to kill the newline.
	return EOutputStateChange::NoChange;
}

/// <summary>
/// Continue the story until the next choice point or until it runs out of content.
/// This is as opposed to the Continue() method which only evaluates one line of
/// output at a time.
/// </summary>
/// <returns>The resulting text evaluated by the ink engine, concatenated together.</returns>
FString Ink::FStory::ContinueMaximally()
{
	IfAsyncWeCant("ContinueMaximally");
	FString text;

	while (CanContinue()) {
		text += Continue();
	}

	return text;
}

void Ink::FStory::Step()
{
	bool shouldAddToStream = true;

	// Get current content
	FPointer pointer = State->GetCurrentPointer();
	if (pointer.IsNull()) {
		return;
	}

	// Step directly to the first element of content in a container (if necessary)
	auto containerToEnter = DynamicCastTo<Ink::FContainer>(pointer.Resolve());
	while (containerToEnter.IsValid()) {
		// Mark container as being entered
		VisitContainer(containerToEnter, true);

		// No content? the most we can do is step past it
		if (containerToEnter->GetContent()->Num() == 0)
			break;

		pointer = FPointer::StartOf(containerToEnter);
		containerToEnter = DynamicCastTo<Ink::FContainer>(pointer.Resolve());
	}
	State->SetCurrentPointer(pointer);

	// Is the current content object:
	//  - Normal content
	//  - Or a logic/flow statement - if so, do it
	// Stop flow if we hit a stack pop when we're unable to pop (e.g. return/done statement in knot
	// that was diverted to rather than called as a function)
	TSharedPtr<Ink::FObject> currentContentObj = pointer.Resolve();
	const bool isLogicOrFlowControl = PerformLogicAndFlowControl(currentContentObj);

	// Has flow been forced to end by flow control above?
	if (State->GetCurrentPointer().IsNull()) {
		return;
	}

	if (isLogicOrFlowControl) {
		shouldAddToStream = false;
	}

	// Choice with condition?
	auto choicePoint = DynamicCastTo<FChoicePoint>(currentContentObj);
	if (choicePoint.IsValid()) {
		const TSharedPtr<Ink::FChoice> choice = ProcessChoice(choicePoint.Get());
		if (choice.IsValid()) {
			State->GeneratedChoices()->Add(choice);
		}

		currentContentObj = nullptr;
		shouldAddToStream = false;
	}

	// If the container has no content, then it will be
	// the "content" itself, but we skip over it.
	auto currentContentAsContainer = DynamicCastTo<FContainer>(currentContentObj);
	if (currentContentAsContainer.IsValid()) {
		shouldAddToStream = false;
	}

	// Content to add to evaluation stack or the output stream
	if (shouldAddToStream) {

		// If we're pushing a variable pointer onto the evaluation stack, ensure that it's specific
		// to our current (possibly temporary) context index. And make a copy of the pointer
		// so that we're not editing the original runtime object.
		auto varPointer = DynamicCastTo<FValueVariablePointer>(currentContentObj);
		if (varPointer.IsValid() && varPointer->GetContextIndex() == -1) {

			// Create new object so we're not overwriting the story's own data
			const int contextIdx = State->CallStack()->ContextForVariableNamed(varPointer->GetVariableName());
			currentContentObj = MakeShared<FValueVariablePointer>(varPointer->GetVariableName(), contextIdx);
		}

		// Expression evaluation content
		if (State->GetInExpressionEvaluation()) {
			State->PushEvaluationStack(currentContentObj);
		}
		// Output stream content (i.e. not expression evaluation)
		else {
			State->PushToOutputStream(currentContentObj);
		}
	}

	// Increment the content pointer, following diverts if necessary
	NextContent();

	// Starting a thread should be done after the increment to the content pointer,
	// so that when returning from the thread, it returns to the content after this instruction.
	auto controlCmd = DynamicCastTo<Ink::FControlCommand>(currentContentObj);
	if (controlCmd.IsValid() && controlCmd->GetCommandType() == ECommandType::StartThread) {
		State->CallStack()->PushThread();
	}
}

TSharedPtr<Ink::FChoice> Ink::FStory::ProcessChoice(FChoicePoint* choicePoint)
{
	bool showChoice = true;

	// Don't create choice if choice point doesn't pass conditional
	if (choicePoint->HasCondition()) {
		auto conditionValue = State->PopEvaluationStack();
		if (!IsTruthy(conditionValue)) {
			showChoice = false;
		}
	}

	FString startText = "";
	FString choiceOnlyText = "";

	if (choicePoint->HasChoiceOnlyContent()) {
		auto choiceOnlyStrVal = DynamicCastTo<FValueString>(State->PopEvaluationStack());
		choiceOnlyText = choiceOnlyStrVal->GetValue();
	}

	if (choicePoint->HasStartContent()) {
		auto startStrVal = DynamicCastTo<FValueString>(State->PopEvaluationStack());
		startText = startStrVal->GetValue();
	}

	// Don't create choice if player has already read this content
	if (choicePoint->IsOnceOnly()) {
		const int visitCount = State->VisitCountForContainer(choicePoint->GetChoiceTarget());
		if (visitCount > 0) {
			showChoice = false;
		}
	}

	// We go through the full process of creating the choice above so
	// that we consume the content for it, since otherwise it'll
	// be shown on the output stream.
	if (!showChoice) {
		return nullptr;
	}

	TSharedPtr<Ink::FChoice> choice = MakeShared<Ink::FChoice>();
	choice->SetTargetPath(choicePoint->GetPathOnChoice());
	choice->SetSourcePath(choicePoint->GetPath()->ToString());
	choice->SetIsInvisibleDefault(choicePoint->IsInvisibleDefault());

	// We need to capture the state of the callstack at the point where
	// the choice was generated, since after the generation of this choice
	// we may go on to pop out from a tunnel (possible if the choice was
	// wrapped in a conditional), or we may pop out from a thread,
	// at which point that thread is discarded.
	// Fork clones the thread, gives it a new ID, but without affecting
	// the thread stack itself.
	choice->SetThreadAtGeneration(State->CallStack()->ForkThread());

	// Set final text for the choice
	const FString finalText = (startText + choiceOnlyText).TrimStartAndEnd();
	choice->SetText(finalText);

	return choice;
}

// Does the expression result represented by this object evaluate to true?
// e.g. is it a Number that's not equal to 1?
bool Ink::FStory::IsTruthy(TSharedPtr<Ink::FObject> Obj)
{
	bool truthy = false;
	const TSharedPtr<FValue> val = DynamicCastTo<FValue>(Obj);
	if (val.IsValid()) {
		TSharedPtr<FValueDivertTarget> divTarget = DynamicCastTo<FValueDivertTarget>(val);
		if (divTarget.IsValid()) {
			Error("Shouldn't use a divert target (to " + divTarget->GetValue()->ToString() + ") as a conditional value. Did you intend a function call 'likeThis()' or a read count check 'likeThis'? (no arrows)");
			return false;
		}

		return val->IsTruthy();
	}
	return truthy;
}

// Maximum snapshot stack:
//  - stateSnapshotDuringSave -- not retained, but returned to game code
//  - _stateSnapshotAtLastNewline (has older patch)
//  - _state (current, being patched)
void Ink::FStory::StateSnapshot()
{
	_stateSnapshotAtLastNewline = State;
	TSharedPtr<FStoryState> patchedCopy = State->CopyAndStartPatching();
	State = patchedCopy;
}

void Ink::FStory::RestoreStateSnapshot()
{
	// Patched state had temporarily hijacked our
	// VariablesState and set its own callstack on it,
	// so we need to restore that.
	// If we're in the middle of saving, we may also
	// need to give the VariablesState the old patch.
	_stateSnapshotAtLastNewline->RestoreAfterPatch();

	State = _stateSnapshotAtLastNewline;
	_stateSnapshotAtLastNewline.Reset();

	// If save completed while the above snapshot was
	// active, we need to apply any changes made since
	// the save was started but before the snapshot was made.
	if (!_asyncSaving) {
		State->ApplyAnyPatch();
	}
}

void Ink::FStory::DiscardSnapshot()
{
	// Normally we want to integrate the patch
	// into the main global/counts dictionaries.
	// However, if we're in the middle of async
	// saving, we simply stay in a "patching" state,
	// albeit with the newer cloned patch.
	if (!_asyncSaving)
		State->ApplyAnyPatch();

	// No longer need the snapshot.
	_stateSnapshotAtLastNewline.Reset();
}

TSharedPtr<Ink::FStoryState> Ink::FStory::CopyStateForBackgroundThreadSave()
{
	IfAsyncWeCant("start saving on a background thread");
	if (_asyncSaving)
	{
		Error("Story is already in background saving mode, can't call CopyStateForBackgroundThreadSave again!");
		return nullptr;
	}
	TSharedPtr<FStoryState> stateToSave = State;
	State = State->CopyAndStartPatching();
	_asyncSaving = true;
	return stateToSave;
}

/// <summary>
/// See CopyStateForBackgroundThreadSave. This method releases the
/// "frozen" save state, applying its patch that it was using internally.
/// </summary>
void Ink::FStory::BackgroundSaveComplete()
{
	// CopyStateForBackgroundThreadSave must be called outside
	// of any async ink evaluation, since otherwise you'd be saving
	// during an intermediate state.
	// However, it's possible to *complete* the save in the middle of
	// a glue-lookahead when there's a state stored in _stateSnapshotAtLastNewline.
	// This state will have its own patch that is newer than the save patch.
	// We hold off on the final apply until the glue-lookahead is finished.
	// In that case, the apply is always done, it's just that it may
	// apply the looked-ahead changes OR it may simply apply the changes
	// made during the save process to the old _stateSnapshotAtLastNewline state.
	if (!_stateSnapshotAtLastNewline.IsValid()) {
		State->ApplyAnyPatch();
	}

	_asyncSaving = false;
}

/// <summary>
/// Checks whether contentObj is a control or flow object rather than a piece of content, 
/// and performs the required command if necessary.
/// </summary>
/// <returns><c>true</c> if object was logic or flow control, <c>false</c> if it's normal content.</returns>
/// <param name="contentObj">Content object.</param>
bool Ink::FStory::PerformLogicAndFlowControl(TSharedPtr<Ink::FObject> contentObj)
{
	if (!contentObj.IsValid()) {
		return false;
	}

	// Divert
	if (TSharedPtr<Ink::FDivert> currentDivert = DynamicCastTo<FDivert>(contentObj)) {

		if (currentDivert->IsConditional()) 
		{
			TSharedPtr<Ink::FObject> conditionValue = State->PopEvaluationStack();

			// False conditional? Cancel divert
			const bool IsFalseConditional = !IsTruthy(conditionValue);
			if (IsFalseConditional)
			{
				return true;
			}
		}

		if (currentDivert->HasVariableTarget()) 
		{
			const FString varName = currentDivert->GetVariableDivertName();

			TSharedPtr<Ink::FObject> varContents = State->GetVariableState()->GetVariableWithName(varName);

			if (!varContents.IsValid()) 
			{
				Error("Tried to divert using a target from a variable that could not be found (" + varName + ")");
			}
			else if (!DynamicCastTo<FValueDivertTarget>(varContents))
			{
				TSharedPtr<FValueInt> intContent = DynamicCastTo<FValueInt>(varContents);

				FString errorMessage = "Tried to divert to a target from a variable, but the variable (" + varName + ") didn't contain a divert target, it ";
				if (intContent && intContent->GetValue() == 0) 
				{
					errorMessage += "was empty/null (the value 0).";
				}
				else 
				{
					errorMessage += "contained '" + varContents->ToString() + "'.";
				}

				Error(errorMessage);
			}
			else
			{
				TSharedPtr<FValueDivertTarget> target = DynamicCastTo<FValueDivertTarget>(varContents);
				State->SetDivertedPointer(PointerAtPath(target->GetValue()));
			}
		}
		else if (currentDivert->IsExternal()) {
			CallExternalFunction(currentDivert->GetTargetPathString(), currentDivert->GetExternalsArgs());
			return true;
		}
		else {
			State->SetDivertedPointer(currentDivert->GetTargetPointer());
		}

		if (currentDivert->GetPushesToStack()) {
			State->CallStack()->Push(
				currentDivert->GetStackPushType(),
				0,
				State->OutputStream().Num()
			);
		}

		if (State->GetDivertedPointer().IsNull() && !currentDivert->IsExternal()) {

			// Human readable name available - runtime divert is part of a hard-written divert that to missing content
			if (currentDivert->GetDebugMetadata().IsValid() && !currentDivert->GetDebugMetadata()->SourceName.IsEmpty()) {
				Error("Divert target doesn't exist: " + currentDivert->GetDebugMetadata()->SourceName);
			}
			else {
				Error("Divert resolution failed: " + currentDivert->ToString());
			}
		}

		return true;
	}

	// Start/end an expression evaluation? Or print out the result?
	else if (TSharedPtr<FControlCommand> evalCommand = DynamicCastTo<FControlCommand>(contentObj))
	{
		switch (evalCommand->GetCommandType())
		{

		case Ink::ECommandType::EvalStart:
		{
			checkf(State->GetInExpressionEvaluation() == false, TEXT("Already in expression evaluation?"));
			State->SetInExpressionEvaluation(true);
			break;
		}

		case Ink::ECommandType::EvalEnd:
		{
			checkf(State->GetInExpressionEvaluation() == true, TEXT("Not in expression evaluation mode"));
			State->SetInExpressionEvaluation(false);
			break;
		}

		case Ink::ECommandType::EvalOutput:
		{
			// If the expression turned out to be empty, there may not be anything on the stack
			if (State->GetEvaluationStack().Num() > 0) {

				TSharedPtr<Ink::FObject> output = State->PopEvaluationStack();

				// Functions may evaluate to Void, in which case we skip output
				if (!DynamicCastTo<FVoid>(output).IsValid()) 
				{
					// Should we really always blanket convert to string?
					// It would be okay to have numbers in the output stream the
					// only problem is when exporting text for viewing, it skips over numbers etc.
					const FString outputAsStr = output->ToString();
					TSharedPtr<FValueString> text = MakeShared<FValueString>(outputAsStr);
					State->PushToOutputStream(text);
				}
			}
			break;
		}

		case Ink::ECommandType::NoOp:
			break;

		case Ink::ECommandType::Duplicate:
		{
			State->PushEvaluationStack(State->PeekEvaluationStack());
			break;
		}

		case Ink::ECommandType::PopEvaluatedValue:
		{
			State->PopEvaluationStack();
			break;
		}

		case Ink::ECommandType::PopFunction:
		case Ink::ECommandType::PopTunnel:
		{
			auto popType = evalCommand->GetCommandType() == Ink::ECommandType::PopFunction ?
				EPushPopType::Function : EPushPopType::Tunnel;

			// Tunnel onwards is allowed to specify an optional override
			// divert to go to immediately after returning: ->-> target
			TSharedPtr<FValueDivertTarget> overrideTunnelReturnTarget;
			if (popType == EPushPopType::Tunnel) {
				TSharedPtr<Ink::FObject> popped = State->PopEvaluationStack();
				overrideTunnelReturnTarget = DynamicCastTo<FValueDivertTarget>(popped);
				if (!overrideTunnelReturnTarget.IsValid()) {
					checkf(DynamicCastTo<FVoid>(popped), TEXT("Expected void if ->-> doesn't override target"));
				}
			}

			if (State->TryExitFunctionEvaluationFromGame()) {
				break;
			}
			else if (State->CallStack()->CurrentElement()->GetType() != popType || !State->CallStack()->CanPop()) {

				TMap<EPushPopType, FString> names;
				names[EPushPopType::Function] = "function return statement (~ return)";
				names[EPushPopType::Tunnel] = "tunnel onwards statement (->->)";

				FString expected = names[State->CallStack()->CurrentElement()->GetType()];
				if (!State->CallStack()->CanPop()) {
					expected = "end of flow (-> END or choice)";
				}

				const FString errorMsg = "Found " + names[popType] + ", when expected " + expected;
				Error(errorMsg);
			}

			else {
				State->PopCallstack();

				// Does tunnel onwards override by diverting to a new ->-> target?
				if (overrideTunnelReturnTarget)
					State->SetDivertedPointer(PointerAtPath(overrideTunnelReturnTarget->GetValue()));
			}

			break;
		}

		case Ink::ECommandType::BeginString:
		{
			TSharedPtr<FControlCommand> castedEvalCommand = DynamicCastTo<FControlCommand>(contentObj);
				
			State->PushToOutputStream(castedEvalCommand);

			checkf(State->GetInExpressionEvaluation() == true, TEXT("Expected to be in an expression when evaluating a string"));
			State->SetInExpressionEvaluation(false);
			break;
		}

		case Ink::ECommandType::EndString:
		{
			// Since we're iterating backward through the content,
			// build a stack so that when we build the string,
			// it's in the right order

			TArray<TSharedPtr<FValueString>> contentStackForString;

			int outputCountConsumed = 0;
			for (int i = State->OutputStream().Num() - 1; i >= 0; --i) {
				auto obj = State->OutputStream()[i];

				outputCountConsumed++;

				TSharedPtr<FControlCommand> command = DynamicCastTo<FControlCommand>(obj);
				if (command.IsValid() && command->GetCommandType() == Ink::ECommandType::BeginString) {
					break;
				}

				TSharedPtr<FValueString> stringValue = DynamicCastTo<FValueString>(obj);
				if (stringValue.IsValid())
					contentStackForString.Push(stringValue);
			}

			// Consume the content that was produced for this string
			State->PopFromOutputStream(outputCountConsumed);

			// Build string out of the content we collected
			FString contentString;
			for (int i = contentStackForString.Num() - 1; i >= 0; --i)
			{
				contentString.Append(contentStackForString[i]->ToString());
			}

			// Return to expression evaluation (from content mode)
			State->SetInExpressionEvaluation(true);
			State->PushEvaluationStack(MakeShared<FValueString>(contentString));
			break;
		}

		case Ink::ECommandType::ChoiceCount:
		{
			const int choiceCount = State->GeneratedChoices()->Num();
			State->PushEvaluationStack(MakeShared<FValueInt>(choiceCount));
			break;
		}

		case Ink::ECommandType::Turns:
		{
			State->PushEvaluationStack(MakeShared<FValueInt>(State->GetCurrentTurnIndex() + 1));
			break;
		}

		case Ink::ECommandType::TurnsSince:
		case Ink::ECommandType::ReadCount:
		{
			TSharedPtr<Ink::FObject> target = State->PopEvaluationStack();

			TSharedPtr<FValueDivertTarget> divertTarget = DynamicCastTo<FValueDivertTarget>(target);
			if (!divertTarget.IsValid()) {
				FString extraNote = "";
				if (DynamicCastTo<FValueInt>(target))
					extraNote = ". Did you accidentally pass a read count ('knot_name') instead of a target ('-> knot_name')?";
				Error("TURNS_SINCE expected a divert target (knot, stitch, label name), but saw " + target->ToString() + extraNote);
				break;
			}

			TSharedPtr<Ink::FObject> correctedObj = ContentAtPath(divertTarget->GetValue()).GetCorrectObject();
			TSharedPtr<Ink::FContainer> container = DynamicCastTo<FContainer>(correctedObj);

			int eitherCount;
			if (container.IsValid()) {
				if (evalCommand->GetCommandType() == Ink::ECommandType::TurnsSince)
					eitherCount = State->TurnsSinceForContainer(container);
				else
					eitherCount = State->VisitCountForContainer(container);
			}
			else {
				if (evalCommand->GetCommandType() == Ink::ECommandType::TurnsSince)
					eitherCount = -1; // turn count, default to never/unknown
				else
					eitherCount = 0; // visit count, assume 0 to default to allowing entry

				Warning("Failed to find container for " + evalCommand->ToString() + " lookup at " + divertTarget->GetValue()->ToString());
			}

			State->PushEvaluationStack(MakeShared<FValueInt>(eitherCount));
			break;
		}

		case Ink::ECommandType::Random:
		{
			TSharedPtr<FValueInt> maxIntPtr = DynamicCastTo<FValueInt>(State->PopEvaluationStack());
			TSharedPtr<FValueInt> minIntPtr = DynamicCastTo<FValueInt>(State->PopEvaluationStack());

			if (maxIntPtr == nullptr)
			{
				Error("Invalid value for maximum parameter of RANDOM(min, max)");
				break;
			}

			if (minIntPtr == nullptr)
			{
				Error("Invalid value for minimum parameter of RANDOM(min, max)");
				break;
			}

			int maxInt = maxIntPtr->GetValue();
			int minInt = minIntPtr->GetValue();
			if (!(maxInt > minInt))
			{
				Error("RANDOM was called with minimum as " + FString::FromInt(minInt) + " and maximum as " + FString::FromInt(maxInt) + ". The maximum must exceed the minimum");
				break;
			}

			FRandomStream random(State->GetStorySeed() + State->GetPreviousRandom());
			const int32 chosenValue = random.RandRange(minInt, maxInt);
			State->PushEvaluationStack(MakeShared<FValueInt>(chosenValue));
			State->SetPreviousRandom(random.GetCurrentSeed());
			break;
		}

		case Ink::ECommandType::SeedRandom:
		{
			TSharedPtr<FValueInt> seed = DynamicCastTo<FValueInt>(State->PopEvaluationStack());
			if (!seed.IsValid())
			{
				Error("Invalid value passed to SEED_RANDOM");
				break;
			}

			// Story seed affects both RANDOM and shuffle behaviour
			State->SetStorySeed(seed->GetValue());
			State->SetPreviousRandom(0);

			// SEED_RANDOM returns nothing.
			State->PushEvaluationStack(MakeShared<FVoid>());
			break;
		}

		case Ink::ECommandType::VisitIndex:
		{
			const int count = State->VisitCountForContainer(State->GetCurrentPointer().GetContainer()) - 1; // index not count
			State->PushEvaluationStack(MakeShared<FValueInt>(count));
			break;
		}

		case Ink::ECommandType::SequenceShuffleIndex:
		{
			auto shuffleIndex = NextSequenceShuffleIndex();
			State->PushEvaluationStack(MakeShared<FValueInt>(shuffleIndex));
			break;
		}

		case Ink::ECommandType::StartThread:
		{
			// Handled in main step function
			break;
		}

		case Ink::ECommandType::Done:
		{
			// We may exist in the context of the initial
			// act of creating the thread, or in the context of
			// evaluating the content.
			if (State->CallStack()->CanPopThread()) {
				State->CallStack()->PopThread();
			}

			// In normal flow - allow safe exit without warning
			else {
				State->SetDidSafeExit(true);

				// Stop flow in current thread
				State->SetCurrentPointer(FPointer::Null());
			}
			break;
		}

		// Force flow to end completely
		case Ink::ECommandType::End:
		{
			State->ForceEnd();
			break;
		}

		case Ink::ECommandType::ListFromInt:
		{
			TSharedPtr<FValueInt> intVal = DynamicCastTo<FValueInt>(State->PopEvaluationStack());
			TSharedPtr<FValueString> listNameVal = DynamicCastTo<FValueString>(State->PopEvaluationStack());

			if (intVal == nullptr) {
				StoryException.Throw("Passed non-integer when creating a list element from a numerical value.");
				return false;
			}

			TSharedPtr<FValueList> generatedListValue;

			TSharedPtr<FListDefinition> foundListDef;
			if (ListDefinitions->TryListGetDefinition(listNameVal->GetValue(), foundListDef)) {
				FInkListItem foundItem;
				if (foundListDef->TryGetItemWithValue(intVal->GetValue(), foundItem)) {
					generatedListValue = MakeShared<FValueList>(foundItem, intVal->GetValue());
				}
			}
			else {
				StoryException.Throw("Failed to find LIST called " + listNameVal->GetValue());
				return false;
			}

			if (!generatedListValue.IsValid())
				generatedListValue = MakeShared<FValueList>();

			State->PushEvaluationStack(generatedListValue);
			break;
		}

		case Ink::ECommandType::ListRange:
		{
			TSharedPtr<FValue> max = DynamicCastTo<FValue>(State->PopEvaluationStack());
			TSharedPtr<FValue> min = DynamicCastTo<FValue>(State->PopEvaluationStack());
			TSharedPtr<FValueList> targetList = DynamicCastTo<FValueList>(State->PopEvaluationStack());

			if (!targetList.IsValid() || !min.IsValid() || !max.IsValid())
			{
				StoryException.Throw("Expected list, minimum and maximum for LIST_RANGE");
				return false;
			}

			FInkList result = targetList->GetValue().ListWithSubRange(min->GetValueObject(), max->GetValueObject());
			State->PushEvaluationStack(MakeShared<FValueList>(result));
			break;
		}

		case Ink::ECommandType::ListRandom:
		{
			auto listVal = DynamicCastTo<FValueList>(State->PopEvaluationStack());
			if (!listVal.IsValid())
			{
				StoryException.Throw("Expected list for LIST_RANDOM");
				return false;
			}

			auto list = listVal->GetValue();
			FInkList newList;

			// List was empty: return empty list
			if (list.Num() > 0) {
				// Generate a random index for the element to take
				const int resultSeed = State->GetStorySeed() + State->GetPreviousRandom();
				FRandomStream random(resultSeed);
				const uint32 nextRandom = random.GetUnsignedInt();
				const int listItemIndex = nextRandom % list.Num();

				auto randomItem = list.GetOrderedItems()[listItemIndex];

				// Origin list is simply the origin of the one element
				newList = FInkList(randomItem.Key.OriginName, this);
				newList.Add(randomItem.Key, randomItem.Value);

				State->SetPreviousRandom(nextRandom);
			}

			State->PushEvaluationStack(MakeShared<FValueList>(newList));
			break;
		}

		default:
		{
			Error("unhandled ControlCommand: " + evalCommand->ToString());
			break;
		}
		}
		return true;
	}
	// Variable assignment
	else if (TSharedPtr<FVariableAssignment> variableAssignment = DynamicCastTo<FVariableAssignment>(contentObj)) {
		auto assignedVal = State->PopEvaluationStack();
		State->GetVariableState()->Assign(variableAssignment, assignedVal);
		return true;
	}

	// Variable reference
	else if (TSharedPtr<FVariableReference> varRef = DynamicCastTo<FVariableReference>(contentObj)) {
		TSharedPtr<Ink::FObject> foundValue;

		// Explicit read count value
		if (!varRef->GetPathStringForCount().IsEmpty()) {

			auto container = varRef->GetContainerForCount();
			int count = State->VisitCountForContainer(container);
			foundValue = MakeShared<FValueInt>(count);
		}

		// Normal variable reference
		else {

			foundValue = State->GetVariableState()->GetVariableWithName(varRef->GetName());

			if (!foundValue.IsValid()) {
				Warning("Variable not found: '" + varRef->GetName() + "'. Using default value of 0 (false). This can happen with temporary variables if the declaration hasn't yet been hit. Globals are always given a default value on load if a value doesn't exist in the save state.");
				foundValue = MakeShared<FValueInt>(0);
			}
		}

		State->PushEvaluationStack(foundValue);

		return true;
	}

	// Native function call
	else if (TSharedPtr<FNativeFunctionCall> func = DynamicCastTo<FNativeFunctionCall>(contentObj)) {
		
		auto funcParams = State->PopEvaluationStack(func->GetNumberOfParameters());
		auto result = func->Call(funcParams);
		State->PushEvaluationStack(result);
		return true;
	}

	// No control content, must be ordinary content
	return false;
}

/// <summary>
/// Change the current position of the story to the given path. From here you can 
/// call Continue() to evaluate the next line.
/// 
/// The path string is a dot-separated path as used internally by the engine.
/// These examples should work:
/// 
///    myKnot
///    myKnot.myStitch
/// 
/// Note however that this won't necessarily work:
/// 
///    myKnot.myStitch.myLabelledChoice
/// 
/// ...because of the way that content is nested within a weave structure.
/// 
/// By default this will reset the callstack beforehand, which means that any
/// tunnels, threads or functions you were in at the time of calling will be
/// discarded. This is different from the behaviour of ChooseChoiceIndex, which
/// will always keep the callstack, since the choices are known to come from the
/// correct state, and known their source thread.
/// 
/// You have the option of passing false to the resetCallstack parameter if you
/// don't want this behaviour, and will leave any active threads, tunnels or
/// function calls in-tact.
/// 
/// This is potentially dangerous! If you're in the middle of a tunnel,
/// it'll redirect only the inner-most tunnel, meaning that when you tunnel-return
/// using '->->', it'll return to where you were before. This may be what you
/// want though. However, if you're in the middle of a function, ChoosePathString
/// will throw an exception.
/// 
/// </summary>
/// <param name="InPath">A dot-separted path string, as specified above.</param>
/// <param name="resetCallstack">Whether to reset the callstack first (see summary description).</param>
/// <param name="Arguments">Optional set of arguments to pass, if path is to a knot that takes them.</param>
void Ink::FStory::ChoosePathString(const FString& InPath, bool resetCallstack, const TArray<TSharedPtr<Ink::FValueType>>& Arguments)
{
	IfAsyncWeCant("call ChoosePathString right now");
	OnChoosePathStringEvent.Broadcast(InPath, Arguments);

	if (resetCallstack) {
		ResetCallstack();
	}
	else {
		// ChoosePathString is potentially dangerous since you can call it when the stack is
		// pretty much in any state. Let's catch one of the worst offenders.
		if (State->CallStack()->CurrentElement()->GetType() == EPushPopType::Function) {
			FString funcDetail = "";
			TSharedPtr<FContainer> container = State->CallStack()->CurrentElement()->GetCurrentPointer().GetContainer();
			if (container.IsValid()) {
				funcDetail = "(" + container->GetPath()->ToString() + ") ";
			}
			Error("Story was running a function " + funcDetail + "when you called ChoosePathString(" + InPath + ") - this is almost certainly not not what you want! Full stack trace: \n" + State->CallStack()->CallStackTrace());
			return;
		}
	}

	State->PassArgumentsToEvaluationStack(Arguments);
	ChoosePath(MakeShared<FPath>(InPath));
}

FString Ink::FStory::GetCurrentText() const
{
	IfAsyncWeCant("call currentText since it's a work in progress");
	return State->GetCurrentText();
}

TArray<TSharedPtr<Ink::FChoice>> Ink::FStory::GetCurrentChoices() const
{
	// Don't include invisible choices for external usage.
	TArray<TSharedPtr<Ink::FChoice>> choices;
	TArray<TSharedPtr<Ink::FChoice>> stateChoices = *State->CurrentChoices();
	for (TSharedPtr<Ink::FChoice> choice : stateChoices)
	{
		if (!choice->IsInvisibleDefault())
		{
			choice->SetIndex(choices.Num());
			choices.Add(choice);
		}
	}
	return choices;
}

const TArray<FString>& Ink::FStory::GetCurrentTags() const
{
	IfAsyncWeCant("call currentTags since it's a work in progress");
	return State->GetCurrentTags();
}

const TArray<FString>& Ink::FStory::GetCurrentErrors() const
{
	return State->GetCurrentErrors();
}

const TArray<FString>& Ink::FStory::GetCurrentWarnings() const
{
	return State->GetCurrentWarnings();
}

FString Ink::FStory::GetCurrentFlowName() const
{
	return State->GetCurrentFlowName();
}

bool Ink::FStory::HasError() const
{
	return State->HasError();
}

bool Ink::FStory::HasWarning() const
{
	return State->HasWarning();
}

TSharedPtr<Ink::FVariableState> Ink::FStory::GetVariablesState() const
{
	return State->GetVariableState();
}

// c#: Throw an exception that gets caught and causes AddError to be called,
// then exits the flow.
// C++: we fake the exception because we aren't allowed to throw in this codebase
// After calling "throwable" code, we must check StoryException.hasException boolean
// we sadly won't actually break out of the program flow though
void Ink::FStory::Error(const FString& InMessage, bool useEndLineNumber) const
{
	StoryException.Throw(InMessage, useEndLineNumber);
}

void Ink::FStory::Warning(const FString& InMessage) const
{
	AddError(InMessage, true);
}

TSharedPtr<Ink::FContainer> Ink::FStory::GetMainContentContainer() const
{
	if (TemporaryEvaluationContainer.IsValid())
		return TemporaryEvaluationContainer;
	else
		return MainContentContainer;
}

Ink::FSearchResult Ink::FStory::ContentAtPath(TSharedPtr<Ink::FPath> InPath) const
{
	return GetMainContentContainer()->GetContentAtPath(InPath);
}

TSharedPtr<Ink::FContainer> Ink::FStory::KnotContainerWithName(const FString& InName) const
{
	const TSharedPtr<Ink::FObject>* foundContainer = MainContentContainer->GetNamedContent()->Find(InName);
	if (foundContainer != nullptr)
		return DynamicCastTo<Ink::FContainer>(*foundContainer);
	else
		return nullptr;
}

Ink::FPointer Ink::FStory::PointerAtPath(TSharedPtr<Ink::FPath> InPath) const
{
	int pathLengthToUse = InPath->GetLength();
	if (pathLengthToUse == 0)
		return FPointer::Null();

	Ink::FPointer p;
	FSearchResult result;

	if (InPath->GetLastComponent()->IsIndex()) {
		pathLengthToUse = InPath->GetLength() - 1;
		result = MainContentContainer->GetContentAtPath(InPath, 0, pathLengthToUse);
		p = FPointer(result.GetObjectAsContainer(), InPath->GetLastComponent()->GetIndex());
	}
	else {
		result = MainContentContainer->GetContentAtPath(InPath);
		p = FPointer(result.GetObjectAsContainer(), -1);
	}

	if (result.GetObject() == nullptr || ((result.GetObject() == MainContentContainer) && pathLengthToUse > 0))
		Error("Failed to find content at path '" + InPath->ToString() + "', and no approximation of it was possible.");
	else if (result.GetIsApproximate())
		Warning("Failed to find content at path '" + InPath->ToString() + "', so it was approximated to: '" + result.GetObject()->GetPath()->ToString() + "'.");

	return p;
}

TSharedPtr<Ink::FStoryState> Ink::FStory::GetStoryState() const
{
	return State;
}

TSharedPtr<Ink::FListDefinitionsOrigin> Ink::FStory::GetListDefinitions() const
{
	return ListDefinitions;
}

TSharedPtr<Ink::FDebugMetadata> Ink::FStory::GetCurrentDebugMetadata() const
{
	TSharedPtr<Ink::FDebugMetadata> debugMetadata = MakeShared<Ink::FDebugMetadata>();

	// Try to get from current path first
	const Ink::FPointer pointer = State->GetCurrentPointer();
	if (!pointer.IsNull())
	{
		debugMetadata = pointer.Resolve()->GetDebugMetadata();
		if (debugMetadata.IsValid())
			return debugMetadata;
	}

	// Move up callstack if possible
	const TArray<TSharedPtr<Ink::FElement>>& callStackElements = State->CallStack()->GetElements();
	for (int32 i = callStackElements.Num() - 1; i >= 0; --i)
	{
		Ink::FPointer elementPointer = callStackElements[i]->GetCurrentPointer();
		if (!elementPointer.IsNull())
		{
			TSharedPtr<Ink::FObject> resolvedObject = elementPointer.Resolve();
			if (resolvedObject.IsValid())
			{
				debugMetadata = resolvedObject->GetDebugMetadata();
				if (debugMetadata.IsValid())
					return debugMetadata;
			}
		}
	}

	// Current/previous path may not be valid if we've just had an error,
	// or if we've simply run out of content.
	// As a last resort, try to grab something from the output stream
	for (int i = State->OutputStream().Num() -1; i >= 0; --i)
	{
		TSharedPtr<Ink::FObject> outputObj = State->OutputStream()[i];
		if (!outputObj.IsValid())
			return nullptr;

		debugMetadata = outputObj->GetDebugMetadata();
		if (debugMetadata.IsValid())
			return debugMetadata;
	}

	return nullptr;
}

void Ink::FStory::AddError(const FString& InMessage, bool InIsWarning /*= false*/, bool InUseEndLineNumber /*= false*/) const
{	
	TSharedPtr<Ink::FDebugMetadata> debugMetadata = GetCurrentDebugMetadata();
	const FString errorTypeString = InIsWarning ? TEXT("WARNING") : TEXT("ERROR");

	FString message;
	if (debugMetadata.IsValid())
	{
		const int32 lineNumber = InUseEndLineNumber ? debugMetadata->EndLineNumber : debugMetadata->StartLineNumber;
		message = FString::Printf(TEXT("RUNTIME %s: '%s' line %d: %s"), *errorTypeString, *debugMetadata->FileName, lineNumber, *InMessage);
	}
	else if (!State->GetCurrentPointer().IsNull())
	{
		message = FString::Printf(TEXT("RUNTIME %s: (%s): %s"), *errorTypeString, *State->GetCurrentPointer().ToString(), *InMessage);
	}
	else
	{
		message = FString::Printf(TEXT("RUNTIME %s: %s"), *errorTypeString, *InMessage);
	}

	State->AddError(message, InIsWarning);

	// In a broken state don't need to know about any other errors
	if (!InIsWarning)
		State->ForceEnd();
}

void Ink::FStory::IfAsyncWeCant(const FString& ActivityStr) const
{
	if (_asyncContinueActive)
		Error("Can't " + ActivityStr + ". Story is in the middle of a ContinueAsync(). Make more ContinueAsync() calls or a single Continue() call beforehand.");
}

void Ink::FStory::ChoosePath(TSharedPtr<FPath> InPath, bool incrementingTurnIndex)
{
	State->SetChosenPath(InPath, incrementingTurnIndex);

	// Take a note of newly visited containers for read counts etc
	VisitChangedContainersDueToDivert();
}

/// <summary>
/// Chooses the Choice from the currentChoices list with the given
/// index. Internally, this sets the current content path to that
/// pointed to by the Choice, ready to continue story evaluation.
/// </summary>
void Ink::FStory::ChooseChoiceIndex(int ChoiceIndex)
{
	const bool choiceIsValid = ChoiceIndex >= 0 && ChoiceIndex < GetCurrentChoices().Num();
	if (!choiceIsValid)
	{
		Error("choice out of range");
		return;
	}

	// Replace callstack with the one from the thread at the choosing point, 
	// so that we can jump into the right place in the flow.
	// This is important in case the flow was forked by a new thread, which
	// can create multiple leading edges for the story, each of
	// which has its own context.
	const TSharedPtr<FChoice> choiceToChoose = GetCurrentChoices()[ChoiceIndex];
	State->CallStack()->SetCurrentThread(choiceToChoose->GetThreadAtGeneration());
	ChoosePath(choiceToChoose->GetTargetPath());

	OnMakeChoiceEvent.Broadcast(choiceToChoose);
}

/// <summary>
/// Checks if a function exists.
/// </summary>
/// <returns>True if the function exists, else false.</returns>
/// <param name="FunctionName">The name of the function as declared in ink.</param>
bool Ink::FStory::HasFunction(const FString& FunctionName) const
{
	const bool hasFunction = KnotContainerWithName(FunctionName) != nullptr;
	return hasFunction;
}

/// <summary>
/// Evaluates a function defined in ink.
/// </summary>
/// <returns>The return value as returned from the ink function with `~ return myValue`, or null if nothing is returned.</returns>
/// <param name="FunctionName">The name of the function as declared in ink.</param>
/// <param name="Arguments">The arguments that the ink function takes, if any. Note that we don't (can't) do any validation on the number of arguments right now, so make sure you get it right!</param>
TSharedPtr<Ink::FValueType> Ink::FStory::EvaluateFunction(const FString& FunctionName, const TArray<TSharedPtr<Ink::FValueType>>& Arguments)
{
	FString dummyArg;
	return EvaluateFunction(FunctionName, dummyArg, Arguments);
}

/// <summary>
/// Evaluates a function defined in ink, and gathers the possibly multi-line text as generated by the function.
/// This text output is any text written as normal content within the function, as opposed to the return value, as returned with `~ return`.
/// </summary>
/// <returns>The return value as returned from the ink function with `~ return myValue`, or null if nothing is returned.</returns>
/// <param name="FunctionName">The name of the function as declared in ink.</param>
/// <param name="outTextOutput">The text content produced by the function via normal ink, if any.</param>
/// <param name="Arguments">The arguments that the ink function takes, if any. Note that we don't (can't) do any validation on the number of arguments right now, so make sure you get it right!</param>
TSharedPtr<Ink::FValueType> Ink::FStory::EvaluateFunction(const FString& FunctionName, FString& outTextOutput, const TArray<TSharedPtr<Ink::FValueType>>& Arguments)
{
	OnEvaluateFunctionEvent.Broadcast(FunctionName, Arguments);
	IfAsyncWeCant("evaluate a function");

	if (FunctionName.IsEmpty()) {
		Error("Function is null");
		return nullptr;
	}
	else if (FunctionName.TrimStartAndEnd().IsEmpty()) {
		Error("Function is empty or white space.");
		return nullptr;
	}

	// Get the content that we need to run
	TSharedPtr<Ink::FContainer> funcContainer = KnotContainerWithName(FunctionName);
	if (!funcContainer.IsValid())
	{
		Error("Function doesn't exist: '" + FunctionName + "'");
		return nullptr;
	}

	// Snapshot the output stream
	TArray<TSharedPtr<Ink::FObject>> outputStreamBefore(State->OutputStream());
	State->ResetOutput();

	// State will temporarily replace the callstack in order to evaluate
	State->StartFunctionEvaluationFromGame(funcContainer, Arguments);

	// Evaluate the function, and collect the string output
	while (CanContinue()) {
		outTextOutput.Append(Continue());
	}

	// Restore the output stream in case this was called
	// during main story evaluation.
	State->ResetOutput(outputStreamBefore);

	// Finish evaluation, and see whether anything was produced
	TSharedPtr<FValueType> result = State->CompleteFunctionEvaluationFromGame();
	OnCompleteEvaluateFunctionEvent.Broadcast(FunctionName, Arguments, outTextOutput, result);
	return result;
}

// Evaluate a "hot compiled" piece of ink content, as used by the REPL-like
// CommandLinePlayer.
TSharedPtr<Ink::FObject> Ink::FStory::EvaluateExpression(TSharedPtr<Ink::FContainer> exprContainer)
{
	const int startCallStackHeight = State->CallStack()->GetElements().Num();

	State->CallStack()->Push(EPushPopType::Tunnel);

	TemporaryEvaluationContainer = exprContainer;

	State->GoToStart();

	const int evalStackHeight = State->GetEvaluationStack().Num();

	Continue();

	TemporaryEvaluationContainer = nullptr;

	// Should have fallen off the end of the Container, which should
	// have auto-popped, but just in case we didn't for some reason,
	// manually pop to restore the state (including currentPath).
	if (State->CallStack()->GetElements().Num() > startCallStackHeight) {
		State->PopCallstack();
	}

	const int endStackHeight = State->GetEvaluationStack().Num();
	if (endStackHeight > evalStackHeight) {
		return State->PopEvaluationStack();
	}
	else {
		return nullptr;
	}
}

void Ink::FStory::CallExternalFunction(const FString& funcName, int numberOfArguments)
{

	FStoryExternalFunctionDef *externalFuncDef = _externals.Find( funcName );
	bool foundExternal = externalFuncDef != nullptr;
	
	TSharedPtr<Ink::FContainer> fallbackFunctionContainer;

    if( foundExternal && externalFuncDef->lookaheadSafe && State->InStringEvaluation() ) {
        // 16th Jan 2023: Example ink that was failing:
        //
        //      A line above
        //      ~ temp text = "{theFunc()}" 
        //      {text} 
        //
        //      === function theFunc() 
        //          { external():
        //              Boom
        //          }
        //
        //      EXTERNAL external() 
        //
        // What was happening: The external() call would exit out early due to
        // _stateSnapshotAtLastNewline having a value, leaving the evaluation stack
        // without a return value on it. When the if-statement tried to pop a value,
        // the evaluation stack would be empty, and there would be an exception.
        //
        // The snapshot rewinding code is only designed to work when outside of
        // string generation code (there's a check for that in the snapshot rewinding code),
        // hence these things are incompatible, you can't have unsafe functions that
        // cause snapshot rewinding in the middle of string generation.
        //
        Error("External function "+funcName+" could not be called because 1) it wasn't marked as lookaheadSafe when BindExternalFunction was called and 2) the story is in the middle of string generation, either because choice text is being generated, or because you have ink like \"hello {func()}\". You can work around this by generating the result of your function into a temporary variable before the string or choice gets generated: ~ temp x = "+funcName+"()");
        return;
    }

    // Should this function break glue? Abort run if we've already seen a newline.
    // Set a bool to tell it to restore the snapshot at the end of this instruction.
    if( foundExternal && !externalFuncDef->lookaheadSafe && _stateSnapshotAtLastNewline.IsValid() ) {
        _sawLookaheadUnsafeFunctionAfterNewline = true;
        return;
    }

    // Try to use fallback function?
    if (!foundExternal) {
        if (allowExternalFunctionFallbacks) {
            fallbackFunctionContainer = KnotContainerWithName (funcName);
            checkf(fallbackFunctionContainer.IsValid(), TEXT("Trying to call EXTERNAL function which has not been bound, and fallback ink function could not be found."));

            // Divert direct into fallback function and we're done
            State->CallStack()->Push (
				Ink::EPushPopType::Function,
				0,
				State->OutputStream().Num()
            );
            State->SetDivertedPointer( FPointer::StartOf(fallbackFunctionContainer) );
            return;

        } else {
            checkf (false, TEXT("Trying to call EXTERNAL function which has not been bound (and ink fallbacks disabled).") );
        }
    }

    // Pop arguments
    // insert arguments at zero to reverse the order they were popped in, so they're the right way round again.
	TArray<Ink::FValueType> arguments;
    for (int i = 0; i < numberOfArguments; ++i) {
		TSharedPtr<FValue> poppedObj = DynamicCastTo<FValue>(State->PopEvaluationStack());
        Ink::FValueType valueObj = poppedObj->GetValueObject();
        arguments.Insert(valueObj, 0);
    }

    // Run the function!
	TSharedPtr<Ink::FValueType> funcResult = nullptr;
	if(foundExternal && externalFuncDef->function.IsValid() )
		funcResult = externalFuncDef->function->Execute( arguments );

    // Convert return value (if any) to the a type that the ink engine can use
    TSharedPtr<Ink::FObject> returnObj = nullptr;
    if (funcResult.IsValid()) {
        returnObj = FValue::Create (*funcResult);
        checkf(returnObj.IsValid(), TEXT("Could not create ink value from returned object of type "));
    } else {
        returnObj = MakeShared<FVoid>();
    }
                
    State->PushEvaluationStack (returnObj);
}

void Ink::FStory::BindExternalFunctionGeneral(const FString& funcName, TSharedPtr<FStoryExternalFunction> func, bool lookaheadSafe)
{
	IfAsyncWeCant( "bind an external function" );
    checkf(_externals.Find( funcName ) == nullptr, TEXT("Function has already been bound."));
    _externals.Emplace( funcName, {func, lookaheadSafe} );
}

void Ink::FStory::UnbindExternalFunction(const FString& funcName)
{
	IfAsyncWeCant( "unbind an external a function" );
    checkf(_externals.Find( funcName ), TEXT("Function has not been bound."));
    _externals.Remove (funcName);
}

void Ink::FStory::ObserveAllVariables(TWeakPtr<FStoryVariableObserver> Observer)
{
	IfAsyncWeCant("observe a new variable");

	TArray<FString> variableNames;
	GetVariablesState()->GetVariables().GenerateKeyArray(variableNames);
	for (const FString& variableName : variableNames)
	{
		ObserveVariable(variableName, Observer);
	}
}

/// <summary>
/// When the named global variable changes it's value, the observer will be
/// called to notify it of the change. Note that if the value changes multiple
/// times within the ink, the observer will only be called once, at the end
/// of the ink's evaluation. If, during the evaluation, it changes and then
/// changes back again to its original value, it will still be called.
/// Note that the observer will also be fired if the value of the variable
/// is changed externally to the ink, by directly setting a value in
/// story.variablesState.
/// </summary>
/// <param name="VariableName">The name of the global variable to observe.</param>
/// <param name="Observer">A delegate function to call when the variable changes.</param>
void Ink::FStory::ObserveVariable(const FString& VariableName, TWeakPtr<FStoryVariableObserver> Observer)
{
	IfAsyncWeCant("observe a new variable");

	if (!State->GetVariableState()->GlobalVariableExistsWithName(VariableName))
	{
		Error("Cannot observe variable '" + VariableName + "' because it wasn't declared in the ink story.");
		return;
	}

	if (_variableObservers.Contains(VariableName))
	{
		_variableObservers[VariableName].Add(Observer);
	}
	else
	{
		TArray<TWeakPtr<FStoryVariableObserver>> observers;
		observers.Emplace(Observer);
		_variableObservers.Add(VariableName, observers);
	}
}

/// <summary>
/// Convenience function to allow multiple variables to be observed with the same
/// observer delegate function. See the singular ObserveVariable for details.
/// The observer will get one call for every variable that has changed.
/// </summary>
/// <param name="VariableNames">The set of variables to observe.</param>
/// <param name="Observer">The delegate function to call when any of the named variables change.</param>
void Ink::FStory::ObserveVariables(const TArray<FString> VariableNames, TWeakPtr<FStoryVariableObserver> Observer)
{
	for (const FString& VariableName : VariableNames)
	{
		ObserveVariable(VariableName, Observer);
	}
}

/// <summary>
/// Removes the variable Observer, to stop getting variable change notifications.
/// If you pass a specific variable name, it will stop observing that particular one. If you
/// pass null (or leave it blank, since it's optional), then the Observer will be removed
/// from all variables that it's subscribed to. If you pass in a specific variable name and
/// null for the the Observer, all observers for that variable will be removed. 
/// </summary>
/// <param name="Observer">(Optional) The Observer to stop observing.</param>
/// <param name="SpecificVariableName">(Optional) Specific variable name to stop observing.</param>
void Ink::FStory::RemoveVariableObserver(TWeakPtr<FStoryVariableObserver> Observer, const FString& SpecificVariableName)
{
	IfAsyncWeCant("remove a variable Observer");

	if (_variableObservers.Num() == 0)
		return;

	// Remove Observer for this specific variable
	if (!SpecificVariableName.IsEmpty()) {
		if (_variableObservers.Contains(SpecificVariableName)) {
			if (Observer.IsValid()) {
				_variableObservers[SpecificVariableName].Remove(Observer);
				if (_variableObservers[SpecificVariableName].Num() == 0) {
					_variableObservers.Remove(SpecificVariableName);
				}
			}
			else {
				_variableObservers.Remove(SpecificVariableName);
			}
		}
	}

	// Remove Observer for all variables
	else if (Observer.IsValid()) {
		TArray<FString> keys;
		_variableObservers.GenerateKeyArray(keys);

		for (const FString& varName : keys)
		{
			_variableObservers[varName].Remove(Observer);
			if (_variableObservers[varName].Num() == 0) {
				_variableObservers.Remove(varName);
			}
		}
	}
}

/// <summary>
/// Get any global tags associated with the story. These are defined as
/// hash tags defined at the very top of the story.
/// </summary>
TArray<FString> Ink::FStory::GlobalTags() const
{
	return TagsAtStartOfFlowContainerWithPathString("");
}

/// <summary>
/// Gets any tags associated with a particular knot or knot.stitch.
/// These are defined as hash tags defined at the very top of a 
/// knot or stitch.
/// </summary>
/// <param name="InPath">The path of the knot or stitch, in the form "knot" or "knot.stitch".</param>
TArray<FString> Ink::FStory::TagsForContentAtPath(const FString& InPath) const
{
	return TagsAtStartOfFlowContainerWithPathString(InPath);
}

TArray<FString> Ink::FStory::TagsAtStartOfFlowContainerWithPathString(const FString& PathString) const
{
	TSharedPtr<FPath> path = MakeShared<FPath>(PathString);

	// Expected to be global story, knot or stitch
	TSharedPtr<FContainer> flowContainer = ContentAtPath(path).GetObjectAsContainer();
	const TSharedPtr<FObject> firstContentObj = flowContainer->GetContent()->operator[](0);
	const TSharedPtr<FContainer> firstContent = DynamicCastTo<FContainer>(firstContentObj);
	if (firstContent.IsValid())
	{
		flowContainer = firstContent;
	}

	// Any initial tag objects count as the "main tags" associated with that story/knot/stitch
	TArray<FString> tags;
	for(const TSharedPtr<FObject>& c : (*flowContainer->GetContent())) {
		TSharedPtr<FTag> tag = DynamicCastTo<FTag>(c);
		if (tag.IsValid()) {
			tags.Add(tag->GetText());
		}
		else break;
	}

	return tags;
}

/// <summary>
/// Useful when debugging a (very short) story, to visualise the state of the
/// story. Add this call as a watch and open the extended text. A left-arrow mark
/// will denote the current point of the story.
/// It's only recommended that this is used on very short debug stories, since
/// it can end up generate a large quantity of text otherwise.
/// </summary>
FString Ink::FStory::BuildStringOfHeirarchy() const
{
	// Unimplemented: Seems to be an optional story debugging aid
	return "";
}

FString Ink::FStory::BuildStringOfContainer(FContainer* Container) const
{
	// Unimplemented: Seems to be an optional story debugging aid
	return "";
}

// Mark a container as having been visited
void Ink::FStory::VisitContainer(TSharedPtr<FContainer> Container, bool AtStart)
{
	if (!Container->GetCountingAtStartOnly() || AtStart)
	{
		if (Container->GetVisitsShouldBeCounted())
			State->IncrementVisitCountForContainer(Container);

		if (Container->GetTurnIndexShouldBeCounted())
			State->RecordTurnIndexVisitToContainer(Container);
	}
}

void Ink::FStory::VisitChangedContainersDueToDivert()
{
	FPointer previousPointer = State->GetPreviousPointer();
	FPointer pointer = State->GetCurrentPointer();

	// Unless we're pointing *directly* at a piece of content, we don't do
	// counting here. Otherwise, the main stepping function will do the counting.
	if (pointer.IsNull() || pointer.GetIndex() == -1)
		return;

	// First, find the previously open set of containers
	TArray<TSharedPtr<FContainer>> _prevContainers;
	if (!previousPointer.IsNull())
	{
		auto prevAncestor = DynamicCastTo<FContainer>(previousPointer.Resolve());
		if (!prevAncestor.IsValid())
		{
			prevAncestor = previousPointer.GetContainer();
		}

		while (prevAncestor.IsValid()) {
			_prevContainers.Add(prevAncestor);
			prevAncestor = DynamicCastTo<FContainer>(prevAncestor->GetParent());
		}
	}

	// If the new object is a container itself, it will be visited automatically at the next actual
	// content step. However, we need to walk up the new ancestry to see if there are more new containers
	TSharedPtr<Ink::FObject> currentChildOfContainer = pointer.Resolve();
	if (!currentChildOfContainer.IsValid())
		return;

	auto currentContainerAncestor = DynamicCastTo<FContainer>(currentChildOfContainer->GetParent());
	bool allChildrenEnteredAtStart = true;
	while (currentContainerAncestor != nullptr && (!_prevContainers.Contains(currentContainerAncestor) || currentContainerAncestor->GetCountingAtStartOnly()))
	{
		// Check whether this ancestor container is being entered at the start,
		// by checking whether the child object is the first.
		const bool enteringAtStart = currentContainerAncestor->GetContent()->Num() > 0
			&& currentChildOfContainer == currentContainerAncestor->GetContent()->operator[](0)
			&& allChildrenEnteredAtStart;

		// Don't count it as entering at start if we're entering random somewhere within
		// a container B that happens to be nested at index 0 of container A. It only counts
		// if we're diverting directly to the first leaf node.
		if (!enteringAtStart)
			allChildrenEnteredAtStart = false;

		// Mark a visit to this container
		VisitContainer(currentContainerAncestor, enteringAtStart);

		currentChildOfContainer = currentContainerAncestor;
		currentContainerAncestor = DynamicCastTo<FContainer>(currentContainerAncestor->GetParent());
	}
}

void Ink::FStory::OnVariableStateDidChangeEvent(const FString& VariableName, TSharedPtr<Ink::FObject> NewValueObj)
{
	const TSharedPtr<FValue> value = DynamicCastTo<FValue>(NewValueObj);
	if (!value.IsValid())
	{
		Error("Tried to get the value of a variable that isn't a standard type");
		return;
	}

	TArray<TWeakPtr<FStoryVariableObserver>>* variableObservers = _variableObservers.Find(VariableName);
	if (variableObservers != nullptr)
	{
		for (const TWeakPtr<FStoryVariableObserver>& weakObserver : *variableObservers)
		{
			TSharedPtr<FStoryVariableObserver> observer = weakObserver.Pin();
			if (observer.IsValid())
			{
				observer->ExecuteIfBound(VariableName, value->GetValueObject());
			}
		}
	}
}

void Ink::FStory::NextContent()
{
	// Setting previousContentObject is critical for VisitChangedContainersDueToDivert
	State->SetPreviousPointer(State->GetCurrentPointer());

	// Divert step?
	if (!State->GetDivertedPointer().IsNull()) {

		State->SetCurrentPointer(State->GetDivertedPointer());
		State->SetDivertedPointer(FPointer::Null());

		// Internally uses State->previousContentObject and State->currentContentObject
		VisitChangedContainersDueToDivert();

		// Diverted location has valid content?
		if (!State->GetCurrentPointer().IsNull()) {
			return;
		}

		// Otherwise, if diverted location doesn't have valid content,
		// drop down and attempt to increment.
		// This can happen if the diverted path is intentionally jumping
		// to the end of a container - e.g. a Conditional that's re-joining
	}

	const bool successfulPointerIncrement = IncrementContentPointer();

	// Ran out of content? Try to auto-exit from a function,
	// or finish evaluating the content of a thread
	if (!successfulPointerIncrement) {

		bool didPop = false;

		if (State->CallStack()->CanPop(EPushPopType::Function)) {

			// Pop from the call stack
			State->PopCallstack(EPushPopType::Function);

			// This pop was due to dropping off the end of a function that didn't return anything,
			// so in this case, we make sure that the evaluator has something to chomp on if it needs it
			if (State->GetInExpressionEvaluation()) {
				State->PushEvaluationStack(MakeShared<FVoid>());
			}

			didPop = true;
		}
		else if (State->CallStack()->CanPopThread()) {
			State->CallStack()->PopThread();

			didPop = true;
		}
		else {
			State->TryExitFunctionEvaluationFromGame();
		}

		// Step past the point where we last called out
		if (didPop && !State->GetCurrentPointer().IsNull()) {
			NextContent();
		}
	}
}

bool Ink::FStory::IncrementContentPointer()
{
	bool successfulIncrement = true;

	FPointer pointer = State->CallStack()->CurrentElement()->GetCurrentPointer();
	pointer.IncrementIndex();

	// Each time we step off the end, we fall out to the next container, all the
	// while we're in indexed rather than named content
	while (pointer.GetIndex() >= pointer.GetContainer()->GetContent()->Num()) {

		successfulIncrement = false;

		auto nextAncestor = DynamicCastTo<FContainer>(pointer.GetContainer()->GetParent());
		if (nextAncestor == nullptr) {
			break;
		}

		const int indexInAncestor = nextAncestor->GetContent()->IndexOfByKey(pointer.GetContainer());
		if (indexInAncestor == INDEX_NONE) {
			break;
		}

		pointer = FPointer(nextAncestor, indexInAncestor);

		// Increment to next content in outer container
		pointer.IncrementIndex();

		successfulIncrement = true;
	}

	if (!successfulIncrement)
		pointer = FPointer::Null();

	State->CallStack()->CurrentElement()->SetCurrentPointer(pointer);

	return successfulIncrement;
}

bool Ink::FStory::TryFollowDefaultInvisibleChoice()
{
	TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> allChoices = State->CurrentChoices();

	// Is a default invisible choice the ONLY choice?
	TArray<TSharedPtr<Ink::FChoice>> invisibleChoices = allChoices->FilterByPredicate([&](TSharedPtr<Ink::FChoice> Choice) {
		return Choice->IsInvisibleDefault();
		});

	if (invisibleChoices.Num() == 0 || allChoices->Num() > invisibleChoices.Num())
		return false;

	const TSharedPtr<Ink::FChoice> choice = invisibleChoices[0];

	// Invisible choice may have been generated on a different thread,
	// in which case we need to restore it before we continue
	State->CallStack()->SetCurrentThread(choice->GetThreadAtGeneration());

	// If there's a chance that this state will be rolled back to before
	// the invisible choice then make sure that the choice thread is
	// left intact, and it isn't re-entered in an old state.
	if (_stateSnapshotAtLastNewline.IsValid())
		State->CallStack()->SetCurrentThread(State->CallStack()->ForkThread());

	ChoosePath(choice->GetTargetPath(), false);

	return true;
}

// Note that this is O(n), since it re-evaluates the shuffle indices
// from a consistent seed each time.
int Ink::FStory::NextSequenceShuffleIndex()
{
	auto numElementsIntVal = DynamicCastTo<FValueInt>(State->PopEvaluationStack());
	if (!numElementsIntVal.IsValid()) {
		Error("expected number of elements in sequence for shuffle index");
		return 0;
	}

	TSharedPtr<FContainer> seqContainer = State->GetCurrentPointer().GetContainer();

	const int numElements = numElementsIntVal->GetValue();

	auto seqCountVal = DynamicCastTo<FValueInt>(State->PopEvaluationStack());
	const int seqCount = seqCountVal->GetValue();
	const int loopIndex = seqCount / numElements;
	const int iterationIndex = seqCount % numElements;

	// Generate the same shuffle based on:
	//  - The hash of this container, to make sure it's consistent
	//    each time the runtime returns to the sequence
	//  - How many times the runtime has looped around this full shuffle
	FString seqPathStr = seqContainer->GetPath()->ToString();
	int sequenceHash = 0;
	for (int i = 0; i < seqPathStr.Len(); ++i)
	{
		sequenceHash += seqPathStr[i];
	}
	const int randomSeed = sequenceHash + loopIndex + State->GetStorySeed();
	FRandomStream random(randomSeed);

	TArray<int> unpickedIndices;
	for (int i = 0; i < numElements; ++i) {
		unpickedIndices.Add(i);
	}

	for (int i = 0; i <= iterationIndex; ++i) {
		const int chosen = random.GetUnsignedInt() % unpickedIndices.Num();
		const int chosenIndex = unpickedIndices[chosen];
		unpickedIndices.RemoveAt(chosen);

		if (i == iterationIndex) {
			return chosenIndex;
		}
	}

	Error("Should never reach here");
	return 0;
}

int Ink::FStory::GetCurrentLineNumber() const
{
	TSharedPtr<Ink::FDebugMetadata> dm = GetCurrentDebugMetadata();
	if (dm.IsValid())
		return dm->StartLineNumber;
	else
		return 0;
}
