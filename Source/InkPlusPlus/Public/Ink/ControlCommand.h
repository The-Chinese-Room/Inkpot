#pragma once
#include "CoreMinimal.h"
#include "Object.h"

namespace Ink
{
	enum class ECommandType
	{
		NotSet = -1,
		EvalStart,
		EvalOutput,
		EvalEnd,
		Duplicate,
		PopEvaluatedValue,
		PopFunction,
		PopTunnel,
		BeginString,
		EndString,
		NoOp,
		ChoiceCount,
		Turns,
		TurnsSince,
		ReadCount,
		Random,
		SeedRandom,
		VisitIndex,
		SequenceShuffleIndex,
		StartThread,
		Done,
		End,
		ListFromInt,
		ListRange,
		ListRandom,
		BeginTag,
		EndTag,
		// ----
		TOTAL_VALUES
	};
	
	class INKPLUSPLUS_API FControlCommand : public Ink::FObject
	{
	public:
		static EInkObjectClass GetInkObjectClass() { return EInkObjectClass::FControlCommand; }
		bool CanCastTo(EInkObjectClass inkClass) const override { return inkClass == EInkObjectClass::FObject
			|| inkClass == EInkObjectClass::FControlCommand; }

		FControlCommand();
		FControlCommand(const Ink::ECommandType& InCommandType);
		FControlCommand(const Ink::FControlCommand& InOtherControlCommand) = default;
		FControlCommand& operator=(const Ink::FControlCommand& InOtherControlCommand) = default;
		virtual ~FControlCommand() override = default;

		ECommandType GetCommandType() const;
		virtual FString ToString() override;
	protected:
		ECommandType CommandType;
	};
}