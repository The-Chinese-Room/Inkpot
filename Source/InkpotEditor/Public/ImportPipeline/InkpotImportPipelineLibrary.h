#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InkpotImportPipelineLibrary.generated.h"

class UInkpotStoryAsset;

UCLASS(meta = (ScriptName = "InkpotPipelineImportLibrary"), MinimalAPI)
class UInkpotImportPipelineLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/* CompileInkFile 
	* Runs Inklecate internally on the provided source file and outputs the compiled reesult as a JSON string.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = "bSuccess"), Category = "Inkpot|Import")
	static INKPOTEDITOR_API void CompileInkFile(const FString& SourceFilePath, bool &bSuccess, FString& CompiledJSON);

	/* CompileInkString
	* Runs Inklecate internally on the provided source file and outputs the compiled reesult as a JSON string.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = "bSuccess"), Category = "Inkpot|Import")
	static INKPOTEDITOR_API void CompileInkString(const FString& SourceString, bool& bSuccess, FString& CompiledJSON);

	/* GenerateTAGs
	* Generates a GameplayTag table asset from the story asset. 
	* GameplayTag table asset is colocated with the story asset. 
	* Adds generated table to the Gamplay Tag Table List in the GameplayTags section of Project Settings.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = "bSuccess"), Category = "Inkpot|Import")
	static INKPOTEDITOR_API void GenerateTAGs( UInkpotStoryAsset* StoryAsset, bool& bSuccess );

	/* ExecProcess
	* Executes an external process.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = "bSuccess"), Category = "Inkpot|Import")
	static INKPOTEDITOR_API void ExecProcess(FString Command, FString WorkingDirectory, FString Params, int32& ReturnCode, FString &StdOut, FString& StdErr, bool &bSuccess);

	/* LoadFileToString
	* Loads the entirety of the source file to a string.
	*/
	UFUNCTION(BlueprintCallable, meta = (ExpandBoolAsExecs = "bSuccess"), Category = "Inkpot|Import")
	static INKPOTEDITOR_API void LoadFileToString(FString SourceFilePath, FString& LoadedString, bool& bSuccess);

	/* Dump Strings
	* Collates all the strings in the story to a csv file.
	* If Output File Path is empty then the csv will be created in the source directory of the initial ink file.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Import")
	static INKPOTEDITOR_API void DumpStrings(const FString& OutputFilePath, UInkpotStoryAsset* StoryAsset);

	/* GetPluginPath
	* Gets the full file path to the inkpot plugin.
	*/
	UFUNCTION(BlueprintPure, Category = "Inkpot|Import")
	static INKPOTEDITOR_API FString GetPluginPath();

	/* GetScratchDirectory
	* Gets the full file path to the Inkpot pipeline scratch directory.
	*/
	UFUNCTION(BlueprintPure, Category = "Inkpot|Import")
	static INKPOTEDITOR_API FString GetScratchDirectory();

	/* GetScratchDirectory
	* Deletes the contents of the Inkpot pipeline scratch directory.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inkpot|Import")
	static INKPOTEDITOR_API void FlushScratchDirectory();
};
