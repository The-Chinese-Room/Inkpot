#pragma once

#include "CoreMinimal.h"
#include "PushPop.h"
#include "Json.h"
#include "Pointer.h"

namespace Ink
{
	class FObject;
	class FStory;
	class FThread;


#pragma region Element
	class FElement
	{
	public:
		FElement() = delete;
		FElement(const EPushPopType& InType, const Ink::FPointer& InPointer, bool InExpressionEvaluation = false);
		FElement(const FElement& InOtherElement) = default;
		FElement& operator=(const FElement& InOtherElement) = default;
		~FElement() = default;

		TSharedPtr<Ink::FElement> Copy() const;
		
		bool GetInExpressionEvaluation() const;
		void SetInExpressionEvaluation(bool InExpressionEvaluation);

		Ink::FPointer GetCurrentPointer() const;
		void SetCurrentPointer(Ink::FPointer InCurrentPointer);

		EPushPopType GetType() const;
		void SetType(const Ink::EPushPopType& InType);
	
	public:
		/*	When this callstack element is actually a function evaluation called from the game,
		we need to keep track of the size of the evaluation stack when it was called
		so that we know whether there was any return value.	*/
		int32 EvaluationStackHeightWhenPushed;

		/*	When functions are called, we trim whitespace from the start and end of what
		they generate, so we make sure know where the function's start and end are.	*/
		int32 FunctionStartInOutputStream;

		TMap<FString, TSharedPtr<Ink::FObject>> TemporaryVariables;
	
	private:
		bool bInExpressionEvaluation;
		EPushPopType Type;
		
		Ink::FPointer CurrentPointer;
	};
#pragma endregion Element

	
#pragma region Thread
	class FThread
	{
	public:
		FThread();
		FThread(const FJsonObject& InJSONObj, const Ink::FStory& InStoryContext);
		FThread(const FThread& InOtherThread) = default;
		FThread& operator=(const FThread& InOtherThread) = default;
		~FThread() = default;

		void WriteJson(TJsonWriter<>* InJSONWriter);
		TSharedPtr<Ink::FThread> Copy() const;

		int32 GetIndex() const;
		void SetIndex(int32 InIndex);
		
		Ink::FPointer GetPreviousPointer() const;
		void SetPreviousPointer(Ink::FPointer InPreviousPointer);
		
		TArray<TSharedPtr<Ink::FElement>>& GetCallStack();
	
	private:
		int32 Index;
		Ink::FPointer PreviousPointer;
		TArray<TSharedPtr<Ink::FElement>> Callstack;
	};
#pragma endregion Thread

	
#pragma region CallStack
	class FCallStack
	{
	public:
		FCallStack() = delete;
		FCallStack(Ink::FStory* InStoryContext);
		FCallStack(const Ink::FCallStack& InOtherCallStack);
		FCallStack& operator=(const Ink::FCallStack& InOtherCallStack);
		~FCallStack() = default;

		const TArray<TSharedPtr<Ink::FElement>>& GetElements() const;
		TSharedPtr<Ink::FElement> CurrentElement() const;
		int32 GetCurrentElementIndex() const;
		
		TSharedPtr<Ink::FThread> GetCurrentThread() const;
		void SetCurrentThread(TSharedPtr<Ink::FThread> InCurrentThread);

		void PushThread();
		void PopThread();
		TSharedPtr<Ink::FThread> ForkThread();

		bool CanPopThread() const;
		bool ElementIsEvaluatedFromGame() const;

		void Push(const Ink::EPushPopType& InType, int32 InExternalEvaluationStackHeight = 0, int32 InOutputStreamLengthWithPushed = 0);
		
		int32 GetDepth() const;
		bool CanPopCallstack() const;
		
		bool CanPop(const Ink::EPushPopType& InType = Ink::EPushPopType::Null);
		void Pop(const Ink::EPushPopType& InType = Ink::EPushPopType::Null);
		
		void Reset();

		void WriteJson(TJsonWriter<>* InJSONWriter);
		void SetJsonToken(const FJsonObject& InJSONObject, Ink::FStory* InStoryContext);

		TSharedPtr<Ink::FObject> GetTemporaryVariableWithName(const FString& InName, int32 InContextIndex = -1);
		void SetTemporaryVariable(const FString& InName, TSharedPtr<Ink::FObject> InValue, bool InDeclareNew, int32 InContextIndex = -1);

		// Find the most appropriate context for this variable.
		// Are we referencing a temporary or global variable?
		// Note that the compiler will have warned us about possible conflicts,
		// so anything that happens here should be safe!
		int32 ContextForVariableNamed(const FString& InName);

		TSharedPtr<Ink::FThread> ThreadWithIndex(int32 InIndex);
		FString CallStackTrace() const;
	
	private:
		TArray<TSharedPtr<Ink::FElement>>& GetCallStack() const;
	
	private:
		int32 ThreadCounter;
		Ink::FPointer StartOfRoot;
		TArray<TSharedPtr<Ink::FThread>> Threads;
	};
#pragma endregion CallStack
	
}