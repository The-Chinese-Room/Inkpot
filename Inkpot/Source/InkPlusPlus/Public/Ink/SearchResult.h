#pragma once

namespace Ink
{
	class FObject;
	class FContainer;


	class FSearchResult
	{
	public:
		FSearchResult();
		FSearchResult(const FSearchResult& InOtherSearchResult) = default;
		FSearchResult& operator=(const FSearchResult& InOtherSearchResult) = default;
		~FSearchResult() = default;
		
		TSharedPtr<Ink::FObject> GetCorrectObject() const;
		TSharedPtr<Ink::FContainer> GetObjectAsContainer() const;

		bool GetIsApproximate() const;
		void SetIsApproximate(bool InIsApproximate);

		TSharedPtr<Ink::FObject> GetObject() const;
		void SetObject(TSharedPtr<Ink::FObject> InObject);
	
	private:
		bool bIsApproximate;
		TSharedPtr<Ink::FObject> Object;
	};
}