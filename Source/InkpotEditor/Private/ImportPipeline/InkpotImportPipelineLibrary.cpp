#include "ImportPipeline/InkpotImportPipelineLibrary.h"
#include "Asset/InkpotStoryAsset.h"
#include "Settings/InkpotPreferences.h"
#include "GameplayTags/InkpotTagUtility.h"
#include "Compiler/InkCompiler.h"
#include "EditorFramework/AssetImportData.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Inkpot/Inkpot.h"

void DumpWarningsAndErrors(const TArray<FString>& errors, const TArray<FString>& warnings)
{
	FMessageLog InkPipelineLog("InkpotPipeline");

	InkPipelineLog.Open(EMessageSeverity::Error);

	for (const FString& warning : warnings)
		InkPipelineLog.Warning(FText::FromString(warning));

	for (const FString& error : errors)
		InkPipelineLog.Error(FText::FromString(error));
	InkPipelineLog.Flush();
}

void UInkpotImportPipelineLibrary::CompileInkFile(const FString& SourceFilePath, bool& bSuccess, FString& OutCompiledJSON)
{
	TArray<FString> errors;
	TArray<FString> warnings;
	bSuccess = InkCompiler::CompileInkFile(SourceFilePath, OutCompiledJSON, errors, warnings);
	if (errors.Num() != 0)
		bSuccess = false;
	DumpWarningsAndErrors(errors, warnings);;
}

void UInkpotImportPipelineLibrary::CompileInkString(const FString& SourceString, bool& bSuccess, FString& OutCompiledJSON)
{
	TArray<FString> errors;
	TArray<FString> warnings;
	bSuccess = InkCompiler::CompileInkString(SourceString, OutCompiledJSON, errors, warnings);
	if (errors.Num() != 0)
		bSuccess = false;
	DumpWarningsAndErrors(errors, warnings);;
}

void UInkpotImportPipelineLibrary::LoadFileToString(FString SourceFilePath, FString& LoadedString, bool& bSuccess)
{
	bSuccess = FFileHelper::LoadFileToString(LoadedString, *SourceFilePath);
}

void UInkpotImportPipelineLibrary::GenerateTAGs(UInkpotStoryAsset* InStoryAsset, bool& bSuccess)
{
	bSuccess = false;

	const UInkpotPreferences* settings = GetDefault<UInkpotPreferences>();
	if (!settings->bAutogenerateGameplayTags)
		return;

	UPackage* package = InStoryAsset->GetPackage();
	if (!package)
		return;

	FString name = FPaths::GetCleanFilename(package->GetName());
	FString path = FPaths::GetPath(package->GetPathName());

	UInkpotTagUtility::CreateTagTable(name, path, InStoryAsset);

	bSuccess = true;
}

void UInkpotImportPipelineLibrary::DumpStrings(const FString& DestFilePath, UInkpotStoryAsset* InStoryAsset)
{
	UInkpot* inkpot = GEngine->GetEngineSubsystem<UInkpot>();
	UInkpotStory* story = inkpot->BeginStory(InStoryAsset);
	TMap<FString, FString> strings;
	story->GatherAllStrings(strings);

	FString filecontent = FString::Printf(TEXT("\"Name\",\"InkText\",\"LocText\"\n"));

	for (auto entry : strings)
	{
		const FString& key = entry.Key;
		const FString& val = entry.Value;
		FString line = FString::Printf(TEXT("%s,\"%s\",\"%s\"\n"), *key, *val, *val);
		filecontent += line;
	}

	FString newPathAndName;
	if (DestFilePath.Len())
	{
		newPathAndName = DestFilePath;
	}
	else
	{
		FString fullPathAndName = InStoryAsset->GetAssetImportData()->GetFirstFilename();
		FString path = FPaths::GetPath(fullPathAndName);
		FString inkname = FPaths::GetBaseFilename(fullPathAndName);
		inkname += TEXT("_");
		newPathAndName = FPaths::CreateTempFilename(*path, *inkname, TEXT(".csv"));
	}

	FFileHelper::SaveStringToFile(filecontent, *newPathAndName);
}

void UInkpotImportPipelineLibrary::ExecProcess(FString URL, FString WorkingDirectory, FString Params, int32 &ReturnCode, FString& StdOut, FString& StdErr, bool& bSuccess)
{
	bSuccess = FGenericPlatformProcess::ExecProcess(*URL,*Params,&ReturnCode,&StdOut,&StdErr,*WorkingDirectory,false);
}

FString UInkpotImportPipelineLibrary::GetPluginPath()
{
	FString path = FPaths::ProjectPluginsDir() + TEXT("Inkpot/");
	IPluginManager& pluginManager = IPluginManager::Get();
	TSharedPtr<IPlugin> inkpotPlugin = pluginManager.FindPlugin("Inkpot");
	if (inkpotPlugin)
		path = inkpotPlugin->GetBaseDir();
	path = FPaths::ConvertRelativePathToFull(path);
	return path;
}

FString UInkpotImportPipelineLibrary::GetScratchDirectory()
{
	return GetPluginPath() + TEXT("/Intermediate/InkpotImport/");
}

void UInkpotImportPipelineLibrary::FlushScratchDirectory()
{
	IPlatformFile::GetPlatformPhysical().DeleteDirectoryRecursively(*GetScratchDirectory());
	IPlatformFile::GetPlatformPhysical().CreateDirectory(*GetScratchDirectory());
}


