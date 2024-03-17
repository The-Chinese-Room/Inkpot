#include "Ink/ControlCommand.h"

Ink::FControlCommand::FControlCommand()
	: CommandType(ECommandType::NotSet)
{
}

Ink::FControlCommand::FControlCommand(const Ink::ECommandType& InCommandType)
	: CommandType(InCommandType)
{
}

Ink::ECommandType Ink::FControlCommand::GetCommandType() const
{
	return CommandType;
}

FString Ink::FControlCommand::ToString()
{
	FString outputString;
	if (CommandType == ECommandType::NotSet)					outputString = TEXT("NotSet");
	else if (CommandType == ECommandType::EvalStart)			outputString = TEXT("EvalStart");
	else if (CommandType == ECommandType::EvalOutput)			outputString = TEXT("EvalOutput");
	else if (CommandType == ECommandType::EvalEnd)				outputString = TEXT("EvalEnd");
	else if (CommandType == ECommandType::Duplicate)			outputString = TEXT("Duplicate");
	else if (CommandType == ECommandType::PopEvaluatedValue) 	outputString = TEXT("PopEvaluatedValue");
	else if (CommandType == ECommandType::PopFunction)			outputString = TEXT("PopFunction");
	else if (CommandType == ECommandType::PopTunnel)			outputString = TEXT("PopTunnel");
	else if (CommandType == ECommandType::BeginString)			outputString = TEXT("BeginString");
	else if (CommandType == ECommandType::EndString)			outputString = TEXT("EndString");
	else if (CommandType == ECommandType::NoOp)					outputString = TEXT("NoOp");
	else if (CommandType == ECommandType::ChoiceCount)			outputString = TEXT("ChoiceCount");
	else if (CommandType == ECommandType::Turns)				outputString = TEXT("Turns");
	else if (CommandType == ECommandType::TurnsSince)			outputString = TEXT("TurnsSince");
	else if (CommandType == ECommandType::ReadCount)			outputString = TEXT("ReadCount");
	else if (CommandType == ECommandType::Random)				outputString = TEXT("Random");
	else if (CommandType == ECommandType::SeedRandom)			outputString = TEXT("SeedRandom");
	else if (CommandType == ECommandType::VisitIndex)			outputString = TEXT("VisitIndex");
	else if (CommandType == ECommandType::SequenceShuffleIndex)	outputString = TEXT("SequenceShuffleIndex");
	else if (CommandType == ECommandType::StartThread)			outputString = TEXT("StartThread");
	else if (CommandType == ECommandType::Done)					outputString = TEXT("Done");
	else if (CommandType == ECommandType::End)					outputString = TEXT("End");
	else if (CommandType == ECommandType::ListFromInt)			outputString = TEXT("ListFromInt");
	else if (CommandType == ECommandType::ListRange)			outputString = TEXT("ListRange");
	else if (CommandType == ECommandType::ListRandom)			outputString = TEXT("ListRandom");
	else if (CommandType == ECommandType::BeginTag)				outputString = TEXT("BeginTag");
	else if (CommandType == ECommandType::EndTag)				outputString = TEXT("EndTag");
    else if (CommandType == ECommandType::TOTAL_VALUES)			outputString = TEXT("TOTAL_VALUES");
	else														outputString = TEXT("UNKNOWN_TYPE");

	return outputString;
}
