#include "Compiler/InkCompiler.h"

#include <HAL/FileManagerGeneric.h>
#include "Misc/FileHelper.h"
#include "Utility/InkpotLog.h"
#include "Serialization/JsonSerializer.h"

const FString InkIncludeFolderName = "SharedInk";
const FString InkpotPath = FPaths::ProjectPluginsDir() + TEXT("Inkpot/");
const FString InkleCatePath = InkpotPath + TEXT("ThirdParty/InkCommandLine/inklecate.exe");
const FString InkScratchDirectory = InkpotPath + TEXT("Intermediate/InkCommandLine/");

namespace InkCompiler
{
	bool CompileInkFile_Internal(const FString& InkFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits)
	{
		// Check if inklecate exists
		const FString inkExePath = InkleCatePath;
		FString InkFilePathStripped = InkFilePath;
		FPaths::CollapseRelativeDirectories(InkFilePathStripped);
		if (!FPaths::FileExists(inkExePath))
		{
			INKPOT_WARN("CompileInkString - %s could not locate inklecate.exe.", *InkFilePathStripped);
			return false;
		}

		FString args = FString::Printf(TEXT("%s -j \"%s\""), shouldCountVisits?TEXT(" -c"):TEXT(""), *InkFilePathStripped );
		// Execute compile using inklecate and output compiled JSON to Intermediate/InkCommandline.
		FString stdOut;
		FString stdErr;
		int32 returnCode;
		bool bSuccess = FPlatformProcess::ExecProcess(*inkExePath, *args, &returnCode, &stdOut, &stdErr);
		if (!bSuccess)
		{
			INKPOT_WARN("CompileInkString - %s failed ExecProcess. Error %s: ", *InkFilePathStripped, *(stdOut + stdErr));
			return false;
		}

		bool bExported = false;
		bool bCompiled = false;
		ParseInklecateOutput(stdOut, Errors, Warnings, bCompiled, bExported);
		if (!bCompiled)
		{
			INKPOT_WARN("CompileInkString - %s failed to compile ink string. Error %s: ", *InkFilePathStripped, *(stdOut + stdErr));
			return false;
		}
		if (!bExported)
		{
			INKPOT_WARN("CompileInkFile - %s failed to export ink.json string. Error : %s", *InkFilePathStripped, *(stdOut + stdErr));
			return false;
		}

		INKPOT_LOG("CompileInkString - Output: %s", *(stdOut + stdErr));

		// Read compiled json from the Intermediate/InkCommandline folder.
		FString compiledJsonPath = InkFilePath + TEXT(".json");
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

	void CopyIncludeFoldersToScratchDirectory(const FString& InSourceFilePath)
	{
		//
		// in order for the ink story to compile in our scratch directory, we need to mimic the original directory structure
		// the plan is that the structure will look like this:
		//
		// InkScratchDirectory
		//		- SharedInk/
		//			* MasterIncludeFile.ink
		//			* OtherInclude.ink
		//			* OtherInclude.ink
		//		- Subfolder 1/
		//			- Subfolder2/
		//				* InkFileToCompileA.ink
		//				* InkFileToCompileB.ink
		//			- Subfolder2/
		//				* InkFileToCompileA.ink
		//				* InkFileToCompileB.ink
		//
		// So we must copy the ink file into a representative subfolder, and also find & copy the
		// ink include folder that lives two levels up

		FString sourceFilePath = InSourceFilePath;
		FPaths::NormalizeFilename(sourceFilePath);

		FString containingFolder;
		if (GetContainingFolder(sourceFilePath, containingFolder) // folder the file is in
			&& GetContainingFolder(containingFolder, containingFolder) // folder above
			&& GetContainingFolder(containingFolder, containingFolder)) // folder two above
		{
			const FString folderToCopy = containingFolder + InkIncludeFolderName + "/";
			if (FPaths::DirectoryExists(folderToCopy))
			{
				const FString includeDestinationFolder = InkScratchDirectory + InkIncludeFolderName + "/";
				CopyFilesMatchingFilter(folderToCopy, includeDestinationFolder, "*.ink");
			}
		}
	}

	FString GetScratchDirectory()
	{
		return InkScratchDirectory;
	}

	void FlushScratchDirectory()
	{
		IPlatformFile::GetPlatformPhysical().DeleteDirectoryRecursively(*InkScratchDirectory);
	}

	void CopyFilesMatchingFilter(const FString& SourceFolder, const FString& DestinationFolder, const FString& FileFilter)
	{
		const FString fileFilter = SourceFolder + FileFilter;

		TArray<FString> IncludeFiles;
		FFileManagerGeneric::Get().FindFiles(IncludeFiles, *fileFilter, true, false);
		for (const FString& file : IncludeFiles)
		{
			const FString sourcePath = SourceFolder + file;
			const FString destinationPath = DestinationFolder + file;
			IFileManager::Get().Copy(*destinationPath, *sourcePath, true, false, false, nullptr);
		}
	}

	bool CompileInkFile(const FString& InSourceFilePath, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits)
	{
		// Copy the ink source file to the scratch directory for compiling 
		IFileManager::Get().Copy(*ScratchFilePath, *InSourceFilePath, true, false, false, nullptr);
		return CompileInkFile_Internal(ScratchFilePath, OutCompiledJSON, Errors, Warnings, shouldCountVisits);
	}

	bool CompileInkString(const FString& SourceString, const FString& ScratchFilePath, FString& OutCompiledJSON, TArray<FString>& Errors, TArray<FString>& Warnings, bool shouldCountVisits)
	{
		// Save the ink source string to an ink file in the scratch directory for compiling 
		FFileHelper::SaveStringToFile(SourceString, *ScratchFilePath);
		return CompileInkFile_Internal(ScratchFilePath, OutCompiledJSON, Errors, Warnings, shouldCountVisits);
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