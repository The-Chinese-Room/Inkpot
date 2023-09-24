#pragma once
#include "CoreMinimal.h"
#include "Json.h"
#include "Utility/InkPlusPlusLog.h"

namespace Ink
{
	class FObject;
	class FContainer;
	class FChoice;
	class FValueList;
	class FListDefinitionsOrigin;

	// May need wrappers for "WritePropertyStart / WritePropertyEnd" that are in source parser (SimpleJson), currently just using Unreal's Write Value and hope to be sufficient
	class INKPLUSPLUS_API FJsonSerialisation
	{
	public:
		~FJsonSerialisation() = default;
		
		static TArray<TSharedPtr<Ink::FObject>> JsonArrayToRuntimeObjectList(const TArray<TSharedPtr<FJsonValue>>& InJSONArray, bool InSkipLast = false);
		static TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> JsonArrayToChoiceList(const TArray<TSharedPtr<FJsonValue>>& InJSONArray, bool InSkipLast = false);
		static TMap<FString, TSharedPtr<Ink::FObject>> JsonObjectToDictionaryRuntimeObjects(const FJsonObject& InJSONObject);
		static TMap<FString, int32> JsonObjectToIntDictionary(const FJsonObject& InJSONObject);
		static TSharedPtr<Ink::FChoice> JsonObjectToChoice(const FJsonObject& InJSONObject);

		static TSharedPtr<Ink::FContainer> JsonArrayToContainer(const TArray<TSharedPtr<FJsonValue>>& InJSONArray);
		
		static void WriteDictionaryRuntimeObjects(TJsonWriter<>* InJSONWriter, const TMap<FString, TSharedPtr<Ink::FObject>>& InDictionary);
		static void WriteListRuntimeObjects(TJsonWriter<>* InJSONWriter, const TArray<TSharedPtr<Ink::FObject>>& InList);
		static void WriteIntDictionary(TJsonWriter<>* InJSONWriter, const TMap<FString, int32>& InDictionary);
		static void WriteRuntimeObject(TJsonWriter<>* InJSONWriter, TSharedPtr<Ink::FObject> InObject);
		static void WriteRuntimeContainer(TJsonWriter<>* InJSONWriter, const Ink::FContainer& InContainer, bool InWithoutName = false);

		static void WriteChoice(TJsonWriter<>* InJSONWriter, const Ink::FChoice& InChoice);
		static void WriteInkList(TJsonWriter<>* InJSONWriter, const Ink::FValueList& InListValue);

		/*	----------------------
			JSON ENCODING SCHEME
			----------------------

			Glue:           "<>", "G<", "G>"
	
			ControlCommand: "ev", "out", "/ev", "du" "pop", "->->", "~ret", "str", "/str", "nop", 
							"choiceCnt", "turns", "visit", "seq", "thread", "done", "end"
	
			NativeFunction: "+", "-", "/", "*", "%" "~", "==", ">", "<", ">=", "<=", "!=", "!"... etc
	
			Void:           "void"
	
			Value:          "^string value", "^^string value beginning with ^"
							5, 5.2
							{"^->": "path.target"}
							{"^var": "varname", "ci": 0}
	
			Container:      [...]
							[..., 
								{
									"subContainerName": ..., 
									"#f": 5,                    // flags
									"#n": "containerOwnName"    // only if not redundant
								}
							]
	
			Divert:			{"->": "path.target", "c": true }
							{"->": "path.target", "var": true}
							{"f()": "path.func"}
							{"->t->": "path.tunnel"}
							{"x()": "externalFuncName", "exArgs": 5}
	
			Var Assign:     {"VAR=": "varName", "re": true}   // reassignment
							{"temp=": "varName"}
	
			Var ref:        {"VAR?": "varName"}
							{"CNT?": "stitch name"}
	
			ChoicePoint:    {"*": pathString,
							"flg": 18 }
	
			Choice:         Nothing too clever, it's only used in the save state,
							there's not likely to be many of them.
	
			Tag:            {"#": "the tag text"}														*/
		static TSharedPtr<Ink::FObject> JsonTokenToRuntimeObject(const FJsonValue& InToken);
		static TSharedPtr<Ink::FListDefinitionsOrigin> JsonTokenToListDefinitions(const FJsonValue& InToken);
	
	private:
		FJsonSerialisation() = default;
		FJsonSerialisation(const Ink::FJsonSerialisation& InOtherJsonSerialisation) = delete;
		FJsonSerialisation& operator=(const Ink::FJsonSerialisation& InOtherJsonSerialisation) = delete;

	private:
		static TArray<FString> ControlCommandNames;
	};

}