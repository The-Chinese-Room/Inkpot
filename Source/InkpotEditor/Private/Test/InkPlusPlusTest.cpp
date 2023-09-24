// (c) 2021 The Chinese Room, Sumo Digital Ltd. All rights reserved. 

///<Author(s): 		Canute Crasto
///<Date:			22/07/2021
///<Description:	Test : Tests for Ink

#if !UE_BUILD_SHIPPING

#include "Misc/AutomationTest.h"
#include <Compiler/InkCompiler.h>
#include <Misc/FileHelper.h>
#include <Asset/InkpotStoryAsset.h>
#include <Inkpot/InkpotStory.h>
#include <HAL/Platform.h>
#include <Ink/Choice.h>
#include <Ink/StoryState.h>
#include <HAL/FileManagerGeneric.h>
#include <Utility/InkpotLog.h>
#include "Inkpot/Inkpot.h"
#include <Serialization/JsonReader.h>
#include <Templates/SharedPointer.h>
#include <Serialization/JsonSerializer.h>
#include <Dom/JsonValue.h>
#include <Serialization/JsonTypes.h>
#include <Ink/VariableState.h>

static const FString TestFolderPath = FPaths::ProjectPluginsDir() + TEXT("Inkpot/TestInkSource/");
static const FString InkScratchFilePath = InkCompiler::GetScratchDirectory() + "TempInkFile.ink";

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FInkTests, "Inkpot",
	EAutomationTestFlags::EditorContext |
	EAutomationTestFlags::ClientContext |
	EAutomationTestFlags::EngineFilter |
	EAutomationTestFlags::HighPriority
)

static UInkpotStory* CreateNewStory(UInkpotStoryAsset* InStoryAsset)
{
	UInkpot *inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	UInkpotStory *story = inkpot->BeginStory( InStoryAsset );
	return story; 
}

static void GetFilesInFolder(const FString& Directory, TArray<FString>& OutTestFiles)
{
	if (FPaths::DirectoryExists(Directory))
	{
		FString path = Directory + "*.ink";
		FFileManagerGeneric::Get().FindFiles(OutTestFiles, *path, true, false);
	}
}

static bool ExecuteContinueMaximally(UInkpotStory* Story, FString& Output)
{
	if (Story)
	{
		Output = Story->ContinueMaximally();
		return true;
	}
	return false;
}

static bool ExecuteContinue(UInkpotStory* Story, FString& Output)
{
	if (Story)
	{
		Output = Story->Continue();
		return true;
	}
	return false;
}

static bool GetCurrentStoryText(UInkpotStory* Story, FString& Output)
{
	if (Story)
	{
		Output = Story->GetCurrentText();
		return true;
	}
	return false;
}

static bool ExecuteChooseChoice(UInkpotStory* Story, const FString& ChoiceLine)
{
	const FRegexPattern numPattern(TEXT("\\d"));
	int32 choice = -1;
	FRegexMatcher matcher(numPattern, ChoiceLine);
	if (matcher.FindNext())
	{
		int32 b = matcher.GetMatchBeginning();
		int32 e = matcher.GetMatchEnding();

		FString num = ChoiceLine.Mid(b, e - b);

		choice = FCString::Atoi(*num);
	}

	if (Story && choice != UInkpotChoice::BadChoice )
	{
		Story->ChooseChoiceIndex(choice);
		return true;
	}
	return false;
}

static bool IsStoryInstruction(const FString& Instructions)
{
	const TArray<FString> storyInstructions = {
		TEXT("EXECUTE_SWITCH_FLOW"),
		TEXT("EXECUTE_STORY_CONTINUE"),
		TEXT("EXECUTE_CONTINUE_MAXIMALLY"),
		TEXT("TEST_CURRENT_STORY_TEXT"),
		TEXT("TEST_CHOICE_COUNT"),
		TEXT("TEST_CHOICE_TEXT"),
		TEXT("EXECUTE_STORY_CHOICE"),
		TEXT("EXECUTE_SAVE_JSON_STATE"),
		TEXT("EXECUTE_LOAD_JSON_STATE"),
		TEXT("EXECUTE_CHOOSE_STRING_PATH"),
		TEXT("EXECUTE_INK_FUNCTION"),
		TEXT("TEST_CURRENT_TAGS"),
		TEXT("TEST_STORY_GLOBAL_TAGS"),
		TEXT("TEST_STORY_EVALUATION_STACK_COUNT"),
		TEXT("TEST_TAG_FOR_PATH"),
		TEXT("TEST_RANDOM_LIST_CONTINUE"),
		TEXT("TEST_PATH_VISIT_COUNT"),
		TEXT("TEST_CONTINUE_CONTAINS"),
		TEXT("SET_VARIABLE"),
		TEXT("TEST_VARIABLE"),
		TEXT("EXPECTED_EXCEPTION"),
		TEXT("OBSERVE_VARIABLE"),
		TEXT("TEST_OBSERVED_VARIABLE"),
		TEXT("TEST_OBSERVER_CALL_COUNT")
	};

	return storyInstructions.Contains(Instructions);
}



void FInkTests::GetTests(TArray<FString>& OutBeautifiedNames, TArray <FString>& OutTestCommands) const
{
	TArray<FString> fileNames;
	GetFilesInFolder(TestFolderPath, fileNames);
	for (const FString& fileName : fileNames)
	{		
		OutBeautifiedNames.Add(FPaths::GetBaseFilename(fileName));
		OutTestCommands.Add(FPaths::GetBaseFilename(fileName));
	}
}

bool FInkTests::RunTest(const FString& InkTestName)
{
	const FString includeSourceFolder = FPaths::ProjectPluginsDir() + TEXT("Inkpot/TestInkSource/Includes/");
	const FString includeDestinationFolder = InkCompiler::GetScratchDirectory() + "Includes/";
	InkCompiler::FlushScratchDirectory();
	InkCompiler::CopyFilesMatchingFilter(includeSourceFolder, includeDestinationFolder, "*.ink");

	const FString testScriptStartPhrase = TEXT("INK_TEST_START");
	const FString testStoryScriptStartPhrase = TEXT("INK_TEST_STORY_START");
	const FString testEndPhrase = TEXT("INK_TEST_END");


	FString jsonState = TEXT("");
	FString continueOutput = TEXT("");

	TSharedPtr<FStoryVariableObserver> observer = MakeShared<FStoryVariableObserver>();

	FString testFilePath = TestFolderPath + InkTestName + ".ink";
	FString fileContents;
	if (FFileHelper::LoadFileToString(fileContents, *testFilePath))
	{
		int32 testScriptStartPhraseIndex = fileContents.Find(testScriptStartPhrase, ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
		int32 testStoryScriptStartPhraseIndex = fileContents.Find(testStoryScriptStartPhrase, ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);

		// UINT_MAX as -1. INDEX_NONE = -1
		bool bCompileAsStory = (uint32)testStoryScriptStartPhraseIndex < (uint32)testScriptStartPhraseIndex;

		FString startPhrase = bCompileAsStory ? testStoryScriptStartPhrase : testScriptStartPhrase;
		int32 testPhraseStartIndex = bCompileAsStory ? testStoryScriptStartPhraseIndex : testScriptStartPhraseIndex;
		int32 testPhraseEndIndex = fileContents.Find(testEndPhrase, ESearchCase::CaseSensitive);

		const bool shouldCountVisits = fileContents.Contains("TEST_PATH_VISIT_COUNT");

		UInkpotStory* story = nullptr;
		UInkpotStoryAsset* storyAsset = nullptr;

		Ink::FValueType ObserverVariable;
		int32 ObserverCallCount = 0;

		TArray<FString> compileErrors, compileWarnings;
		FString compiledJSON;
		bool bCompileSuccess = InkCompiler::CompileInkString(fileContents, InkScratchFilePath, compiledJSON, compileErrors, compileWarnings, shouldCountVisits);

		if (bCompileAsStory && bCompileSuccess)
		{
			storyAsset = NewObject<UInkpotStoryAsset>();
			storyAsset->SetCompiledJSON(compiledJSON);
			story = CreateNewStory(storyAsset);
		}

		while (testPhraseStartIndex != INDEX_NONE && testPhraseEndIndex != INDEX_NONE)
		{
			FString testStr = fileContents.Mid(testPhraseStartIndex + startPhrase.Len(), testPhraseEndIndex - (testPhraseStartIndex + startPhrase.Len()));

			TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(testStr);

			TArray< TSharedPtr<FJsonValue> > Array;
			if (!FJsonSerializer::Deserialize(Reader, Array))
			{
				INKPOT_ERROR("Failed to Deserialize JSON test script: %s", *InkTestName);
				return false;
			}

			int32 jsonParsedInt = 0;
			FString jsonParsedString = TEXT("");
			for (TSharedPtr<FJsonValue> element : Array)
			{
				const TSharedPtr<FJsonObject>& testInstruction = element->AsObject();


				TArray<FString> keys;
				testInstruction->Values.GetKeys(keys);
				if (keys.Num() != 1)
				{
					INKPOT_ERROR("Invalid test script, expected only one field per instruction: %s", *InkTestName);
					return false;
				}

				const FString& instruction = keys[0];
				if (!IsStoryInstruction(instruction))
				{
					if (testInstruction->HasField(TEXT("TEST_ERROR_EQUAL")))
					{
						if (testInstruction->TryGetNumberField(TEXT("TEST_ERROR_EQUAL"), jsonParsedInt))
						{
							int32 errors = jsonParsedInt;
							bool instructionSuccess = errors == compileErrors.Num();
							if (!instructionSuccess)
							{
								INKPOT_ERROR("%s : TEST_ERROR_EQUAL are not as expected. \nExpected: %d\nActual__: %d", *InkTestName, errors, compileErrors.Num());
								return false;
							}
						}
					}
					else if (testInstruction->HasField(TEXT("TEST_ERROR_TEXT")))
					{
						if (testInstruction->TryGetStringField(TEXT("TEST_ERROR_TEXT"), jsonParsedString))
						{
							FString errorString = jsonParsedString;
							bool bFoundError = false;
							for (FString error : compileErrors)
							{
								if (error.Contains(errorString))
								{
									bFoundError = true;
								}
							}
							if (!bFoundError)
							{
								INKPOT_ERROR("%s : TEST_ERROR_TYPE are not as expected. Expected error : %s ", *InkTestName, *errorString);
								return false;
							}
						}
					}
					else if (testInstruction->HasField(TEXT("TEST_WARNING_TEXT")))
					{
						if (testInstruction->TryGetStringField(TEXT("TEST_WARNING_TEXT"), jsonParsedString))
						{
							FString warningString = jsonParsedString;
							bool bFoundError = false;
							for (FString warning : compileWarnings)
							{
								if (warning.Contains(warningString))
								{
									bFoundError = true;
								}
							}
							if (!bFoundError)
							{
								INKPOT_ERROR("%s : TEST_ERROR_TYPE are not as expected. Expected error : %s ", *InkTestName, *warningString);
								return false;
							}
						}
					}
					else if (testInstruction->HasField(TEXT("TEST_WARNING_EQUAL")))
					{
						if (testInstruction->TryGetNumberField(TEXT("TEST_WARNING_EQUAL"), jsonParsedInt))
						{
							int32 warnings = jsonParsedInt;
							if (warnings != compileWarnings.Num())
							{
								INKPOT_ERROR("%s : TEST_WARNING_EQUAL are not as expected. \nExpected: %d\nActual__: %d", *InkTestName, warnings, compileWarnings.Num());
								return false;
							}
						}
					}
					else if (testInstruction->HasField(TEXT("COMPILE_STORY_STRING")))
					{
						if (testInstruction->TryGetStringField(TEXT("COMPILE_STORY_STRING"), jsonParsedString))
						{
							FString compiledStringJSON;
							compileErrors.Empty();
							compileWarnings.Empty();
							InkCompiler::CompileInkString(jsonParsedString, InkScratchFilePath, compiledStringJSON, compileErrors, compileWarnings);
							
							storyAsset = NewObject<UInkpotStoryAsset>();
							storyAsset->SetCompiledJSON( compiledStringJSON );
							story = CreateNewStory(storyAsset);

							if (story == nullptr)
							{
								INKPOT_ERROR("Failed to Compile test story: %s", *InkTestName);
								return false;
							}
						}
					}
					else if (testInstruction->HasField(TEXT("COMPILE_STRING")))
					{
						if (testInstruction->TryGetStringField(TEXT("COMPILE_STRING"), jsonParsedString))
						{
							compileErrors.Empty();
							compileWarnings.Empty();
							InkCompiler::CompileInkString(jsonParsedString, InkScratchFilePath, compiledJSON, compileErrors, compileWarnings);
						}
					}
					else if (testInstruction->HasField(TEXT("EXECUTE_RECOMPILE_STORY")))
					{
						compileErrors.Empty();
						compileWarnings.Empty();
						InkCompiler::CompileInkFile(testFilePath, InkScratchFilePath, compiledJSON, compileErrors, compileWarnings);

						storyAsset = NewObject<UInkpotStoryAsset>();
						storyAsset->SetCompiledJSON(compiledJSON);
						story = CreateNewStory(storyAsset);

						if (story == nullptr)
						{
							INKPOT_ERROR("Failed to Compile test script: %s", *InkTestName);
							return false;
						}
					}

				}
				else
				{
					if (story != nullptr)
					{
						if (testInstruction->HasField(TEXT("EXECUTE_SWITCH_FLOW")))
						{
							FString switchText;
							if (testInstruction->TryGetStringField(TEXT("EXECUTE_SWITCH_FLOW"), switchText))
							{
								story->SwitchFlow(switchText);
							}
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_STORY_CONTINUE")))
						{
							if (testInstruction->TryGetStringField(TEXT("EXECUTE_STORY_CONTINUE"), jsonParsedString))
							{
								FString expected = jsonParsedString;
								FString continueText;
								if (ExecuteContinue(story, continueText))
								{
									continueOutput = continueText;
									expected.ReplaceInline(TEXT("\n"), TEXT(""));
									continueText.ReplaceInline(TEXT("\n"), TEXT(""));

									const bool executeSuccess = (expected != "-1") ? continueText.Equals(expected) : true;
									if (!executeSuccess)
									{
										INKPOT_ERROR("%s : EXECUTE_STORY_CONTINUE are not as expected. \nExpected: %s\nActual__: %s", *InkTestName, *expected, *continueText);
										return false;
									}
								}
								else
								{
									INKPOT_ERROR("%s : EXECUTE_STORY_CONTINUE Executing Continue() failed ", *InkTestName);
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_CONTINUE_MAXIMALLY")))
						{
							if (testInstruction->TryGetStringField(TEXT("EXECUTE_CONTINUE_MAXIMALLY"), jsonParsedString))
							{
								FString expected = jsonParsedString;
								FString continueText;
								if (ExecuteContinueMaximally(story, continueText))
								{
									continueOutput = continueText;
									expected.ReplaceInline(TEXT("\n"), TEXT(""));
									continueText.ReplaceInline(TEXT("\n"), TEXT(""));

									const bool executeSuccess = (expected != "-1") ? continueText.Equals(expected) : true;
									if (!executeSuccess)
									{
										INKPOT_ERROR("%s : EXECUTE_CONTINUE_MAXIMALLY are not as expected.\nExpected: %s\nActual__: %s", *InkTestName, *expected, *continueText);
										return false;
									}
								}
								else
								{
									INKPOT_ERROR("%s : EXECUTE_CONTINUE_MAXIMALLY Executing ContinueMaximally() ailed ", *InkTestName);
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_CURRENT_STORY_TEXT")))
						{
							if (testInstruction->TryGetStringField(TEXT("TEST_CURRENT_STORY_TEXT"), jsonParsedString))
							{
								FString expected = jsonParsedString;
								FString actual;
								if (GetCurrentStoryText(story, actual))
								{
									expected.ReplaceInline(TEXT("\n"), TEXT(""));
									actual.ReplaceInline(TEXT("\n"), TEXT(""));
									const bool success = actual.Equals(expected);
									if (!success)
									{
										INKPOT_ERROR("%s : TEST_CURRENT_STORY_TEXT not as expected. \nExpected: %s\nActual__: %s", *InkTestName, *expected, *actual);
										return false;
									}
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_CHOICE_COUNT")))
						{
							if (testInstruction->TryGetNumberField(TEXT("TEST_CHOICE_COUNT"), jsonParsedInt))
							{
								int32 choiceCount = jsonParsedInt;
								int32 storyChoiceCount = story->GetCurrentChoices().Num();

								const bool success = (choiceCount == storyChoiceCount);
								if (!success)
								{
									INKPOT_ERROR("%s : TEST_CHOICE_COUNT are not as expected. \nExpected: %d\nActual__: %d", *InkTestName, choiceCount, storyChoiceCount );
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_STORY_EVALUATION_STACK_COUNT")))
						{
							if (testInstruction->TryGetNumberField(TEXT("TEST_STORY_EVALUATION_STACK_COUNT"), jsonParsedInt))
							{
								int32 expectedStackCount = jsonParsedInt;
								int32 actualStackCount = story->GetStoryInternal()->GetStoryState()->GetEvaluationStack().Num();
								const bool success = expectedStackCount == actualStackCount;
								if (!success)
								{
									INKPOT_ERROR("%s : TEST_STORY_EVALUATION_STACK_COUNT are not as expected. \nExpected: %d\nActual__: %d", *InkTestName, expectedStackCount, actualStackCount);
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_CHOICE_TEXT")))
						{
							const TArray<TSharedPtr<FJsonValue>>* choiceArray;
							if (testInstruction->TryGetArrayField(TEXT("TEST_CHOICE_TEXT"), choiceArray))
							{
								if (choiceArray->Num() != 2)
								{
									INKPOT_ERROR("%s : TEST_CHOICE_TEXT expected a choice and choice text in array eg [2, choiceText] ", *InkTestName);
									return false;
								}

								int32 choiceIndex;
								if ((*choiceArray)[0]->TryGetNumber(choiceIndex))
								{
									FString choiceText;
									if ((*choiceArray)[1]->TryGetString(choiceText))
									{
										const TArray<UInkpotChoice*> &choices = story->GetCurrentChoices();
										if (choiceIndex < choices.Num())
										{
											FString expectedChoiceText = choices[choiceIndex]->GetString();
											const bool success = choiceText.Equals(expectedChoiceText);
											if (!success)
											{
												INKPOT_ERROR("%s : TEST_CHOICE_TEXT are not as expected. \nExpected: %s\nActual__: %s", *InkTestName, *expectedChoiceText, *choiceText);
												return false;
											}
										}
										else
										{
											INKPOT_ERROR("%s : TEST_CHOICE_TEXT not enough choices.", *InkTestName);
											return false;
										}
									}
									else
									{
										INKPOT_ERROR("%s : TEST_CHOICE_TEXT could not parse choice string.", *InkTestName);
										return false;
									}
								}
								else
								{
									INKPOT_ERROR("%s : TEST_CHOICE_TEXT could not parse choice index.", *InkTestName);
									return false;
								}
							}
							else
							{
								INKPOT_ERROR("%s : TEST_CHOICE_TEXT could not parse choice array.", *InkTestName);
								return false;
							}

						}
						else if (testInstruction->HasField(TEXT("EXECUTE_STORY_CHOICE")))
						{
							if (testInstruction->TryGetNumberField(TEXT("EXECUTE_STORY_CHOICE"), jsonParsedInt))
							{
								int32 choiceIndex = jsonParsedInt;
								story->ChooseChoiceIndex(choiceIndex);
							}
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_SAVE_JSON_STATE")))
						{
							INKPOT_ERROR("%s : EXECUTE_SAVE_JSON_STATE has not been implemented.", *InkTestName);
							//TSharedPtr<Ink::FStoryState> state = story->State(); // TODO: json loading/saving
							//jsonState = state->ToJson();
							return false;
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_LOAD_JSON_STATE")))
						{
							if (!jsonState.IsEmpty())
							{
								INKPOT_ERROR( "%s : EXECUTE_LOAD_JSON_STATE has not been implemented.", *InkTestName );
								//story->State()->LoadJSON(jsonState); // TODO: json loading/saving
								return false;
							}
							else
							{
								INKPOT_ERROR("%s, Missing saved JSON, Call EXECUTE_SAVE_JSON_STATE before this", *InkTestName);
								return false;
							}
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_CHOOSE_STRING_PATH")))
						{
							if (testInstruction->TryGetStringField(TEXT("EXECUTE_CHOOSE_STRING_PATH"), jsonParsedString))
							{
								FString pathString = jsonParsedString;
								story->ChoosePath(pathString);
							}
						}
						else if (testInstruction->HasField(TEXT("EXECUTE_INK_FUNCTION")))
						{
							bool success = false;
							const TSharedPtr<FJsonObject>* inkFunctionObject;
							if (testInstruction->TryGetObjectField(TEXT("EXECUTE_INK_FUNCTION"), inkFunctionObject))
							{
								FString functionName;
								if ((*inkFunctionObject)->TryGetStringField(TEXT("FUNCTION_NAME"), functionName))
								{
									FString expectedFunctionOutput;
									if ((*inkFunctionObject)->TryGetStringField(TEXT("FUNCTION_OUTPUT"), expectedFunctionOutput))
									{
										TArray<TSharedPtr<Ink::FValueType>> vars;
										const TArray< TSharedPtr<FJsonValue> >* jsonValues;
										if ((*inkFunctionObject)->TryGetArrayField(TEXT("ARGS"), jsonValues))
										{
											for (TSharedPtr<FJsonValue> jsonValue : *jsonValues)
											{
												if (jsonValue->Type == EJson::Number)
												{
													int32 asNumber = jsonValue->AsNumber();
													vars.Add(MakeShared<Ink::FValueType>(asNumber));
												}
												else if (jsonValue->Type == EJson::String)
												{
													FString asString = jsonValue->AsString();
													vars.Add(MakeShared<Ink::FValueType>(asString));
												}
												else if (jsonValue->Type == EJson::Boolean)
												{
													bool asBool = jsonValue->AsBool();
													vars.Add(MakeShared<Ink::FValueType>(asBool));
												}
											}

											TSharedPtr<Ink::FValueType> var = story->GetStoryInternal()->EvaluateFunction(functionName, vars);

											if (!var.IsValid())
											{
												success = expectedFunctionOutput.Equals("");
											}
											else if (var->HasSubtype<FString>())
											{
												const FString actualFunctionOutput = var->GetSubtype<FString>();
												success = actualFunctionOutput.Equals(expectedFunctionOutput);
											}
											else if (var->HasSubtype<int32>())
											{
												const int32 expected = FCString::Atoi(*expectedFunctionOutput);
												const int32 actual = var->GetSubtype<int32>();
												success = actual == expected;
											}
										}
									}
								}
							}

							if (!success)
								return false;
						}
						else if (testInstruction->HasField(TEXT("TEST_CURRENT_TAGS")))
						{
							const TArray< TSharedPtr<FJsonValue> >* expectedTags;
							if (testInstruction->TryGetArrayField(TEXT("TEST_CURRENT_TAGS"), expectedTags))
							{
								const TArray<FString> &actualTags = story->GetCurrentTags();
								if (expectedTags->Num() != actualTags.Num())
								{
									INKPOT_ERROR("%s : TEST_CURRENT_TAGS Tag count not equal: %s, \nExpected: %d\nActual__: %d", *InkTestName, expectedTags->Num(), actualTags.Num());
									return false;
								}

								for (int32 i = 0; i < expectedTags->Num(); i++)
								{
									TSharedPtr<FJsonValue> value = (*expectedTags)[i];
									if (value->Type != EJson::String)
									{
										INKPOT_ERROR("%s : TEST_CURRENT_TAGS Tag is not a string, check test script JSON", *InkTestName, expectedTags->Num(), actualTags.Num());
										return false;
									}

									const bool success = value->AsString().Equals(actualTags[i]);
									if (!success)
									{
										INKPOT_ERROR("%s : TEST_CURRENT_TAGS tag string did not match. \nExpected: %s\nActual__: %s", *InkTestName, *value->AsString(), *actualTags[i]);
										return false;
									}
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_STORY_GLOBAL_TAGS")))
						{
							const TArray< TSharedPtr<FJsonValue> >* expectedTags;
							if (testInstruction->TryGetArrayField(TEXT("TEST_STORY_GLOBAL_TAGS"), expectedTags))
							{
								TArray<FString> actualTags = story->GlobalTags();
								if (expectedTags->Num() != actualTags.Num())
								{
									INKPOT_ERROR("%s : TEST_STORY_GLOBAL_TAGS Tag count not equal: %s, \nExpected: %d\nActual__: %d", *InkTestName, expectedTags->Num(), actualTags.Num());
									return false;
								}

								for (int32 i = 0; i < expectedTags->Num(); i++)
								{
									TSharedPtr<FJsonValue> value = (*expectedTags)[i];
									if (value->Type != EJson::String)
									{
										INKPOT_ERROR("%s : TEST_STORY_GLOBAL_TAGS Tag is not a string, check test script JSON", *InkTestName, expectedTags->Num(), actualTags.Num());
										return false;
									}

									const bool success = value->AsString().Equals(actualTags[i]);
									if (!success)
									{
										INKPOT_ERROR("%s : TEST_STORY_GLOBAL_TAGS tag string did not match. \nExpected: %s\nActual__: %s", *InkTestName, *value->AsString(), *actualTags[i]);
										return false;
									}
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_TAG_FOR_PATH")))
						{
							const TSharedPtr<FJsonObject>* tagForPathObject;
							if (testInstruction->TryGetObjectField(TEXT("TEST_TAG_FOR_PATH"), tagForPathObject))
							{
								FString path;
								if ((*tagForPathObject)->TryGetStringField(TEXT("PATH"), path))
								{
									TArray<FString> actualTags = story->TagsForContentAtPath(path);
									TArray<FString> expectedTags;
									if ((*tagForPathObject)->TryGetStringArrayField(TEXT("TAGS"), expectedTags))
									{
										if (expectedTags.Num() != actualTags.Num())
										{
											INKPOT_ERROR("%s : TEST_TAG_FOR_PATH Tag count not equal: %s, \nExpected: %d\nActual__: %d", *InkTestName, expectedTags.Num(), actualTags.Num());
											return false;
										}

										for (int32 i = 0; i < expectedTags.Num(); i++)
										{
											const bool success = expectedTags[i].Equals(expectedTags[i]);
											if (!success)
											{
												INKPOT_ERROR("%s : TEST_TAG_FOR_PATH tag string did not match. \nExpected: %s\nActual__: %s", *InkTestName, *expectedTags[i], *actualTags[i]);
												return false;
											}
										}
									}
								}
								else
								{
									INKPOT_ERROR("%s : TEST_TAG_FOR_PATH Could not find PATH in JSON object", *InkTestName);
									return false;
								}
							}
							else
							{
								INKPOT_ERROR("%s : TEST_TAG_FOR_PATH. Could not Deserialize to object ", *InkTestName);
								return false;
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_RANDOM_LIST_CONTINUE")))
						{
							TArray<FString> expectedRandomOutputs;
							if (testInstruction->TryGetStringArrayField(TEXT("TEST_RANDOM_LIST_CONTINUE"), expectedRandomOutputs))
							{
								if (!continueOutput.IsEmpty())
								{
									const bool success = expectedRandomOutputs.Contains(continueOutput);
									if (!success)
									{
										INKPOT_ERROR("%s : TEST_RANDOM_LIST did not contain expected output", *InkTestName);
										return false;
									}
								}
								else
								{
									INKPOT_ERROR("%s : TEST_RANDOM_LIST Call EXECUTE_STORY_CONTINUE before testing random", *InkTestName);
									return false;
								}
							}
							else
							{
								INKPOT_ERROR("%s : TEST_RANDOM_LIST Expected a string array", *InkTestName);
								return false;
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_PATH_VISIT_COUNT")))
						{
							const TSharedPtr<FJsonObject>* visitPathObject;
							if (testInstruction->TryGetObjectField(TEXT("TEST_PATH_VISIT_COUNT"), visitPathObject))
							{
								FString path;
								if ((*visitPathObject)->TryGetStringField(TEXT("PATH"), path))
								{
									int32 actualVisitCount = story->GetStoryInternal()->GetStoryState()->VisitCountAtPathString(path);
									int32 expectedVisitCount = 0;
									if ((*visitPathObject)->TryGetNumberField(TEXT("EXPECTED_COUNT"), expectedVisitCount))
									{
										const bool success = actualVisitCount == expectedVisitCount;
										if (!success)
										{
											INKPOT_ERROR("%s : TEST_PATH_VISIT_COUNT \nExpected: %d\nActual__: %d", *InkTestName, expectedVisitCount, actualVisitCount);
											return false;
										}
									}
								}
								else
								{
									INKPOT_ERROR("%s : TEST_PATH_VISIT_COUNT Could not find PATH in JSON object", *InkTestName);
									return false;
								}
							}
							else
							{
								INKPOT_ERROR("%s : TEST_PATH_VISIT_COUNT. Could not Deserialize to object ", *InkTestName);
								return false;
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_CONTINUE_CONTAINS")))
						{
							if (testInstruction->TryGetStringField(TEXT("TEST_CONTINUE_CONTAINS"), jsonParsedString))
							{
								FString expected = jsonParsedString;
								if (!continueOutput.IsEmpty())
								{
									const bool success = continueOutput.Contains(expected);
									if (!success)
									{
										INKPOT_ERROR("%s : TEST_CONTINUE_CONTAINS did not contain expected output", *InkTestName);
										return false;
									}
								}
								else
								{
									INKPOT_ERROR("%s : TEST_CONTINUE_CONTAINS Call EXECUTE_STORY_CONTINUE/EXECUTE_CONTINUE_MAXIMALLY before testing random", *InkTestName);
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("SET_VARIABLE")))
						{
							const TSharedPtr<FJsonObject>* setVariableObject;
							if (testInstruction->TryGetObjectField(TEXT("SET_VARIABLE"), setVariableObject))
							{
								FString variableName;
								if ((*setVariableObject)->TryGetStringField(TEXT("VARIABLE_NAME"), variableName))
								{
									FString variableType;
									if ((*setVariableObject)->TryGetStringField(TEXT("VARIABLE_TYPE"), variableType))
									{
										if (variableType.Equals("Int"))
										{
											int32 intValue;
											if ((*setVariableObject)->TryGetNumberField(TEXT("VARIABLE_VALUE"), intValue))
											{
												story->SetInt(variableName, intValue);
											}
										}
										else if (variableType.Equals("Float"))
										{
											double floatValue;
											if ((*setVariableObject)->TryGetNumberField(TEXT("VARIABLE_VALUE"), floatValue))
											{
												story->SetFloat( variableName, static_cast<float>(floatValue));
											}
										}
										else if (variableType.Equals("String"))
										{
											FString stringValue;
											if ((*setVariableObject)->TryGetStringField(TEXT("VARIABLE_VALUE"), stringValue))
											{
												story->SetString(variableName, stringValue);
											}
										}
										else if (variableType.Equals("null"))
										{
											FString stringValue;
											if ((*setVariableObject)->TryGetStringField(TEXT("VARIABLE_VALUE"), stringValue))
											{
												story->SetEmpty(variableName);
											}
										}
									}
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_VARIABLE")))
						{
							TArray<FString> variables;
							story->GetVariableKeys(variables);

							const TSharedPtr<FJsonObject>* testVariableObject;
							if (testInstruction->TryGetObjectField(TEXT("TEST_VARIABLE"), testVariableObject))
							{
								FString variableName;
								if ((*testVariableObject)->TryGetStringField(TEXT("VARIABLE_NAME"), variableName))
								{
									FString variableType;
									if ((*testVariableObject)->TryGetStringField(TEXT("VARIABLE_TYPE"), variableType))
									{
										if (variableType.Equals("Int"))
										{
											int32 intValue;
											if ((*testVariableObject)->TryGetNumberField(TEXT("VARIABLE_VALUE"), intValue))
											{
												int32 storysIntValue;
												bool success = story->GetVariable<int32, Ink::FValueInt>(variableName, Ink::EValueType::Int, storysIntValue );
												success &= ( intValue == storysIntValue );
												if (!success)
												{
													INKPOT_ERROR("%s : TEST_VARIABLE not as expected. \nExpected: %d\nActual__: %d", *InkTestName, storysIntValue, intValue);
													return false;
												}
											}
											else
											{
												INKPOT_ERROR("%s : TEST_VARIABLE Could not parse VARIABLE_VALUE as Int", *InkTestName);
											}
										}
										else if (variableType.Equals("Float"))
										{
											double doubleValue;
											if ((*testVariableObject)->TryGetNumberField(TEXT("VARIABLE_VALUE"), doubleValue))
											{
												float storysFloatValue;
												bool success = story->GetVariable<float, Ink::FValueFloat>(variableName, Ink::EValueType::Float, storysFloatValue );
												success &= ((float)doubleValue == storysFloatValue);
												if (!success)
												{
													INKPOT_ERROR("%s : TEST_VARIABLE not as expected. \nExpected: %f\nActual__: %f", *InkTestName, storysFloatValue, (float)doubleValue);
													return false;
												}
											}
											else
											{
												INKPOT_ERROR("%s : TEST_VARIABLE Could not parse VARIABLE_VALUE as Float", *InkTestName);
											}
										}
										else if (variableType.Equals("String"))
										{
											FString stringValue;
											if ((*testVariableObject)->TryGetStringField(TEXT("VARIABLE_VALUE"), stringValue))
											{
												FString storysStringValue;
												bool success = story->GetVariable<FString, Ink::FValueString>( variableName, Ink::EValueType::String, storysStringValue );
												success &= (stringValue == storysStringValue);
												if (!success)
												{
													INKPOT_ERROR("%s : TEST_VARIABLE not as expected. \nExpected: %s\nActual__: %s", *InkTestName, *storysStringValue, *stringValue);
													return false;
												}
											}
											else
											{
												INKPOT_ERROR("%s : TEST_VARIABLE Could not parse VARIABLE_VALUE as String", *InkTestName);
											}
										}
										else if (variableType.Equals("null"))
										{
											TSharedPtr<FJsonValue> variableValue = (*testVariableObject)->TryGetField(TEXT("VARIABLE_VALUE"));
											if (!variables.Contains(variableName))
											{
												const bool success = variableValue->IsNull();
												if (!success)
												{
													INKPOT_ERROR("%s : TEST_VARIABLE not as expected, Expected null.", *InkTestName);
													return false;
												}
											}
										}
									}
								}
							}
						}
						else if (testInstruction->HasField(TEXT("EXPECTED_EXCEPTION")))
						{
							FString exceptionText;
							if (testInstruction->TryGetStringField(TEXT("EXPECTED_EXCEPTION"), exceptionText))
							{
								AddExpectedError(exceptionText, EAutomationExpectedErrorFlags::Contains, 1);
							}
						}
						else if (testInstruction->HasField(TEXT("OBSERVE_VARIABLE")))
						{
							FString variableName;
							if (testInstruction->TryGetStringField(TEXT("OBSERVE_VARIABLE"), variableName))
							{
								TSharedPtr<Ink::FObject> variableObj = story->GetVariable(variableName);
								TSharedPtr<Ink::FValue> variable = StaticCastSharedPtr<Ink::FValue>(variableObj);

								ObserverVariable = variable->GetValueObject();
								ObserverCallCount = 0;
								
								observer->BindLambda([&ObserverCallCount, &ObserverVariable](FString VariableName, Ink::FValueType NewValue) {
									ObserverVariable = NewValue;
									ObserverCallCount++;
									});
							
								story->ObserveVariable(variableName, observer);
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_OBSERVED_VARIABLE")))
						{
							if (testInstruction->TryGetNumberField(TEXT("TEST_OBSERVED_VARIABLE"), jsonParsedInt))
							{
								const int32 expectedVariableValue = jsonParsedInt;
								const int32 actualVariableValue = ObserverVariable.GetSubtype<int32>();
								const bool success = expectedVariableValue == actualVariableValue;
								if (!success)
								{
									INKPOT_ERROR("%s : TEST_OBSERVED_VARIABLE not as expected. \nExpected: %d\nActual__: %d", *InkTestName, expectedVariableValue, actualVariableValue);
									return false;
								}
							}
						}
						else if (testInstruction->HasField(TEXT("TEST_OBSERVER_CALL_COUNT")))
						{
							if (testInstruction->TryGetNumberField(TEXT("TEST_OBSERVER_CALL_COUNT"), jsonParsedInt))
							{
								const int32 expectedObserverCallCount = jsonParsedInt;
								const bool success = expectedObserverCallCount == ObserverCallCount;
								if (!success)
								{
									INKPOT_ERROR("%s : TEST_OBSERVER_CALL_COUNT not as expected. \nExpected: %d\nActual__: %d", *InkTestName, expectedObserverCallCount, ObserverCallCount);
									return false;
								}
							}
						}
						else
						{
							INKPOT_ERROR("UNKNOWN TEST COMMAND '%s' IN TEST %s", *instruction, *InkTestName);
							return false;
						}
					}
					else
					{
						INKPOT_ERROR("Invalid test script, expected only one field per instruction: %s", *InkTestName);
						return false;
					}
				}
			}

			testScriptStartPhraseIndex = fileContents.Find(testScriptStartPhrase, ESearchCase::CaseSensitive, ESearchDir::FromStart, testPhraseEndIndex);
			testStoryScriptStartPhraseIndex = fileContents.Find(testStoryScriptStartPhrase, ESearchCase::CaseSensitive, ESearchDir::FromStart, testPhraseEndIndex);
			
			bCompileAsStory = (uint32)testStoryScriptStartPhraseIndex < (uint32)testScriptStartPhraseIndex;
			startPhrase = bCompileAsStory ? testStoryScriptStartPhrase : testScriptStartPhrase;
			testPhraseStartIndex = bCompileAsStory ? testStoryScriptStartPhraseIndex : testScriptStartPhraseIndex;
			testPhraseEndIndex = fileContents.Find(testEndPhrase, ESearchCase::CaseSensitive, ESearchDir::FromStart, testPhraseEndIndex);
		}
	}
	
	return true;
}


#endif