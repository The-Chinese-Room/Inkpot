#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"
#include "EditorReimportHandler.h"
#include "Asset/InkpotStoryAsset.h"
//#include "Ink/Value.h"
#include "InkpotStoryAssetFactory.generated.h"

UCLASS(hidecategories = Object)
class INKPOTEDITOR_API UInkpotStoryAssetFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

	UInkpotStoryAssetFactory();
	//~ UFactory interface
	virtual bool FactoryCanImport(const FString& InFilename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled) override;
	//~ UFactory interface

	//~ Begin FReimportHandler Interface
	virtual bool CanReimport(UObject* InObject, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* InObject, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* InObject) override;
	virtual int32 GetPriority() const override;
	//~ End FReimportHandler Interface

private:
	bool LoadAndCompileStory(const FString& InFilename, FString& OutStory, FString& OutCompiledStory) const;
	void DumpStrings(UInkpotStoryAsset *InStoryAsset) const;
	void GenerateTAGs( UObject* InParent, UInkpotStoryAsset *InStoryAsset ) const;
};