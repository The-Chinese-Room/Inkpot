#pragma once

#include "Object.h"
#include "CoreMinimal.h"


namespace Ink
{
    // New version of tags is dynamic - it constructs the tags
    // at runtime based on BeginTag and EndTag control commands.
    // Plain text that's in the output stream is turned into tags
    // when you do story.currentTags.
    // The only place this is used is when flattening tags down
    // to string in advance, during dynamic string generation if
    // there's a tag embedded in it. See how ControlCommand.EndString
    // is implemented in Story.cs for more details + comment
	class INKPLUSPLUS_API FTag : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FTag; }
		bool CanCastTo(EInkObjectClass inkClass) const override {
			return inkClass == EInkObjectClass::FObject
				|| inkClass == EInkObjectClass::FTag;
		}

		FTag() = delete;
		FTag(const FString& InText);
		FTag(const FTag& InOtherTag) = default;
		FTag& operator=(const FTag& InOtherTag) = default;
		virtual ~FTag() override = default;

		virtual FString ToString() override;

		FString GetText() const;
		void SetText(const FString& InText);
	
	private:
		FString Text;
	};
}
