#pragma once

#include "CoreMinimal.h"
#include "Ink/Object.h"


namespace Ink
{
	class FPath;
    class FThread;
	
	class INKPLUSPLUS_API FChoice : public FObject
	{
	public:
		FString GetText() const;
		void SetText(const FString& InText);

		FString GetSourcePath() const;
		void SetSourcePath(const FString& InSourcePath);

		TSharedPtr<FPath> GetTargetPath() const;
		void SetTargetPath(TSharedPtr<FPath> InTargetPath);

		int32 GetIndex() const;
		void SetIndex(int32 InIndex);

		int32 GetOriginalThreadIndex() const;
		void SetOriginalThreadIndex(int32 InOriginalThreadIndex);

		bool IsInvisibleDefault() const;
		void SetIsInvisibleDefault(bool InIsInvisibleDefault);

		const TArray<FString> GetTags() const;
		void SetTags(const TArray<FString> &InTags);

		FString GetPathStringOnChoice() const;
		void SetPathStringOnChoice(const FString& InPathStringOnChoice);

		TSharedPtr<Ink::FThread> GetThreadAtGeneration() const;
		void SetThreadAtGeneration(TSharedPtr<Ink::FThread> InThreadAtGeneration);

	private:
		FString Text;
		FString SourcePath;
		TSharedPtr<FPath> TargetPath;

		TSharedPtr<FThread> ThreadAtGeneration;
		
		int32 Index;
		int32 OriginalThreadIndex;
		
		bool bIsInvisibleDefault;

		TArray<FString> Tags;
	};
}