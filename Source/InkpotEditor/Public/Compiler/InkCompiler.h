#pragma once

namespace InkCompiler
{
	INKPOTEDITOR_API FString GetScratchDirectory();
	INKPOTEDITOR_API void FlushScratchDirectory();
	INKPOTEDITOR_API void CopyFilesMatchingFilter(const FString& SourceFolder, const FString& DestinationFolder, const FString& FileFilter);
	INKPOTEDITOR_API bool CompileInkFile(const FString& SourceFilePath, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits = false);
	INKPOTEDITOR_API bool CompileInkString(const FString& SourceString, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits = false);
	INKPOTEDITOR_API void ParseInklecateOutput(const FString& InklecateOutput, TArray<FString>& Errors, TArray<FString>& Warnings, bool& bCompileSuccess, bool& bExportSuccess);
}