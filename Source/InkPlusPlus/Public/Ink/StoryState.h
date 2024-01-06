#pragma once
#include "CoreMinimal.h"
#include "Json.h"
#include "VariableState.h"
#include "Choice.h"
#include "Value.h"

namespace Ink
{
	class FFlow;
	class FPointer;
	class FCallStack;
	
	class INKPLUSPLUS_API FStoryState
	{
	public:
		FStoryState(Ink::FStory* InStory);
		// ~FStoryState();

		void GoToStart() const;

		FString ToJson() const;
		void ToJson(FArchive* const Stream) const;
		void LoadJSON(const FString& InJSON);
		void LoadJSONObj(const FJsonObject& InJSONObj);

		int VisitCountAtPathString(const FString& PathString);
		int VisitCountForContainer(TSharedPtr<Ink::FContainer> Container);
		void IncrementVisitCountForContainer(TSharedPtr<Ink::FContainer> Container);
		void RecordTurnIndexVisitToContainer(TSharedPtr<Ink::FContainer> Container);
		int TurnsSinceForContainer(TSharedPtr<Ink::FContainer> Container);
		int GetCallstackDepth() const;

		TArray<TSharedPtr<Ink::FObject>>& OutputStream() const;
		TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> CurrentChoices() const;
		TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> GeneratedChoices() const;

		const TArray<FString>& GetCurrentErrors() const;
		const TArray<FString>& GetCurrentWarnings() const;
		TSharedPtr<Ink::FVariableState> GetVariableState();
		TSharedPtr<Ink::FCallStack> CallStack() const;

		TArray<TSharedPtr<Ink::FObject>>& GetEvaluationStack();
		Ink::FPointer GetDivertedPointer();
		void SetDivertedPointer(Ink::FPointer InDivertedPointer);

		int GetCurrentTurnIndex() const;
		int GetStorySeed() const;
		void SetStorySeed(int InStorySeed);
		int GetPreviousRandom() const;
		void SetPreviousRandom(int InPreviousRandom);
		bool GetDidSafeExit() const;
		void SetDidSafeExit(bool InDidSafeExit);

		Ink::FStory* GetStory() const;
		void SetStory(Ink::FStory* InStory);

		FString getCurrentPathString() const;

		Ink::FPointer GetCurrentPointer() const;
		void SetCurrentPointer(Ink::FPointer InCurrentPointer);

		Ink::FPointer GetPreviousPointer() const;
		void SetPreviousPointer(Ink::FPointer InPreviousPointer);

		bool CanContinue() const;
		bool HasError() const;
		bool HasWarning() const;

		FString GetCurrentText();
		FString CleanOutputWhitespace(const FString& Text);

		TArray<FString>& GetCurrentTags();
		FString GetCurrentFlowName() const;
		TSharedPtr<TMap<FString, TSharedPtr<FFlow>>> GetNamedFlows();

		bool GetInExpressionEvaluation() const;
		void SetInExpressionEvaluation(bool InExpressionEvaluation);

		void SwitchFlow_Internal(const FString& FlowName);
		void SwitchToDefaultFlow_Internal();
		void RemoveFlow_Internal(const FString& FlowName);

		TSharedPtr<Ink::FStoryState> CopyAndStartPatching();

		void RestoreAfterPatch();
		void ApplyAnyPatch();

		void WriteJson(TJsonWriter<>* Writer) const;

		void ResetErrors();
		void ResetOutput();
		void ResetOutput(TArray<TSharedPtr<Ink::FObject>>& objs);
		void PushToOutputStream(TSharedPtr<Ink::FObject> RuntimeObject);
		void PopFromOutputStream(int Count);
		TArray<TSharedPtr<Ink::FValueString>> TrySplittingHeadTailWhitespace(TSharedPtr<Ink::FValueString> Single);
		void PushToOutputStreamIndividual(TSharedPtr<Ink::FObject> RuntimeObject);
		void TrimNewlinesFromOutputStream();
		void RemoveExistingGlue();
		bool OutputStreamEndsInNewline() const;
		bool OutputStreamContainsContent() const;
		bool InStringEvaluation() const;
		void PushEvaluationStack(TSharedPtr<Ink::FObject> obj);
		TSharedPtr<Ink::FObject> PopEvaluationStack();
		TArray<TSharedPtr<Ink::FObject>> PopEvaluationStack(int NumberOfObjects);
		TSharedPtr<Ink::FObject> PeekEvaluationStack();
		void PopCallstack(Ink::EPushPopType PopType = Ink::EPushPopType::Null);

		void SetChosenPath(TSharedPtr<FPath> Path, bool incrementingTurnIndex);
		void StartFunctionEvaluationFromGame(TSharedPtr<Ink::FContainer> FuncContainer, const TArray<TSharedPtr<Ink::FValueType>>& Arguments);
		void PassArgumentsToEvaluationStack(const TArray<TSharedPtr<Ink::FValueType>>& Arguments);
		bool TryExitFunctionEvaluationFromGame();
		TSharedPtr<Ink::FValueType> CompleteFunctionEvaluationFromGame();


		/*	Ends the current ink flow, unwrapping the callstack but without
			affecting any variables. Useful if the ink is (say) in the middle
			a nested tunnel, and you want it to reset so that you can divert
			elsewhere using ChoosePathString(). Otherwise, after finishing
			the content you diverted to, it would continue where it left off.
			Calling this is equivalent to calling -> END in ink.				*/
		void ForceEnd();

		void TrimWhitespaceFromFunctionEnd();

		void AddError(const FString& InMessage, bool InIsWarning);

	private:
		void SetOutputStreamDirty();
	
	private:
		const int32 InkSaveStateVersion;
		const int32 MinCompatibleLoadVersion;
		const FString DefaultFlowName;
		FString CurrentText;

		// Callback for when a state is loaded
		FSimpleMulticastDelegate onDidLoadState;

		TArray<FString> CurrentErrors;
		TArray<FString> CurrentWarnings;
		TArray<TSharedPtr<Ink::FObject>> EvaluationStack;
		Ink::FPointer DivertedPointer;

		int CurrentTurnIndex;
		int StorySeed;
		int PreviousRandom;
		bool DidSafeExit;

		Ink::FStory* Story;

		TArray<FString> CurrentTags;

		TMap<FString, int> VisitCounts;
		TMap<FString, int> TurnIndices;
		bool OutputStreamTextDirty;
		bool OutputStreamTagsDirty;

		TSharedPtr<Ink::FStatePatch> Patch;

		TSharedPtr<FFlow> CurrentFlow;
		TSharedPtr<Ink::FVariableState> VariableState;
		TSharedPtr<TMap<FString, TSharedPtr<FFlow>>> NamedFlows;
	};
}
