#pragma once

namespace InkCompiler
{
	INKPOTEDITOR_API bool CompileInkFile(const FString& SourceFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits = false, bool bIsCompilationFailExpected = false);
	INKPOTEDITOR_API bool CompileInkString(const FString& SourceString, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits = false, bool bIsCompilationFailExpected = false);
	INKPOTEDITOR_API void ParseInklecateOutput(const FString& InklecateOutput, TArray<FString>& Errors, TArray<FString>& Warnings, bool& bCompileSuccess, bool& bExportSuccess);
}