#pragma once
#include "CoreMinimal.h"


namespace Ink
{
	class FPath;
	class FObject;
	class FContainer;

	
	/*	Internal structure used to point to a particular / current point in the story.
		Where Path is a set of components that make content fully addressable, this is
		a reference to the current container, and the index of the current piece of 
		content within that container. This scheme makes it as fast and efficient as
		possible to increment the pointer (move the story forwards) in a way that's as
		native to the internal engine as possible. */
	class FPointer
	{
	public:
		FPointer();
		FPointer(int32 InIndex);
		FPointer(TSharedPtr<FContainer> InContainer, int32 InIndex);
		FPointer(const FPointer& InOtherPointer) = default;
		FPointer& operator=(const FPointer& InOtherPointer) = default;
		~FPointer() = default;

		bool IsNull() const;
		TSharedPtr<FPath> GetPath() const;

		TSharedPtr<Ink::FObject> Resolve() const;
		FString ToString() const;

		static FPointer StartOf(TSharedPtr<Ink::FContainer> InContainer);
		
		int32 GetIndex() const;
		void SetIndex(int32 InIndex);
		void IncrementIndex();

		TSharedPtr<Ink::FContainer> GetContainer() const;
		void SetContainer(TSharedPtr<Ink::FContainer> InContainer);

		static FPointer Null();
	
	private:
		int32 Index;
		TSharedPtr<Ink::FContainer> Container;
	};

}