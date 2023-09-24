#pragma once
#include "StoryException.h"


namespace Ink
{
	class FPath;
	class FContainer;
	class FSearchResult;
    class FDebugMetadata;


	// list of classes for fake dynamic casts
	enum class EInkObjectClass
	{
		FChoicePoint,
		FContainer,
		FControlCommand,
		FDivert,
		FGlue,
		FNativeFunctionCall,
		FObject,
		FTag,
		FValue,
		FValueBool,
		FValueDivertTarget,
		FValueFloat,
		FValueInt,
		FValueList,
		FValueString,
		FValueVariablePointer,
		FVariableAssignment,
		FVariableReference,
		FVoid
	};


	class INKPLUSPLUS_API FObject : public TSharedFromThis<FObject>
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FObject; }
		virtual bool CanCastTo(EInkObjectClass inkClass) const { return inkClass == EInkObjectClass::FObject; }

		FObject();
		FObject(const FObject& InOtherObject) = default;
		FObject& operator=(const FObject& InOtherObject) = default;
		virtual ~FObject();

		template<typename T>
		static TSharedPtr<T> DynamicCastTo(TSharedPtr<FObject> Object);
		virtual FString ToString();
		virtual int32 GetHashCode();
		
		TSharedPtr<Ink::FObject> GetParent() const;
		void SetParent(TSharedPtr<Ink::FObject> InParent);

		TSharedPtr<Ink::FPath> GetPath();
		Ink::FSearchResult ResolvePath(TSharedPtr<Ink::FPath> InPath);

		TSharedPtr<Ink::FContainer> GetRootContentContainer();
		TSharedPtr<Ink::FPath> ConvertPathToRelative(TSharedPtr<Ink::FPath> InGlobalPath);

		FString CompactPathString(TSharedPtr<Ink::FPath> InPath);
		bool HasParent() const;

		virtual FString GetName() const;
		virtual void SetName(const FString& InName);
		bool HasValidName() const;
		
		TSharedPtr<Ink::FDebugMetadata> GetDebugMetadata() const;
        TSharedPtr<Ink::FDebugMetadata> GetOwnDebugMetadata() const;
        void SetDebugMetadata(const Ink::FDebugMetadata& InDebugMetadata);
        int32 DebugLineNumberOfPath(TSharedPtr<Ink::FPath> InPath) const;

		static Ink::FStoryExceptionSubstitute StoryException;
		// StoryException being static is causing problems during the tests as it isn't always resetting between runs. Currently using simple logging instead of this

	protected:
		TSharedPtr<Ink::FObject> Parent;
		TSharedPtr<Ink::FPath> _Path;
		TSharedPtr<Ink::FDebugMetadata> DebugMetadata;
		Ink::FContainer* RootContentContainer;
		FString Name;
	};

	template <typename T>
	TSharedPtr<T> FObject::DynamicCastTo(TSharedPtr<Ink::FObject> Object)
	{
		if (Object.IsValid() && Object->CanCastTo(T::GetInkObjectClass()))
			return StaticCastSharedPtr<T>(Object);
		else
			return nullptr;
	}

}
