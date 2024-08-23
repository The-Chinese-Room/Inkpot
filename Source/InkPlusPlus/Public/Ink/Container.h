#pragma once

#include "CoreMinimal.h"
#include "Ink/Object.h"


namespace Ink
{
	class FPath;
	class FComponent;
	class FSearchResult;


	enum class ECountFlags
	{
		None = 0x00,
		Visits = 0x01,
		Turns = 0x02,
		CountStartOnly = 0x04
	};
	ENUM_CLASS_FLAGS(ECountFlags);


	class INKPLUSPLUS_API FContainer : public FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FContainer; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FContainer;
		}

		FContainer();
		FContainer(const FContainer& InOtherContainer);
		FContainer& operator=(const FContainer& InOtherContainer);
		virtual ~FContainer() override;

		bool operator==(const FContainer& Other) const
		{
			return Equals(Other);
		}

		bool Equals(const FContainer& Other) const
		{
			return Name.Equals(Other.Name);
		}

		friend uint32 GetTypeHash(const Ink::FContainer& Container)
		{
			const uint32 hash = FCrc::MemCrc32(&Container, sizeof(Ink::FContainer));
			return hash;
		}

		TSharedPtr<TArray<TSharedPtr<Ink::FObject>>> GetContent() const;
		void SetContent(TArray<TSharedPtr<Ink::FObject>> InContent);

		TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> GetNamedContent() const;
		void SetNamedContent(TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> InNamedContent);

		TMap<FString, TSharedPtr<Ink::FObject>> GetNamedOnlyContent() const;
		void SetNamedOnlyContent(const TMap<FString, TSharedPtr<Ink::FObject>>& InNamedOnlyContent);
		
		bool GetVisitsShouldBeCounted() const;

		bool GetTurnIndexShouldBeCounted() const;
		void SetTurnIndexShouldBeCounted(bool InShouldTurnIndexBeCounted);

		bool GetCountingAtStartOnly() const;
		void SetCountingAtStartOnly(bool InShouldCountAtStartOnly);

		uint8 GetCountFlags() const;
		void SetCountFlags(ECountFlags InFlags);

		TSharedPtr<Ink::FPath> GetPathToFirstLeafContent();

		Ink::FSearchResult GetContentAtPath(TSharedPtr<Ink::FPath> InPath, int32 InPartialPathStart = 0, int32 InPartialPathLength = -1);

		void AddToNamedContentOnly(TSharedPtr<Ink::FObject> InNamedContent);
		void RemoveFromNamedContent(const FString &InName);

	private:
		void AddContent(const TArray<TSharedPtr<Ink::FObject>>& ContentList);
		void AddContent(TSharedPtr<Ink::FObject> InObject);
		void TryAddNamedContent(TSharedPtr<Ink::FObject> InObject);

		TSharedPtr<Ink::FObject> GetContentWithPathComponent(const Ink::FComponent& InComponent);
		TSharedPtr<Ink::FPath> GetInternalPathToFirstLeafContent();
	
	private:
		bool bVisitsShouldBeCounted;
		bool bTurnIndexShouldBeCounted;
		bool bCountingAtStartOnly;

		TSharedPtr<Ink::FPath> PathToFirstLeafContent;
		TSharedPtr<Ink::FPath> InternalPathToFirstLeafContent;
		
		TSharedPtr<TArray<TSharedPtr<Ink::FObject>>> Content;
		TSharedPtr<TMap<FString, TSharedPtr<Ink::FObject>>> NamedContent;
	};
}
