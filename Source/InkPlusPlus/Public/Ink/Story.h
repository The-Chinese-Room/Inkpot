#pragma once

#include "CoreMinimal.h"
#include "Ink/Object.h"
#include "Ink/ListDefinitionsOrigin.h"
#include "Ink/Container.h"
#include "Ink/Error.h"
#include "Value.h"
#include "VariableState.h"

DECLARE_DELEGATE_TwoParams( FStoryVariableObserver, const FString&, Ink::FValueType )
DECLARE_DELEGATE_RetVal_OneParam( TSharedPtr<Ink::FValueType>, FStoryExternalFunction, const TArray<Ink::FValueType>& )

namespace Ink
{
	class FChoicePoint;
	class FChoice;
	class FSearchResult;
	class FPath;
	class FPointer;
	class FDebugMetadata;
	class FStoryState;
	class FListDefinition;


	struct INKPLUSPLUS_API FStoryExternalFunctionDef 
	{
		TSharedPtr<FStoryExternalFunction> function;
		bool lookaheadSafe;
    };
	
	class INKPLUSPLUS_API FStory : public Ink::FObject
	{
		enum EOutputStateChange
		{
			NoChange,
			ExtendedBeyondNewline,
			NewlineRemoved
		};

	public:
		FErrorHandler& OnError() { return OnErrorEvent; }

		DECLARE_EVENT(FStory, FOnDidContinueEvent)
		FOnDidContinueEvent& OnDidContinue() { return OnDidContinueEvent; }

		DECLARE_EVENT_OneParam(FStory, FOnMakeChoiceEvent, TSharedPtr<FChoice>)
		FOnMakeChoiceEvent& OnMakeChoice() { return OnMakeChoiceEvent; }

		// Callback for when a function is about to be evaluated
		DECLARE_EVENT_TwoParams(FStory, FOnEvaluateFunctionEvent, const FString&, const TArray<TSharedPtr<Ink::FValueType>>&)
		FOnEvaluateFunctionEvent& OnEvaluateFunction() { return OnEvaluateFunctionEvent; }

		// Callback for when a function has been evaluated
		// This is necessary because evaluating a function can cause continuing
		DECLARE_EVENT_FourParams(FStory, FOnCompleteEvaluateFunctionEvent, const FString&, const TArray<TSharedPtr<Ink::FValueType>>&, const FString&, TSharedPtr<FValueType>)
		FOnCompleteEvaluateFunctionEvent& OnCompleteEvaluateFunction() { return OnCompleteEvaluateFunctionEvent; }

		DECLARE_EVENT_TwoParams(FStory, FChoosePathStringEvent, const FString&, const TArray<TSharedPtr<Ink::FValueType>>&)
		FChoosePathStringEvent& OnChoosePathString() { return OnChoosePathStringEvent; }

		FStory(const FString& jsonString);
		FString ToJson() const;
		void ToJson(FArchive* const Stream) const;
		void ToJson(TJsonWriter<>* InJSONWriter) const;

		void ResetState();
		void ResetErrors();
		void ResetCallstack();
		void ResetGlobals();

		void SwitchFlow(const FString& FlowName) const;
		void RemoveFlow(const FString& FlowName) const;
		void SwitchToDefaultFlow() const;

		FString Continue();
		bool CanContinue() const;
		bool AsyncContinueComplete() const;
		void ContinueAsync(float MillisecondsLimitAsync);
		void ContinueInternal(float MillisecondsLimitAsync = 0);
		bool ContinueSingleStep();
		EOutputStateChange CalculateNewlineOutputStateChange(const FString& prevText, const FString& currText, int prevTagCount, int currTagCount);
		FString ContinueMaximally();
		Ink::FSearchResult ContentAtPath(TSharedPtr<Ink::FPath> InPath) const;
		TSharedPtr<Ink::FContainer> KnotContainerWithName(const FString& InName) const;
		Ink::FPointer PointerAtPath(TSharedPtr<Ink::FPath> InPath) const;

		void StateSnapshot();
		void RestoreStateSnapshot();
		void DiscardSnapshot();

		TSharedPtr<FStoryState> CopyStateForBackgroundThreadSave();
		void BackgroundSaveComplete();

		void Step();

		bool PerformLogicAndFlowControl(TSharedPtr<Ink::FObject> contentObj);
		void ChoosePathString(const FString& InPath, bool resetCallstack, const TArray<TSharedPtr<Ink::FValueType>>& Arguments);
		void ChoosePath(TSharedPtr<FPath> InPath, bool incrementingTurnIndex = true);
		void ChooseChoiceIndex(int ChoiceIndex);
		bool HasFunction(const FString& FunctionName) const;
		TSharedPtr<Ink::FValueType> EvaluateFunction(const FString& FunctionName, const TArray<TSharedPtr<Ink::FValueType>>& Arguments);
		TSharedPtr<Ink::FValueType> EvaluateFunction(const FString& FunctionName, FString& outTextOutput, const TArray<TSharedPtr<Ink::FValueType>>& Arguments);
		TSharedPtr<Ink::FObject> EvaluateExpression(TSharedPtr<Ink::FContainer> exprContainer);
		void CallExternalFunction(const FString& funcName, int numberOfArguments);

		void BindExternalFunctionGeneral(const FString& funcName, TSharedPtr<FStoryExternalFunction> func, bool lookaheadSafe = true);
		void UnbindExternalFunction(const FString& funcName);

		/* // EXTERNAL FUNCTIONS FUNCTIONALITY
		 * 
		 * public delegate object ExternalFunction(object[] args);
		 * object TryCoerce<T>(object value)
		 * public void BindExternalFunction(string funcName, Func<object> func, bool lookaheadSafe=false)
		 * public void BindExternalFunction(string funcName, Action act, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T>(string funcName, Func<T, object> func, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T>(string funcName, Action<T> act, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T1, T2>(string funcName, Func<T1, T2, object> func, bool lookaheadSafe = false)
		 * public void BindExternalFunction<T1, T2>(string funcName, Action<T1, T2> act, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T1, T2, T3>(string funcName, Func<T1, T2, T3, object> func, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T1, T2, T3>(string funcName, Action<T1, T2, T3> act, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T1, T2, T3, T4>(string funcName, Func<T1, T2, T3, T4, object> func, bool lookaheadSafe=false)
		 * public void BindExternalFunction<T1, T2, T3, T4>(string funcName, Action<T1, T2, T3, T4> act, bool lookaheadSafe=false)
		 * public void UnbindExternalFunction(string funcName)
		 * public void ValidateExternalBindings()
		 * void ValidateExternalBindings(Container c, HashSet<string> missingExternals)
		 * void ValidateExternalBindings(Runtime.Object o, HashSet<string> missingExternals)
		 */

		void ObserveAllVariables(TWeakPtr<FStoryVariableObserver> Observer);
		void ObserveVariable(const FString& VariableName, TWeakPtr<FStoryVariableObserver> Observer);
		void ObserveVariables(const TArray<FString> VariableNames, TWeakPtr<FStoryVariableObserver> Observer);
		void RemoveVariableObserver(TWeakPtr<FStoryVariableObserver> Observer = nullptr, const FString& SpecificVariableName = "");

		TArray<FString> GlobalTags() const;
		TArray<FString> TagsForContentAtPath(const FString& InPath) const;
		TArray<FString> TagsAtStartOfFlowContainerWithPathString(const FString& PathString) const;
		FString BuildStringOfHeirarchy() const;
		FString BuildStringOfContainer(FContainer* Container) const;
		
		bool IncrementContentPointer();
		bool TryFollowDefaultInvisibleChoice();
		int NextSequenceShuffleIndex();

		TSharedPtr<Ink::FChoice> ProcessChoice(FChoicePoint* choicePoint);
		bool IsTruthy(TSharedPtr<Ink::FObject> Obj);

		// The latest line of text to be generated from a Continue() call.
		FString GetCurrentText() const;

		/// <summary>
		/// The list of Choice objects available at the current point in
		/// the Story. This list will be populated as the Story is stepped
		/// through with the Continue() method. Once canContinue becomes
		/// false, this list will be populated, and is usually
		/// (but not always) on the final Continue() step.
		/// </summary>
		TArray<TSharedPtr<FChoice>> GetCurrentChoices() const;
		
		/* Gets a list of tags as defined with '#' in source that were seen
		   during the latest Continue() call. */
		const TArray<FString>& GetCurrentTags() const;

		// Gets Any errors generated during evaluation of the Story.
		const TArray<FString>& GetCurrentErrors() const;
		
		// Gets Any warnings generated during evaluation of the Story.
		const TArray<FString>& GetCurrentWarnings() const;

		FString GetCurrentFlowName() const;

		/* Whether the currentErrors list contains any errors.
		/// THIS MAY BE REMOVED - you should be setting an error handler directly
			using Story.onError. */
		bool HasError() const;
		bool HasWarning() const; // Gets Whether the currentWarnings list contains any warnings.
		TSharedPtr<FVariableState> GetVariablesState() const;
		void Error(const FString& InMessage, bool useEndLineNumber = false) const;
		void Warning(const FString& InMessage) const;
		
		TSharedPtr<Ink::FContainer> GetMainContentContainer() const;
		TSharedPtr<Ink::FStoryState> GetStoryState() const;
		TSharedPtr<Ink::FListDefinitionsOrigin> GetListDefinitions() const;
		int GetCurrentLineNumber() const;
	
	public:
		// The current version of the ink story file format.
		const int32 inkVersionCurrent = 20;

		/*	Version numbers are for engine itself and story file, rather
			than the story state save format
			-- old engine, new format: always fail
			-- new engine, old format: possibly cope, based on this number
			When incrementing the version number above, the question you
			should ask yourself is:
			-- Will the engine be able to load an old story file from 
			   before I made these changes to the engine?
			   If possible, you should support it, though it's not as
			   critical as loading old save games, since it's an
			   in-development problem only. */

		// The minimum legacy version of ink that can be loaded by the current version of the code.
		const int32 inkVersionMinimumCompatible = 18;

		// An ink file can provide a fallback functions for when when an EXTERNAL has been left
		// unbound by the client, and the fallback function will be called instead. Useful when
		// testing a story in playmode, when it's not possible to write a client-side C# external
		// function, but you don't want it to fail to run.
		bool allowExternalFunctionFallbacks = false;

	protected:
		void IfAsyncWeCant(const FString& ActivityStr) const;
		void VisitContainer(TSharedPtr<FContainer> Container, bool AtStart);
		void VisitChangedContainersDueToDivert();
		TSharedPtr<Ink::FDebugMetadata> GetCurrentDebugMetadata() const;
		void AddError(const FString& InMessage, bool InIsWarning = false, bool InUseEndLineNumber = false) const;
		void OnVariableStateDidChangeEvent(const FString& VariableName, TSharedPtr<Ink::FObject> NewValueObj);
		void NextContent();
	
	protected:
		FErrorHandler OnErrorEvent;
		FOnDidContinueEvent OnDidContinueEvent;
		FOnMakeChoiceEvent OnMakeChoiceEvent;
		FOnEvaluateFunctionEvent OnEvaluateFunctionEvent;
		FOnCompleteEvaluateFunctionEvent OnCompleteEvaluateFunctionEvent;
		FChoosePathStringEvent OnChoosePathStringEvent;

		bool _asyncContinueActive = false;

		TSharedPtr<Ink::FStoryState> State;
		TSharedPtr<Ink::FStoryState> _stateSnapshotAtLastNewline;
		TSharedPtr<Ink::FContainer> MainContentContainer;
		TSharedPtr<Ink::FContainer> TemporaryEvaluationContainer;
		
		TArray<FString> CurrentTags;
		TArray<FString> CurrentErrors;
		bool bHasError = false;
		TArray<FString> CurrentWarnings;
		bool bHasWarning = false;

		TSharedPtr<FListDefinitionsOrigin> ListDefinitions;
		TMap<FString, TArray<TWeakPtr<FStoryVariableObserver>>> _variableObservers;

		int _recursiveContinueCount = 0;
		bool _asyncSaving = false;
		bool _sawLookaheadUnsafeFunctionAfterNewline = false;

		TMap<FString, FStoryExternalFunctionDef> _externals;
	};
}
