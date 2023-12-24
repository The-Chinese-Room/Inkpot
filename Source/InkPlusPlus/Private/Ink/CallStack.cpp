#include "Ink/CallStack.h"

#include "Utility/InkPlusPlusLog.h"
#include "Ink/Container.h"
#include "Ink/JsonSerialisation.h"
#include "Ink/Object.h"
#include "Ink/Pointer.h"
#include "Ink/Story.h"
#include "Ink/Path.h"
#include "Ink/SearchResult.h"
#include "Ink/Value.h"


#pragma region Element
Ink::FElement::FElement(const EPushPopType& InType, const Ink::FPointer& InPointer, bool InExpressionEvaluation /*= false*/)
	: EvaluationStackHeightWhenPushed(0),
	FunctionStartInOutputStream(0),
	bInExpressionEvaluation(InExpressionEvaluation),
	Type(InType),
	CurrentPointer(InPointer)
{
}

bool Ink::FElement::GetInExpressionEvaluation() const
{
	return bInExpressionEvaluation;
}

void Ink::FElement::SetInExpressionEvaluation(bool InExpressionEvaluation)
{
	bInExpressionEvaluation = InExpressionEvaluation;
}

Ink::FPointer Ink::FElement::GetCurrentPointer() const
{
	return CurrentPointer;
}

void Ink::FElement::SetCurrentPointer(Ink::FPointer InCurrentPointer)
{
	CurrentPointer = InCurrentPointer;
}

Ink::EPushPopType Ink::FElement::GetType() const
{
	return Type;
}

void Ink::FElement::SetType(const Ink::EPushPopType& InType)
{
	Type = InType;
}

TSharedPtr<Ink::FElement> Ink::FElement::Copy() const
{
	TSharedPtr<Ink::FElement> element = MakeShared<Ink::FElement>(Type, CurrentPointer, bInExpressionEvaluation);
	element->TemporaryVariables = TemporaryVariables;
	element->EvaluationStackHeightWhenPushed = EvaluationStackHeightWhenPushed;
	element->FunctionStartInOutputStream = FunctionStartInOutputStream;
	return element;
}
#pragma endregion Element


#pragma region Thread
Ink::FThread::FThread()
	: Index(-1),
	PreviousPointer(Ink::FPointer())
{
}

Ink::FThread::FThread(const FJsonObject& InJSONObj, const Ink::FStory& InStoryContext)
	: PreviousPointer(Ink::FPointer())
{
	if (!InJSONObj.TryGetNumberField(TEXT("threadIndex"), Index))
		UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get thread index out of passed in JSON object!"));

	const TArray<TSharedPtr<FJsonValue>>* jsonThreadCallstack;
	if (!InJSONObj.TryGetArrayField(TEXT("callstack"), jsonThreadCallstack))
		UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get thread callstack out of passed in JSON object!"));

	for (const TSharedPtr<FJsonValue>& jsonElementToken : *jsonThreadCallstack)
	{
		const TSharedPtr<FJsonObject>* jsonElementObject;
		if (!jsonElementToken->TryGetObject(jsonElementObject))
		{
			UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get JSON element object out of current JSON element token!"));
			continue;
		}

		uint32 type = 0;
		if (!jsonElementObject->Get()->TryGetNumberField(TEXT("type"), type))
		{
			UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get push pop type out of current JSON element object!"));
		}
		
		Ink::EPushPopType pushPopType = static_cast<Ink::EPushPopType>(type);
		Ink::FPointer pointer = Ink::FPointer::Null();

		FString currentContainerPathString = FString();
		if(jsonElementObject->Get()->TryGetStringField(TEXT("cPath"), currentContainerPathString))
		{
			const TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>(currentContainerPathString);
			Ink::FSearchResult threadPointerResult = InStoryContext.ContentAtPath(path);
		
			pointer.SetContainer(threadPointerResult.GetObjectAsContainer());
			int32 pointerIndex;
			if (!jsonElementObject->Get()->TryGetNumberField(TEXT("idx"), pointerIndex))
			{
				UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get pointer index out of current JSON element object!"));				
			}
			pointer.SetIndex(pointerIndex);

			if (threadPointerResult.GetObject() == nullptr)
			{
				UE_LOG(InkPlusPlus, Error, TEXT("When loading state, internal story location couldn't be found: %s . Has the story changed since this save data was created?"), *currentContainerPathString);
			}
			else if (threadPointerResult.GetIsApproximate())
			{
				const FString warning = FString::Printf(TEXT("When loading state, exact internal story location couldn't be found: %s , so it was approximated to %s to recover. Has the story changed since this save data was created?"), *currentContainerPathString, *pointer.GetContainer()->GetPath()->ToString());
				InStoryContext.Warning(warning);
			}
		}

		bool inExpressionEvaluation = false;
		if (!jsonElementObject->Get()->TryGetBoolField(TEXT("exp"), inExpressionEvaluation))
		{
			UE_LOG(InkPlusPlus, Error, TEXT("Callstack : could not get in expression evaluation out of current JSON element object!"));
		}

		TSharedPtr<Ink::FElement> element = MakeShared<Ink::FElement>(pushPopType, pointer, inExpressionEvaluation);
		
		const TSharedPtr<FJsonObject>* temporaryObjects;
		if (jsonElementObject->Get()->TryGetObjectField(TEXT("temp"), temporaryObjects))
		{
			element->TemporaryVariables = Ink::FJsonSerialisation::JsonObjectToDictionaryRuntimeObjects(*temporaryObjects->Get());
		}
		else
		{
			element->TemporaryVariables.Reset();
		}

		Callstack.Add(element);
	}

	FString previousContentObjectPath = FString();
	if (InJSONObj.TryGetStringField(TEXT("previousContentObject"), previousContentObjectPath))
	{
		const TSharedPtr<Ink::FPath> previousPath = MakeShared<Ink::FPath>(previousContentObjectPath);
		PreviousPointer = InStoryContext.PointerAtPath(previousPath);
	}
}

void Ink::FThread::WriteJson(TJsonWriter<>* InJSONWriter)
{
	InJSONWriter->WriteObjectStart();

	// Callstack
	InJSONWriter->WriteIdentifierPrefix(TEXT("callstack"));
	InJSONWriter->WriteArrayStart();
	for (const TSharedPtr<Ink::FElement>& element : Callstack)
	{
		InJSONWriter->WriteObjectStart();
		if (!element->GetCurrentPointer().IsNull())
		{
			InJSONWriter->WriteValue(TEXT("cPath"), element->GetCurrentPointer().GetContainer()->GetPath()->GetComponentsString());
			InJSONWriter->WriteValue(TEXT("idx"), element->GetCurrentPointer().GetIndex());
		}

		InJSONWriter->WriteValue(TEXT("exp"), element->GetInExpressionEvaluation());
		InJSONWriter->WriteValue(TEXT("type"), static_cast<int32>(element->GetType()));

		if (element->TemporaryVariables.Num() > 0)
		{
			InJSONWriter->WriteIdentifierPrefix(TEXT("temp"));
			Ink::FJsonSerialisation::WriteDictionaryRuntimeObjects(InJSONWriter, element->TemporaryVariables);
		}

		InJSONWriter->WriteObjectEnd();
	}
	InJSONWriter->WriteArrayEnd();

	// Thread Index
	InJSONWriter->WriteValue(TEXT("threadIndex"), Index);

	if (!PreviousPointer.IsNull())
	{
		InJSONWriter->WriteValue(TEXT("previousContentObject"), PreviousPointer.Resolve()->GetPath()->ToString());
	}

	InJSONWriter->WriteObjectEnd();
}

int32 Ink::FThread::GetIndex() const
{
	return Index;
}

void Ink::FThread::SetIndex(int32 InIndex)
{
	Index = InIndex;
}

Ink::FPointer Ink::FThread::GetPreviousPointer() const
{
	return PreviousPointer;
}

void Ink::FThread::SetPreviousPointer(Ink::FPointer InPreviousPointer)
{
	PreviousPointer = InPreviousPointer;
}

TArray<TSharedPtr<Ink::FElement>>& Ink::FThread::GetCallStack()
{
	return Callstack;
}

TSharedPtr<Ink::FThread> Ink::FThread::Copy() const
{
	TSharedPtr<Ink::FThread> thread = MakeShared<Ink::FThread>();
	thread->SetIndex(Index);
	for (const TSharedPtr<Ink::FElement>& element : Callstack)
	{
		thread->Callstack.Add(element->Copy());
	}
	thread->PreviousPointer = PreviousPointer;
	return thread;
}
#pragma endregion Thread


#pragma region CallStack
Ink::FCallStack::FCallStack(Ink::FStory* InStoryContext)
	: ThreadCounter(0)
{
	StartOfRoot = Ink::FPointer::StartOf(nullptr);
	// the c# code equivalent, below, which breaks for us. The function call would always return nullptr here, so our argument replacement is equivalent
	// StartOfRoot = Ink::FPointer::StartOf(InStoryContext->GetRootContentContainer());
	Reset();
}

Ink::FCallStack::FCallStack(const Ink::FCallStack& InOtherCallStack)
{
	Threads.Reserve(InOtherCallStack.Threads.Num());
	for (const TSharedPtr<Ink::FThread>& thread : InOtherCallStack.Threads)
	{
		Threads.Add(thread->Copy());
	}
	ThreadCounter = InOtherCallStack.ThreadCounter;
	StartOfRoot = InOtherCallStack.StartOfRoot;
}

Ink::FCallStack& Ink::FCallStack::operator=(const Ink::FCallStack& InOtherCallStack)
{
	Threads.Reserve(InOtherCallStack.Threads.Num());
	for (const TSharedPtr<Ink::FThread>& thread : InOtherCallStack.Threads)
	{
		Threads.Add(thread->Copy());
	}
	ThreadCounter = InOtherCallStack.ThreadCounter;
	StartOfRoot = InOtherCallStack.StartOfRoot;
	
	return *this;
}

const TArray<TSharedPtr<Ink::FElement>>& Ink::FCallStack::GetElements() const
{
	return GetCallStack();
}

TSharedPtr<Ink::FElement> Ink::FCallStack::CurrentElement() const
{
	if (Threads.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : could not get current element, because Threads array is empty"))
		return nullptr;
	}
	
	const TSharedPtr<Ink::FThread> thread = Threads[Threads.Num() - 1];
	const TArray<TSharedPtr<Ink::FElement>>& callStack = thread->GetCallStack();

	if (callStack.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : could not get current element, because CallStack array is empty"))
		return nullptr;
	}
	
	return callStack[callStack.Num() - 1];
}

int32 Ink::FCallStack::GetCurrentElementIndex() const
{
	const TArray<TSharedPtr<Ink::FElement>> callStack = GetCallStack();
	if (callStack.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : could not get current element index, because CallStack array is empty"))
		return -1;
	}

	return GetCallStack().Num() - 1;
}

void Ink::FCallStack::PushThread()
{
	TSharedPtr<Ink::FThread> newThread = GetCurrentThread()->Copy();
	if (!newThread.IsValid())
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack: failed to push thread, newThread is invalid!"))
		return;
	}
	
	++ThreadCounter;
	newThread->SetIndex(ThreadCounter);
	Threads.Add(newThread);
}

void Ink::FCallStack::PopThread()
{
	if (CanPopThread())
		Threads.Remove(GetCurrentThread());
	else
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack: failed to pop thread, thread cannot be popped!"));
}

TSharedPtr<Ink::FThread> Ink::FCallStack::ForkThread()
{
	TSharedPtr<Ink::FThread> forkedThread = GetCurrentThread()->Copy();
	++ThreadCounter;
	forkedThread->SetIndex(ThreadCounter);
	return forkedThread;
}

bool Ink::FCallStack::CanPopThread() const
{
	return Threads.Num() > 1 && !ElementIsEvaluatedFromGame();
}

bool Ink::FCallStack::ElementIsEvaluatedFromGame() const
{
	return CurrentElement()->GetType() == Ink::EPushPopType::FunctionEvaluationFromGame;
}

void Ink::FCallStack::Push(const Ink::EPushPopType& InType, int32 InExternalEvaluationStackHeight, int32 InOutputStreamLengthWithPushed)
{
	// When pushing to callstack, maintain the current content path, but jump out of expressions by default
	const TSharedPtr<Ink::FElement> element = MakeShared<Ink::FElement>(InType, CurrentElement()->GetCurrentPointer(), false);
	element->EvaluationStackHeightWhenPushed = InExternalEvaluationStackHeight;
	element->FunctionStartInOutputStream = InOutputStreamLengthWithPushed;

	GetCallStack().Add(element);
}

int32 Ink::FCallStack::GetDepth() const
{
	return GetElements().Num();
}

bool Ink::FCallStack::CanPopCallstack() const
{
	return GetCallStack().Num() > 1;
}

bool Ink::FCallStack::CanPop(const Ink::EPushPopType& InType)
{
	if (!CanPopCallstack())
		return false;
	if (InType == Ink::EPushPopType::Null)
		return true;

	return CurrentElement()->GetType() == InType;
}

void Ink::FCallStack::Pop(const Ink::EPushPopType& InType)
{
	if (!CanPop(InType))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : Mismatched push/pop in CallStack!"));
		return;
	}

	TArray<TSharedPtr<Ink::FElement>>& callStack = GetCallStack();
	if (callStack.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to pop CallStack as the CallStack is empty!"));
		return;
	}
		
	callStack.RemoveAt(callStack.Num() - 1);
}

void Ink::FCallStack::WriteJson(TJsonWriter<>* InJSONWriter)
{
	InJSONWriter->WriteObjectStart();
	
	InJSONWriter->WriteIdentifierPrefix(TEXT("threads"));
	InJSONWriter->WriteArrayStart();
	for (const TSharedPtr<Ink::FThread>& thread : Threads)
	{
		thread->WriteJson(InJSONWriter);
	}
	InJSONWriter->WriteArrayEnd();

	InJSONWriter->WriteValue(TEXT("threadCounter"), ThreadCounter);
	
	InJSONWriter->WriteObjectEnd();
}

void Ink::FCallStack::SetJsonToken(const FJsonObject& InJSONObject, Ink::FStory* InStoryContext)
{
	Threads.Reset();

	const TArray<TSharedPtr<FJsonValue>>* jsonThreads;
	if (!InJSONObject.TryGetArrayField(TEXT("threads"), jsonThreads))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to get threads out of the passed in JSON object!"))
		return;
	}

	for (const TSharedPtr<FJsonValue>& jsonThreadToken : *jsonThreads)
	{
		const TSharedPtr<FJsonObject>* jsonThreadObject;
		if (!jsonThreadToken->TryGetObject(jsonThreadObject))
		{
			UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to get thread object out of the JSON Thread Token!"))
			continue;
		}
		TSharedPtr<Ink::FThread> thread = MakeShared<Ink::FThread>(*jsonThreadObject->Get(), *InStoryContext);
		Threads.Add(thread);
	}

	if (!InJSONObject.TryGetNumberField(TEXT("threadCounter"), ThreadCounter))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to set thread counter out of the passed in JSON Object!"))
		return;
	}

	StartOfRoot = Ink::FPointer::StartOf(InStoryContext->GetRootContentContainer());
}

TSharedPtr<Ink::FObject> Ink::FCallStack::GetTemporaryVariableWithName(const FString& InName, int32 InContextIndex /* = -1*/)
{
	if (InContextIndex == -1)
		InContextIndex = GetCurrentElementIndex() + 1;

	TArray<TSharedPtr<FElement>>& callStack = GetCallStack();
	if (callStack.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to get context element out of CallStack, as CallStack is empty!"))
		return nullptr;
	}


	const TSharedPtr<Ink::FElement> contextElement = callStack[InContextIndex - 1];
	const TSharedPtr<Ink::FObject>* object = contextElement->TemporaryVariables.Find(InName);
	if (object != nullptr && object->IsValid())
		return *object;

	return nullptr;
}

void Ink::FCallStack::SetTemporaryVariable(const FString& InName, TSharedPtr<Ink::FObject> InValue, bool InDeclareNew, int32 InContextIndex /* = -1*/)
{
	if (InContextIndex == -1)
		InContextIndex = GetCurrentElementIndex() + 1;

	TArray<TSharedPtr<FElement>>& callStack = GetCallStack();
	if (callStack.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to get context element out of CallStack, as CallStack is empty!"))
		return;
	}

	const TSharedPtr<Ink::FElement> contextElement = callStack[InContextIndex - 1];
	if(!InDeclareNew && !contextElement->TemporaryVariables.Contains(InName))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : could not find temporary variable to set: %s"), *InName);
		return;;
	}

	TSharedPtr<Ink::FObject>* oldValue = contextElement->TemporaryVariables.Find(InName);
	if (oldValue != nullptr && oldValue->IsValid())
		Ink::FValueList::RetainListOriginsForAssignment(*oldValue, InValue);

	contextElement->TemporaryVariables.Add(InName, InValue);
}

int32 Ink::FCallStack::ContextForVariableNamed(const FString& InName)
{
	// Current temporary context?
	// (Shouldn't attempt to access contexts higher in the callstack.)
	if (CurrentElement()->TemporaryVariables.Contains(InName))
		return GetCurrentElementIndex() + 1;

	// Global
	return 0;
}

TSharedPtr<Ink::FThread> Ink::FCallStack::ThreadWithIndex(int32 InIndex)
{
	TSharedPtr<Ink::FThread> *found = Threads.FindByPredicate(
		[&](const TSharedPtr<Ink::FThread>& InThread)
		{
			return InThread->GetIndex() == InIndex;
		}
	);
	return found ? *found : nullptr;
}

FString Ink::FCallStack::CallStackTrace() const
{
	FString trace = FString();

	for (int32 i = 0; i < Threads.Num(); ++i)
	{
		TSharedPtr<FThread> thread = Threads[i];
		const bool isCurrent = (i == (Threads.Num() - 1));
		trace += FString::Printf(TEXT("=== THREAD %d/%d/%s ===\n"), (i+1), Threads.Num(), (isCurrent ? TEXT("(current") : TEXT("")));

		for (int32 j = 0; j < thread->GetCallStack().Num(); ++j)
		{
			TArray<TSharedPtr<Ink::FElement>>& callStack = thread->GetCallStack();
			if (callStack[j]->GetType() == Ink::EPushPopType::Function)
				trace += TEXT("  [FUNCTION] ");
			else
				trace += TEXT("  [TUNNEL] ");

			Ink::FPointer pointer = callStack[j]->GetCurrentPointer();
			if (!pointer.IsNull())
			{
				trace += TEXT("<SOMWHERE IN ");
				trace += pointer.GetContainer()->GetPath()->ToString();
				trace += TEXT(">");
			}
		}
	}

	return trace;
}

TSharedPtr<Ink::FThread> Ink::FCallStack::GetCurrentThread() const
{
	if (Threads.Num() == 0)
	{
		UE_LOG(InkPlusPlus, Error, TEXT("CallStack : failed to get current thread as Threads array is empty"));
		return nullptr;
	}
		
	return Threads[Threads.Num() - 1];
}

void Ink::FCallStack::SetCurrentThread(TSharedPtr<Ink::FThread> InCurrentThread)
{
	checkf(Threads.Num() == 1, TEXT("Shouldn't be directly setting the current thread when we have a stack of them!"));
	Threads.Reset();
	Threads.Add(InCurrentThread);
}

void Ink::FCallStack::Reset()
{
	Threads.Reset();
	Threads.Add(MakeShared<Ink::FThread>());

	const TSharedPtr<Ink::FElement> element = MakeShared<Ink::FElement>(Ink::EPushPopType::Tunnel, StartOfRoot);
	Threads[0]->GetCallStack().Add(element);
}

TArray<TSharedPtr<Ink::FElement>>& Ink::FCallStack::GetCallStack() const
{
	return GetCurrentThread()->GetCallStack();
}
#pragma endregion CallStack
