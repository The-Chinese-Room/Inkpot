#pragma once

#include "CoreMinimal.h"


namespace Ink
{
	class FComponent;
	class FObject;

	static FString GParentID = "^";

	
#pragma region Path
	class INKPLUSPLUS_API FPath
	{
	public:
		FPath();
		FPath(const TArray<TSharedPtr<FComponent>>& InComponents, bool InIsRelative = false);
		FPath(const FString& InComponentsString);

		bool Equals(TSharedPtr<FPath> InOtherPath) const;
	
		TSharedPtr<FComponent> GetComponent(int32 InIndex) const;
		TSharedPtr<FComponent> GetHead() const;
		TSharedPtr<FPath> GetTail() const;

		TSharedPtr<FComponent> GetLastComponent() const;
		bool ContainsNamedComponent() const;
	
		bool IsRelative() const;
		int32 GetLength() const;

		TSharedPtr<FPath> GetPathByAppendingPath(TSharedPtr<FPath> InPathToAppend);
		TSharedPtr<FPath> GetPathByAppendingComponent(TSharedPtr<FComponent> InComponentToAppend) const;

		FString GetComponentsString() const;
		FString ToString() const;
		uint32 GetHashCode() const;

		static TSharedPtr<FPath> Self();

	private:
		void SetComponentsString(const FString& InComponentsString);

	private:
		bool bIsRelative;
		mutable FString _ComponentsString;
		TArray<TSharedPtr<FComponent>> Components;
	};
#pragma endregion Path


#pragma region Component
	class INKPLUSPLUS_API FComponent
	{
	public:
		FComponent(int32 InIndex);
		FComponent(const FString& InName);
		FComponent(const FComponent& InOtherComponent);
		FComponent& operator=(const FComponent& InOtherComponent);
		~FComponent();

		bool operator==(const FComponent& InOtherComponent) const;

		int32 GetIndex() const;
		FString GetName() const;

		bool IsIndex() const;
		bool IsParent() const;

		FString ToString() const;
		static TSharedPtr<Ink::FComponent> ToParent();

		bool Equals(const Ink::FComponent& InOtherComponent) const;

	private:
		int32 Index;
		FString Name;
	};
#pragma endregion Component
}