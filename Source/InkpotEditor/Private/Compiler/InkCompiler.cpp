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
		path = FPaths::ConvertRelativePathToFull( path );
		return path;
	}

	FString GetInkleCatePath()
	{
	#if PLATFORM_WINDOWS
		return GetPluginPath() + TEXT( "/ThirdParty/InkCommandLine/inklecate.exe" );
	#elif PLATFORM_MAC
		return GetPluginPath() + TEXT( "/ThirdParty/InkCommandLine/inklecate" );
	#endif
	}

	FString GetScratchDirectory()
	{
		return GetPluginPath() + TEXT( "/Intermediate/InkCommandLine/" );
	}

	FString GetScratchFileName()
	{
		return GetScratchDirectory() + TEXT( "InkFileTemp.ink" );
	}

	bool CompileInkFile_Internal(const FString& InkFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		// Check if inklecate exists
		const FString inkExePath = GetInkleCatePath();
		if (!FPaths::FileExists(inkExePath))
		{
			INKPOT_ERROR("CompileInkFile_Internal - could not locate inklecate.exe." );
			return false;
		}

		// resolve file paths
		FString sourceInkPath = InkFilePath;
		FPaths::CollapseRelativeDirectories(sourceInkPath );
		sourceInkPath  = FPaths::ConvertRelativePathToFull( sourceInkPath  );

		FString compiledJsonPath = GetScratchDirectory() + FPaths::GetCleanFilename(sourceInkPath ) + TEXT(".json");
		FPaths::CollapseRelativeDirectories(compiledJsonPath);

		FString args = FString::Printf(TEXT("%s -j -o \"%s\" \"%s\""), shouldCountVisits?TEXT(" -c"):TEXT(" "), *compiledJsonPath, *sourceInkPath );

		// Execute compile using inklecate and output compiled JSON to Intermediate/InkCommandline.
		FString stdOut;
		FString stdErr;
		int32 returnCode;
		FString baseDir = FString( FPlatformProcess::BaseDir() );

		INKPOT_LOG("ExecProcess :\n\tcmd : %s\n\targ : %s\n\tbsd : %s", *inkExePath, *args, *baseDir );

		bool bSuccess = FPlatformProcess::ExecProcess(*inkExePath, *args, &returnCode, &stdOut, &stdErr );
		if (!bSuccess)
		{
			INKPOT_ERROR("CompileInkFile_Internal - %s failed ExecProcess. Error %s: ", *sourceInkPath, *(stdOut + stdErr));
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
				INKPOT_ERROR( "CompileInkFile_Internal - %s failed to compile ink. Error %s: ", *sourceInkPath, *(stdOut + stdErr) );
			}
			return false;
		}
		if (!bExported)
		{
			INKPOT_ERROR("CompileInkFile_Internal - %s failed to export ink.json . Error : %s", *sourceInkPath, *(stdOut + stdErr));
			return false;
		}

		INKPOT_LOG("CompileInkFile_Internal - Output: %s", *(stdOut + stdErr));

		return FFileHelper::LoadFileToString(OutCompiledJSON, *compiledJsonPath);
	}


	void FlushScratchDirectory()
	{
		IPlatformFile::GetPlatformPhysical().DeleteDirectoryRecursively( *GetScratchDirectory() );
		IPlatformFile::GetPlatformPhysical().CreateDirectory( *GetScratchDirectory() );
	}

	bool CompileInkFile(const FString& InSourceFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		FlushScratchDirectory();
		return CompileInkFile_Internal(InSourceFilePath, OutCompiledJSON, Errors, Warnings, shouldCountVisits, bIsCompilationFailExpected);
	}

	bool CompileInkString(const FString& SourceString, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits, bool bIsCompilationFailExpected )
	{
		// Save the ink source string to an ink file in the scratch directory for compiling 
		FlushScratchDirectory();
		FString tmpFileName = GetScratchFileName();
		FFileHelper::SaveStringToFile(SourceString, *tmpFileName );
		return CompileInkFile_Internal(tmpFileName, OutCompiledJSON, Errors, Warnings, shouldCountVisits, bIsCompilationFailExpected );
	}

	void ParseInklecateOutput(const FString& InklecateOutput, TArray<FString>& Errors, TArray<FString>& Warnings, bool& bCompileSuccess, bool& bExportSuccess)
	{
		FString toParse = TEXT("[") + InklecateOutput + TEXT("]");
		toParse = toParse.Replace(TEXT("}"), TEXT("},"));

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