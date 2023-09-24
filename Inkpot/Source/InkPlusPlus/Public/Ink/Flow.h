#pragma once
#include "CoreMinimal.h"
#include "Json.h"


namespace Ink
{
	class FStory;
	class FObject;
	class FChoice;
	class FCallStack;
	
	class FFlow
	{
	public:
		FFlow() = delete;
		FFlow(const FString& InName, Ink::FStory* InStory);
		FFlow(const FString& InName, Ink::FStory* InStory, const TSharedPtr<FJsonObject> InJSONObject);

		void WriteJSON(TJsonWriter<>* InJSONWriter);

		// Used both to load old format and current
		void LoadFlowChoiceThreads(const TMap<FString, TSharedPtr<FJsonValue>>& InJSONChoiceThreads,  const Ink::FStory& InStory);

		TSharedPtr<Ink::FCallStack> GetCallStack() const;
		void SetCallStack(const Ink::FCallStack& InCallStack);

		TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> GetCurrentChoices();

		TArray<TSharedPtr<Ink::FObject>>& GetOutputStream();
		void SetOutputStream(const TArray<TSharedPtr<Ink::FObject>>& InOutputStream);
		
		FString GetName() const;
		void SetName(const FString& InName);
	
	private:
		FString Name;
		TSharedPtr<FCallStack> CallStack;
		TArray<TSharedPtr<Ink::FObject>> OutputStream;
		TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> CurrentChoices;
	};
}