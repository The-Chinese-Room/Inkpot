#include "Compiler/InkCompiler.h"

#include <HAL/FileManagerGeneric.h>
#include "Misc/FileHelper.h"
#include "Utility/InkpotLog.h"
#include "Serialization/JsonSerializer.h"
#include "Interfaces/IPluginManager.h"


namespace InkCompiler
{
	FString GetPluginPath()
	{
		FString path = FPaths::ProjectPluginsDir() + TEXT( "Inkpot/" );
		IPluginManager &pluginManager =  IPluginManager::Get();
		TSharedPtr<IPlugin> inkpotPlugin = pluginManager.FindPlugin("Inkpot");
		if(inkpotPlugin)
			path = inkpotPlugin->GetBaseDir();
		return path;
	}

	FString GetInkleCatePath()
	{
		return GetPluginPath() + TEXT( "/ThirdParty/InkCommandLine/inklecate.exe" );
	}

	FString GetScratchDirectory()
	{
		return GetPluginPath() + TEXT( "/Intermediate/InkCommandLine/" );
	}

	bool CompileInkFile_Internal(const FString& InkFilePath, const FString& InScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		// Check if inklecate exists
		const FString inkExePath = GetInkleCatePath();
		FString InkFilePathStripped = InkFilePath;
		FPaths::CollapseRelativeDirectories(InkFilePathStripped);
		if (!FPaths::FileExists(inkExePath))
		{
			INKPOT_ERROR("CompileInkFile_Internal - %s could not locate inklecate.exe.", *InkFilePathStripped);
			return false;
		}

		FString compiledJsonPath = InScratchFilePath + TEXT(".json");;
		FPaths::CollapseRelativeDirectories(compiledJsonPath);

		FString args = FString::Printf(TEXT("%s -j -o \"%s\" \"%s\""), shouldCountVisits?TEXT(" -c"):TEXT(" "), *compiledJsonPath, *InkFilePathStripped );

		// Execute compile using inklecate and output compiled JSON to Intermediate/InkCommandline.
		FString stdOut;
		FString stdErr;
		int32 returnCode;
		FString wkDir = GetScratchDirectory();

		bool bSuccess = FPlatformProcess::ExecProcess(*inkExePath, *args, &returnCode, &stdOut, &stdErr, *wkDir, true );
		if (!bSuccess)
		{
			INKPOT_ERROR("CompileInkFile_Internal - %s failed ExecProcess. Error %s: ", *InkFilePathStripped, *(stdOut + stdErr));
			return false;
		}

		bool bExported = false;
		bool bCompiled = false;
		ParseInklecateOutput(stdOut, Errors, Warnings, bCompiled, bExported);
		if (!bCompiled)
		{
			// supress warnings for tests that we expect to fail compilation
			if(!bIsCompilationFailExpected)
			{
				INKPOT_ERROR( "CompileInkFile_Internal - %s failed to compile ink. Error %s: ", *InkFilePathStripped, *(stdOut + stdErr) );
			}
			return false;
		}
		if (!bExported)
		{
			INKPOT_ERROR("CompileInkFile_Internal - %s failed to export ink.json . Error : %s", *InkFilePathStripped, *(stdOut + stdErr));
			return false;
		}

		INKPOT_LOG("CompileInkFile_Internal - Output: %s", *(stdOut + stdErr));

		return FFileHelper::LoadFileToString(OutCompiledJSON, *compiledJsonPath);
	}


	bool GetContainingFolder(FString InputPath, FString& OutContainingFolder)
	{
		// strip the end of the path until we reach the next folder
		while (!InputPath.IsEmpty())
		{
			InputPath.RemoveAt(InputPath.Len() - 1, 1);
			if (InputPath.EndsWith("/"))
			{
				OutContainingFolder = InputPath;
				return true;
			}
		}
		OutContainingFolder = "";
		return false;
	}

	void FlushScratchDirectory()
	{
		IPlatformFile::GetPlatformPhysical().DeleteDirectoryRecursively( *GetScratchDirectory() );
		IPlatformFile::GetPlatformPhysical().CreateDirectory( *GetScratchDirectory() );
	}

	bool CompileInkFile(const FString& InSourceFilePath, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		return CompileInkFile_Internal(InSourceFilePath, ScratchFilePath, OutCompiledJSON, Errors, Warnings, shouldCountVisits, bIsCompilationFailExpected);
	}

	bool CompileInkString(const FString& SourceString, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		// Save the ink source string to an ink file in the scratch directory for compiling 
		FlushScratchDirectory();
		FFileHelper::SaveStringToFile(SourceString, *ScratchFilePath);
		return CompileInkFile_Internal(ScratchFilePath, ScratchFilePath, OutCompiledJSON, Errors, Warnings, shouldCountVisits, bIsCompilationFailExpected );
	}

	void ParseInklecateOutput(const FString& InklecateOutput, TArray<FString>& Errors, TArray<FString>& Warnings, bool& bCompileSuccess, bool& bExportSuccess)
	{
		FString toParse = TEXT("[") + InklecateOutput + TEXT("]");
		toParse = toParse.Replace(_T("}"), _T("},"));

		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(toParse);

		TArray< TSharedPtr<FJsonValue> > jsonArray;
		if (FJsonSerializer::Deserialize(Reader, jsonArray))
		{
			INKPOT_LOG("CompileInkString - Output: %d", jsonArray.Num());
		}

		for (TSharedPtr<FJsonValue> element : jsonArray)
		{
			const TSharedPtr<FJsonObject>& elementObject = element->AsObject();
			if (elementObject->HasField(TEXT("compile-success")))
			{
				bCompileSuccess = elementObject->GetBoolField(TEXT("compile-success"));
			}
			else if (elementObject->HasField(TEXT("issues")))
			{
				TArray<FString> issueStrings;
				elementObject->TryGetStringArrayField(TEXT("issues"), issueStrings);

				for (FString issue : issueStrings)
				{
					if (issue.Contains(TEXT("ERROR")))
					{
						Errors.Add(issue);
					}
					else if (issue.Contains(TEXT("WARNING")))
					{
						Warnings.Add(issue);
					}
				}
			}
			else if (elementObject->HasField(TEXT("export-complete")))
			{
				bExportSuccess = elementObject->GetBoolField(TEXT("export-complete"));
			}
		}
	}

}