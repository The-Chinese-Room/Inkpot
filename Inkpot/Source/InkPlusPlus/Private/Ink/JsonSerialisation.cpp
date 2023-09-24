#include "Ink/JsonSerialisation.h"

#include "Ink/Object.h"
#include "Ink/Container.h"
#include "Ink/Choice.h"
#include "Ink/ChoicePoint.h"
#include "Ink/ControlCommand.h"
#include "Ink/Divert.h"
#include "Ink/Glue.h"
#include "Ink/JsonExtension.h"
#include "Ink/Value.h"
#include "Ink/ListDefinitionsOrigin.h"
#include "Ink/NativeFunctionCall.h"
#include "Ink/Void.h"
#include "Ink/Path.h"
#include "Ink/PushPop.h"
#include "Ink/Tag.h"
#include "Ink/VariableAssignment.h"
#include "Ink/VariableReference.h"
#include "Misc/DefaultValueHelper.h"


TArray<FString> Ink::FJsonSerialisation::ControlCommandNames = {		TEXT("ev"),
																		TEXT("out"),
																		TEXT("/ev"),
																		TEXT("du"),
																		TEXT("pop"),
																		TEXT("~ret"),
																		TEXT("->->"),
																		TEXT("str"),
																		TEXT("/str"),
																		TEXT("nop"),
																		TEXT("choiceCnt"),
																		TEXT("turn"),
																		TEXT("turns"),
																		TEXT("readc"),
																		TEXT("rnd"),
																		TEXT("srnd"),
																		TEXT("visit"),
																		TEXT("seq"),
																		TEXT("thread"),
																		TEXT("done"),
																		TEXT("end"),
																		TEXT("listInt"),
																		TEXT("range"),
																		TEXT("lrnd")		};


TArray<TSharedPtr<Ink::FObject>> Ink::FJsonSerialisation::JsonArrayToRuntimeObjectList(const TArray<TSharedPtr<FJsonValue>>& InJSONArray, bool InSkipLast)
{
	int32 count = InJSONArray.Num();
	if (InSkipLast)
		--count;

	TArray<TSharedPtr<Ink::FObject>> list;
	list.Reserve(InJSONArray.Num());

	for (int32 i = 0; i < count; ++i)
	{
		TSharedPtr<FJsonValue> jsonToken = InJSONArray[i];
		TSharedPtr<Ink::FObject> runtimeObject = JsonTokenToRuntimeObject(*jsonToken);
		if (runtimeObject == nullptr)
		{
			UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to get Run Time Object from the passed in JSON Token!"))
			continue;
		}
		
		list.Add(runtimeObject);
	}

	return list;
}

TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> Ink::FJsonSerialisation::JsonArrayToChoiceList(const TArray<TSharedPtr<FJsonValue>>& InJSONArray, bool InSkipLast)
{
	int32 count = InJSONArray.Num();
	if (InSkipLast)
		--count;

	TSharedPtr<TArray<TSharedPtr<Ink::FChoice>>> list = MakeShared<TArray<TSharedPtr<Ink::FChoice>>>();
	list->Reserve(count);

	for (int32 i = 0; i < count; ++i)
	{
		TSharedPtr<FJsonValue> jsonToken = InJSONArray[i];
		TSharedPtr<Ink::FObject> runtimeObject(JsonTokenToRuntimeObject(*jsonToken));
		if (!runtimeObject.IsValid())
		{
			UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to get Run Time Object from the passed in JSON Token!"))
				continue;
		}

		TSharedPtr<Ink::FChoice> convertedRuntimeObject = FObject::DynamicCastTo<Ink::FChoice>(runtimeObject);
		if (!convertedRuntimeObject.IsValid())
		{
			UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to convert Run Time object to the specified template!"))
				continue;
		}
		list->Add(convertedRuntimeObject);
	}

	return list;
}

TMap<FString, TSharedPtr<Ink::FObject>> Ink::FJsonSerialisation::JsonObjectToDictionaryRuntimeObjects(const FJsonObject& InJSONObject)
{
	TMap<FString, TSharedPtr<Ink::FObject>> dictionary;

	const TMap<FString, TSharedPtr<FJsonValue>> jsonDict = InJSONObject.Values;
	dictionary.Reserve(jsonDict.Num());

	for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : jsonDict)
	{
		dictionary.Add(pair.Key, TSharedPtr<FObject>(JsonTokenToRuntimeObject(*pair.Value)));
	}

	return dictionary;
}

TMap<FString, int32> Ink::FJsonSerialisation::JsonObjectToIntDictionary(const FJsonObject& InJSONObject)
{
	TMap<FString, int32> dictionary;

	const TMap<FString, TSharedPtr<FJsonValue>> jsonDict = InJSONObject.Values;
	dictionary.Reserve(jsonDict.Num());

	for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : jsonDict)
	{
		int32 value;
		if (pair.Value->TryGetNumber(value))
			dictionary.Add(pair.Key, value);
		else
			UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not convert %s to int"), *pair.Value->AsString());
	}

	return dictionary;
}

TSharedPtr<Ink::FChoice> Ink::FJsonSerialisation::JsonObjectToChoice(const FJsonObject& InJSONObject)
{
	TSharedPtr<Ink::FChoice> choice = MakeShared<Ink::FChoice>();
	
	FString text;
	if (!InJSONObject.TryGetStringField(TEXT("text"), text))
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not get choice text"));
	choice->SetText(text);

	int32 index;
	if (!InJSONObject.TryGetNumberField(TEXT("index"), index))
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not get choice index"));
	choice->SetIndex(index);

	FString sourcePath;
	if (!InJSONObject.TryGetStringField(TEXT("originalChoicePath"), sourcePath))
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not get choice original source path"));
	choice->SetSourcePath(sourcePath);
	
	int32 originalThreadIndex;
	if (!InJSONObject.TryGetNumberField(TEXT("originalThreadIndex"), originalThreadIndex))
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not get choice original thread index"));
	choice->SetOriginalThreadIndex(originalThreadIndex);

	FString pathStringOnChoice;
	if (!InJSONObject.TryGetStringField(TEXT("targetPath"), pathStringOnChoice))
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not get choice target path"));
	choice->SetPathStringOnChoice(pathStringOnChoice);

	return choice;
}

TSharedPtr<Ink::FContainer> Ink::FJsonSerialisation::JsonArrayToContainer(const TArray<TSharedPtr<FJsonValue>>& InJSONArray)
{
	TArray<TSharedPtr<Ink::FObject>> objList = JsonArrayToRuntimeObjectList(InJSONArray, true);

	TSharedPtr<Ink::FContainer> container = MakeShared<Ink::FContainer>();
	container->SetContent(objList);
	
	// Final object in the array is always a combination of
	//  - named content
	//  - a "#f" key with the countFlags
	// (if either exists at all, otherwise null)
	const TSharedPtr<FJsonObject>* terminatingObject;
	if (InJSONArray.Num() > 0 && InJSONArray.Last()->TryGetObject(terminatingObject))
	{
		TMap<FString, TSharedPtr<Ink::FObject>> namedOnlyContent;
		namedOnlyContent.Reserve(terminatingObject->Get()->Values.Num());
		for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : terminatingObject->Get()->Values)
		{
			if (pair.Key == TEXT("#f"))
			{
				uint8 countFlags;
				if (!pair.Value->TryGetNumber(countFlags))
				{
					UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not extract container count flag from current json value!"));
					continue;
				}
				container->SetCountFlags(static_cast<Ink::ECountFlags>(countFlags));
			}

			else if (pair.Key == TEXT("#n"))
			{
				FString name;
				if (!pair.Value->TryGetString(name))
				{
					UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : could not extract container name from current json value!"));
					continue;
				}
				container->SetName(name);
			}

			else
			{
				TSharedPtr<Ink::FObject> namedContentItem(JsonTokenToRuntimeObject(*pair.Value));
				TSharedPtr<Ink::FContainer> namedSubContainer = FObject::DynamicCastTo<Ink::FContainer>(namedContentItem);
				if (namedSubContainer)
					namedSubContainer->SetName(pair.Key);

				namedOnlyContent.Add(pair.Key, namedContentItem);
			}
		}
		container->SetNamedOnlyContent(namedOnlyContent);
	}

	return container;
}

void Ink::FJsonSerialisation::WriteDictionaryRuntimeObjects(TJsonWriter<>* InJSONWriter, const TMap<FString, TSharedPtr<Ink::FObject>>& InDictionary)
{
	InJSONWriter->WriteObjectStart();
	for (const TPair<FString, TSharedPtr<Ink::FObject>>& pair : InDictionary)
	{
		InJSONWriter->WriteIdentifierPrefix(pair.Key);
		WriteRuntimeObject(InJSONWriter, pair.Value);
	}
	InJSONWriter->WriteObjectEnd();
}

void Ink::FJsonSerialisation::WriteListRuntimeObjects(TJsonWriter<>* InJSONWriter, const TArray<TSharedPtr<Ink::FObject>>& InList)
{
	InJSONWriter->WriteArrayStart();
	for (TSharedPtr<Ink::FObject> object : InList)
	{
		WriteRuntimeObject(InJSONWriter, object);
	}
	InJSONWriter->WriteArrayEnd();
}

void Ink::FJsonSerialisation::WriteIntDictionary(TJsonWriter<>* InJSONWriter,const TMap<FString, int32>& InDictionary)
{
	InJSONWriter->WriteObjectStart();
	for (const TPair<FString, int32>& pair : InDictionary)
	{
		InJSONWriter->WriteValue(pair.Key, pair.Value);
	}
	InJSONWriter->WriteObjectEnd();
}

void Ink::FJsonSerialisation::WriteRuntimeObject(TJsonWriter<>* InJSONWriter, TSharedPtr<Ink::FObject> InObject)
{
	// Container
	TSharedPtr<Ink::FContainer> container = FObject::DynamicCastTo<Ink::FContainer>(InObject);
	if (container.IsValid())
	{
		WriteRuntimeContainer(InJSONWriter, *container);
		return;
	}

	// Divert
	TSharedPtr<Ink::FDivert> divert = FObject::DynamicCastTo<Ink::FDivert>(InObject);
	if (divert.IsValid())
	{
		FString divertTypeKey = TEXT("->");
		if (divert->IsExternal())
		{
			divertTypeKey = TEXT("x()");
		}
		
		else if (divert->GetPushesToStack())
		{
			if (divert->GetStackPushType() == Ink::EPushPopType::Function)
				divertTypeKey = TEXT("f()");
			else if (divert->GetStackPushType() == Ink::EPushPopType::Tunnel)
				divertTypeKey = TEXT("->t->");
		}

		FString targetString;
		if (divert->HasVariableTarget())
			targetString = divert->GetVariableDivertName();
		else
			targetString = divert->GetTargetPathString();

		InJSONWriter->WriteObjectStart();
		InJSONWriter->WriteValue(divertTypeKey, targetString);

		if (divert->HasVariableTarget())
			InJSONWriter->WriteValue(TEXT("var"), true);

		if (divert->IsConditional())
			InJSONWriter->WriteValue(TEXT("c"), true);

		if (divert->GetExternalsArgs() > 0)
			InJSONWriter->WriteValue(TEXT("exArgs"), divert->GetExternalsArgs());

		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Choice Point
	TSharedPtr<Ink::FChoicePoint> choicePoint = FObject::DynamicCastTo<Ink::FChoicePoint>(InObject);
	if (choicePoint.IsValid())
	{
		InJSONWriter->WriteObjectStart();
		InJSONWriter->WriteValue(TEXT("*"), choicePoint->GetPathStringOnChoice());
		InJSONWriter->WriteValue(TEXT("flg"), choicePoint->GetFlags());
		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Bool Value
	TSharedPtr<Ink::FValueBool> boolValue = FObject::DynamicCastTo<Ink::FValueBool>(InObject);
	if (boolValue.IsValid())
	{
		InJSONWriter->WriteValue(boolValue->GetValue());
		return;
	}

	// Int Value
	TSharedPtr<Ink::FValueInt> intValue = FObject::DynamicCastTo<Ink::FValueInt>(InObject);
	if (intValue.IsValid())
	{
		InJSONWriter->WriteValue(intValue->GetValue());
		return;
	}

	// Float Value
	TSharedPtr<Ink::FValueFloat> floatValue = FObject::DynamicCastTo<Ink::FValueFloat>(InObject);
	if (floatValue.IsValid())
	{
		InJSONWriter->WriteValue(floatValue->GetValue());
		return;
	}

	// String Value
	TSharedPtr<Ink::FValueString> stringValue = FObject::DynamicCastTo<Ink::FValueString>(InObject);
	if (stringValue.IsValid())
	{
		if (stringValue->IsNewLine())
		{
			InJSONWriter->WriteValue(TEXT("\\n"));
		}
		else
		{
			InJSONWriter->WriteValue(TEXT("^") + stringValue->GetValue());
		}
		return;
	}

	// List Value
	TSharedPtr< Ink::FValueList> listValue = FObject::DynamicCastTo<Ink::FValueList>(InObject);
	if (listValue.IsValid())
	{
		WriteInkList(InJSONWriter, *listValue);
		return;
	}

	// Divert Target Value
	TSharedPtr< Ink::FValueDivertTarget> divertTargetValue = FObject::DynamicCastTo<Ink::FValueDivertTarget>(InObject);
	if (divertTargetValue.IsValid())
	{
		InJSONWriter->WriteObjectStart();
		InJSONWriter->WriteValue(TEXT("^->"), divertTargetValue->GetValue()->GetComponentsString());
		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Variable Pointer Value
	TSharedPtr<Ink::FValueVariablePointer> variablePointerValue = FObject::DynamicCastTo<Ink::FValueVariablePointer>(InObject);
	if (variablePointerValue.IsValid())
	{
		InJSONWriter->WriteObjectStart();
		InJSONWriter->WriteValue(TEXT("^var"), variablePointerValue->GetVariableName());
		InJSONWriter->WriteValue(TEXT("ci"), variablePointerValue->GetContextIndex());
		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Glue
	TSharedPtr<Ink::FGlue> glue = FObject::DynamicCastTo<Ink::FGlue>(InObject);
	if (glue.IsValid())
	{
		InJSONWriter->WriteValue(TEXT("<>"));
		return;
	}

	// Control Command
	TSharedPtr<Ink::FControlCommand> controlCommand = FObject::DynamicCastTo<Ink::FControlCommand>(InObject);
	if (controlCommand.IsValid())
	{
		const FString commandName = ControlCommandNames[static_cast<int32>(controlCommand->GetCommandType())];
		InJSONWriter->WriteValue(commandName);
		return;
	}

	TSharedPtr<Ink::FNativeFunctionCall> nativeFunc = FObject::DynamicCastTo<Ink::FNativeFunctionCall>(InObject);
	if (nativeFunc.IsValid())
	{
		FString name = nativeFunc->GetName();

		// Avoid collision with ^ used to indicate a string
		if (name == "^")
			name = "L^";

		InJSONWriter->WriteValue(name);
		return;
	}

	// Variable Reference
	TSharedPtr<Ink::FVariableReference> variableReference = FObject::DynamicCastTo<Ink::FVariableReference>(InObject);
	if (variableReference.IsValid())
	{
		InJSONWriter->WriteObjectStart();

		const FString readCountPath = variableReference->GetPathStringForCount();
		if (!readCountPath.IsEmpty())
			InJSONWriter->WriteValue(TEXT("CNT?"), readCountPath);
		else
			InJSONWriter->WriteValue(TEXT("VAR?"), variableReference->GetName());

		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Variable Assignment
	TSharedPtr<Ink::FVariableAssignment> variableAssignment = FObject::DynamicCastTo<Ink::FVariableAssignment>(InObject);
	if (variableAssignment.IsValid())
	{
		InJSONWriter->WriteObjectStart();

		const FString key = variableAssignment->IsGlobal() ? TEXT("VAR=") : TEXT("temp=");
		InJSONWriter->WriteValue(key, variableAssignment->GetVariableName());

		// Reassignment?
		if (!variableAssignment->IsNewDeclaration())
			InJSONWriter->WriteValue(TEXT("re"), true);

		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Void
	TSharedPtr<Ink::FVoid> voidObject = FObject::DynamicCastTo<Ink::FVoid>(InObject);
	if (voidObject.IsValid())
	{
		InJSONWriter->WriteValue(TEXT("void"));
		return;
	}

	// Tag
	TSharedPtr<Ink::FTag> tag = FObject::DynamicCastTo<Ink::FTag>(InObject);
	if (tag.IsValid())
	{
		InJSONWriter->WriteObjectStart();
		InJSONWriter->WriteValue(TEXT("#"), tag->GetText());
		InJSONWriter->WriteObjectEnd();
		return;
	}

	// Used when serialising save state only
	TSharedPtr<Ink::FChoice> choice = FObject::DynamicCastTo<Ink::FChoice>(InObject);
	if (choice.IsValid())
	{
		WriteChoice(InJSONWriter, *choice);
		return;
	}

	UE_LOG(InkPlusPlus, Error, TEXT("Failed to write runtime object to JSON: %s"), *InObject->ToString());
}

void Ink::FJsonSerialisation::WriteRuntimeContainer(TJsonWriter<>* InJSONWriter,const Ink::FContainer& InContainer, bool InWithoutName)
{
	InJSONWriter->WriteArrayStart();

	for (const TSharedPtr<Ink::FObject>& object : *(InContainer.GetContent()))
	{
		WriteRuntimeObject(InJSONWriter, object);
	}

	// Container is always an array [...]
	// But the final element is always either:
	//  - a dictionary containing the named content, as well as possibly
	//    the key "#" with the count flags
	//  - null, if neither of the above
	const TMap<FString, TSharedPtr<Ink::FObject>> namedOnlyContent = InContainer.GetNamedOnlyContent();
	const int32 countFlags = InContainer.GetCountFlags();
	const bool hasNameProperty = !InContainer.GetName().IsEmpty() && !InWithoutName;

	const bool hasTerminator = namedOnlyContent.Num() != 0 || countFlags > 0 || hasNameProperty;

	if (hasTerminator)
		InJSONWriter->WriteObjectStart();

	if (namedOnlyContent.Num() != 0)
	{
		for (const TPair<FString, TSharedPtr<Ink::FObject>>& namedContent : namedOnlyContent)
		{
			FString name = namedContent.Key;
			Ink::FContainer* namedContainer = static_cast<Ink::FContainer*>(namedContent.Value.Get());
			InJSONWriter->WriteIdentifierPrefix(name);
			WriteRuntimeContainer(InJSONWriter, *namedContainer, true);
		}
	}

	if (countFlags > 0)
		InJSONWriter->WriteValue("#f", countFlags);

	if (hasNameProperty)
		InJSONWriter->WriteValue("#n", InContainer.GetName());

	if (hasTerminator)
		InJSONWriter->WriteObjectEnd();
	else
		InJSONWriter->WriteNull();

	InJSONWriter->WriteArrayEnd();
}

void Ink::FJsonSerialisation::WriteChoice(TJsonWriter<>* InJSONWriter, const Ink::FChoice& InChoice)
{
	InJSONWriter->WriteObjectStart();
	InJSONWriter->WriteValue(TEXT("text"), InChoice.GetText());
	InJSONWriter->WriteValue(TEXT("index"), InChoice.GetIndex());
	InJSONWriter->WriteValue(TEXT("originalChoicePath"), InChoice.GetSourcePath());
	InJSONWriter->WriteValue(TEXT("originalThreadIndex"), InChoice.GetOriginalThreadIndex());
	InJSONWriter->WriteValue(TEXT("targetPath"), InChoice.GetPathStringOnChoice());
	InJSONWriter->WriteObjectEnd();
}

void Ink::FJsonSerialisation::WriteInkList(TJsonWriter<>* InJSONWriter, const Ink::FValueList& InListValue)
{
	const Ink::FInkList rawList = InListValue.GetValue();

	InJSONWriter->WriteObjectStart();
	InJSONWriter->WriteIdentifierPrefix(TEXT("list"));
	
	InJSONWriter->WriteObjectStart();
	for (const TPair<Ink::FInkListItem, int32>& pair : rawList)
	{
		const Ink::FInkListItem item = pair.Key;
		const int32 itemValue = pair.Value;

		InJSONWriter->WriteIdentifierPrefix(FString());

		FString value = !item.OriginName.IsEmpty() ? item.OriginName : TEXT("");
		value += TEXT(".");
		value += item.ItemName;
		InJSONWriter->WriteValue(value);
		
		InJSONWriter->WriteValue(itemValue);
	}

	InJSONWriter->WriteObjectEnd();
	
	if (rawList.Num() == 0 && rawList.GetOriginNames().IsValid() && rawList.GetOriginNames()->Num() > 0)
	{
		InJSONWriter->WriteValue(TEXT("origins"), *rawList.GetOriginNames());
	}

	InJSONWriter->WriteObjectEnd();
}

TSharedPtr<Ink::FObject> Ink::FJsonSerialisation::JsonTokenToRuntimeObject(const FJsonValue& InToken)
{
	if (InToken.Type == EJson::Number)
	{
		const FString tokenAsString = InToken.AsString();
		if (FDefaultValueHelper::IsStringValidInteger(tokenAsString))
		{
			int32 tokenAsInt;
			FDefaultValueHelper::ParseInt(tokenAsString, tokenAsInt);
			return Ink::FValue::Create(FValueType(tokenAsInt));
		}
		else
		{
			float tokenAsFloat;
			FDefaultValueHelper::ParseFloat(tokenAsString, tokenAsFloat);
			return Ink::FValue::Create(FValueType(tokenAsFloat));
		}
	}
	
	if (InToken.Type == EJson::Boolean)
		return Ink::FValue::Create(FValueType(InToken.AsBool()));

	if (InToken.Type == EJson::String)
	{
		FString stringValue = InToken.AsString();

		// String
		if (stringValue.IsEmpty())
			return MakeShared<Ink::FValueString>(stringValue);

		const TCHAR firstCharacter = stringValue[0];
		if (firstCharacter == '^')
			return MakeShared<Ink::FValueString>(stringValue.Mid(1));
		if (firstCharacter == '\n' && stringValue.Len() == 1)
			return MakeShared<Ink::FValueString>("\n");

		// Glue
		if (stringValue == TEXT("<>"))
			return MakeShared<Ink::FGlue>();

		// Control commands
		for (int32 i = 0; i < ControlCommandNames.Num(); ++i)
		{
			FString commandName = ControlCommandNames[i];
			if (stringValue == commandName)
				return MakeShared<Ink::FControlCommand>(static_cast<Ink::ECommandType>(i));
		}

		// Native Functions
		// "^" conflicts with the way to identify strings, so now
		// we know it's not a string, we can convert back to the proper
		// symbol for the operator.
		if (stringValue == TEXT("L^"))
			stringValue = TEXT("^");

		if (Ink::FNativeFunctionCall::CallExistsWithName(stringValue))
			return Ink::FNativeFunctionCall::CallWithName(stringValue);

		// Pop
		if (stringValue == TEXT("->->"))
			return MakeShared<Ink::FControlCommand>(Ink::ECommandType::PopTunnel);;
		if (stringValue == TEXT("~ret"))
			return MakeShared<Ink::FControlCommand>(Ink::ECommandType::PopFunction);;

		// Void
		if (stringValue == TEXT("void"))
			return MakeShared<Ink::FVoid>();
	}

	if (InToken.Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> objectValue = InToken.AsObject();
		TSharedPtr<FJsonValue> propertyValue;

		// Divert target value to path
		if (Ink::FJsonExtension::TryGetField(TEXT("^->"), *objectValue, propertyValue))
		{
			TSharedPtr<Ink::FPath> path = MakeShared<Ink::FPath>(propertyValue->AsString());
			return MakeShared<Ink::FValueDivertTarget>(path);
		}

		// Variable Pointer Value
		if (Ink::FJsonExtension::TryGetField(TEXT("^var"), *objectValue, propertyValue))
		{
			TSharedPtr<Ink::FValueVariablePointer> variablePointer = MakeShared<Ink::FValueVariablePointer>(propertyValue->AsString());
			if (Ink::FJsonExtension::TryGetField(TEXT("ci"), *objectValue, propertyValue))
					variablePointer->SetContextIndex(propertyValue->AsNumber());
			return variablePointer;
		}

		// Divert
		bool isDivert = false;
		bool pushesToStack = false;
		Ink::EPushPopType divertPushType = EPushPopType::Function;
		bool isExternal = false;

		if (Ink::FJsonExtension::TryGetField(TEXT("->"), *objectValue, propertyValue))
		{
			isDivert = true;
		}
		else if (Ink::FJsonExtension::TryGetField(TEXT("f()"), *objectValue, propertyValue))
		{
			isDivert = true;
			pushesToStack = true;
			divertPushType = Ink::EPushPopType::Function;
		}
		else if (Ink::FJsonExtension::TryGetField(TEXT("->t->"), *objectValue, propertyValue))
		{
			isDivert = true;
			pushesToStack = true;
			divertPushType = Ink::EPushPopType::Tunnel;
		}
		else if (Ink::FJsonExtension::TryGetField(TEXT("x()"), *objectValue, propertyValue))
		{
			isDivert = true;
			pushesToStack = false;
			isExternal = true;
			divertPushType = Ink::EPushPopType::Function;
		}

		if (isDivert)
		{
			TSharedPtr<Ink::FDivert> divert = MakeShared<Ink::FDivert>();
			divert->SetPushesToStack(pushesToStack);
			divert->SetStackPushType(divertPushType);
			divert->SetIsExternal(isExternal);

			FString target = propertyValue->AsString();
			if (Ink::FJsonExtension::TryGetField(TEXT("var"), *objectValue, propertyValue))
				divert->SetVariableDivertName(target);
			else
				divert->SetTargetPathString(target);

			divert->SetIsConditional(objectValue->HasField(TEXT("c")));

			if (isExternal)
			{
				if (Ink::FJsonExtension::TryGetField(TEXT("exArgs"), *objectValue, propertyValue))
				{
					int32 args = 0;
					propertyValue->TryGetNumber(args);
					divert->SetExternalArgs(args);
				}

			}

			return divert;
		}

		// Choice
		if (Ink::FJsonExtension::TryGetField(TEXT("*"), *objectValue, propertyValue))
		{
			TSharedPtr<Ink::FChoicePoint> choice = MakeShared<Ink::FChoicePoint>();
			choice->SetPathStringOnChoice(propertyValue->AsString());

			if (Ink::FJsonExtension::TryGetField(TEXT("flg"), *objectValue, propertyValue))
			{
				int32 flags = 0;
				propertyValue->TryGetNumber(flags);
				choice->SetFlags(flags);
			}

			return choice;
		}

		// Variable Reference
		if (Ink::FJsonExtension::TryGetField(TEXT("VAR?"), *objectValue, propertyValue))
			return MakeShared<Ink::FVariableReference>(propertyValue->AsString());
		
		if (Ink::FJsonExtension::TryGetField(TEXT("CNT?"), *objectValue, propertyValue))
		{
			TSharedPtr<Ink::FVariableReference> readCountVariableReference = MakeShared<Ink::FVariableReference>();
			readCountVariableReference->SetPathStringForCount(propertyValue->AsString());
			return readCountVariableReference;
		}

		// Variable Assignment
		bool isVariableAssignment = false;
		bool isGlobalVariable = false;
		if (Ink::FJsonExtension::TryGetField(TEXT("VAR="), *objectValue, propertyValue))
		{
			isVariableAssignment = true;
			isGlobalVariable = true;
		}
		else if (Ink::FJsonExtension::TryGetField(TEXT("temp="), *objectValue, propertyValue))
		{
			isVariableAssignment = true;
			isGlobalVariable = false;
		}

		if (isVariableAssignment)
		{
			FString variableName = propertyValue->AsString();
			bool isNewDeclaration = !objectValue->HasField(TEXT("re"));
			TSharedPtr<Ink::FVariableAssignment> variableAssignment = MakeShared<Ink::FVariableAssignment>(variableName, isNewDeclaration);
			variableAssignment->IsGlobal(isGlobalVariable);
			return variableAssignment;
		}

		// Tag
		if (Ink::FJsonExtension::TryGetField(TEXT("#"), *objectValue, propertyValue))
			return MakeShared<Ink::FTag>(propertyValue->AsString());

		// List Value
		if (Ink::FJsonExtension::TryGetField(TEXT("list"), *objectValue, propertyValue))
		{
			TMap<FString, TSharedPtr<FJsonValue>> listContent = propertyValue->AsObject()->Values;
			
			Ink::FInkList rawList;			
			if (Ink::FJsonExtension::TryGetField(TEXT("origins"), *objectValue, propertyValue))
			{
				const TArray<TSharedPtr<FJsonValue>> namesAsObjects = propertyValue->AsArray();
				
				TSharedPtr<TArray<FString>> objectNames = MakeShared<TArray<FString>>();
				objectNames->Reserve(namesAsObjects.Num());
				for (const TSharedPtr<FJsonValue>& value : namesAsObjects)
				{
					objectNames->Emplace(value->AsString());
				}
				rawList.SetInitialOriginNames(objectNames);
			}

			rawList.Reserve(listContent.Num());
			for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : listContent)
			{
				Ink::FInkListItem item(pair.Key);
				int32 value = 0;
				pair.Value->TryGetNumber(value);
				rawList.Add(item, value);
			}

			return MakeShared<Ink::FValueList>(rawList);
		}

		// Used when serialising save state only
		if (objectValue->HasField(TEXT("originalChoicePath")))
			return JsonObjectToChoice(*objectValue);
	}
	
	if (InToken.Type == EJson::Array)
	{
		return JsonArrayToContainer(InToken.AsArray());
	}

	if (InToken.IsNull())
		return nullptr;

	//UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to convert token {%s} to runtime object :("), *InToken.AsString());
	return nullptr;
}

TSharedPtr<Ink::FListDefinitionsOrigin> Ink::FJsonSerialisation::JsonTokenToListDefinitions(const FJsonValue& InToken)
{
	const TSharedPtr<FJsonObject>* definitionsObject;
	if (!InToken.TryGetObject(definitionsObject))
	{
		UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to extract definitions object from passed in json token!"));
		return nullptr;
	}

	TArray<TSharedPtr<Ink::FListDefinition>> allDefinitions;
	for (const TPair<FString, TSharedPtr<FJsonValue>>& pair : definitionsObject->Get()->Values)
	{
		const FString name = pair.Key;
		const TSharedPtr<FJsonObject>* listDefinitionJson;
		if (!pair.Value->TryGetObject(listDefinitionJson))
		{
			UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to get object out of definitions object"))
			continue;
		}

		// Cast (string, value) to (string, int) for items
		TMap<FString, int32> items;
		items.Reserve(listDefinitionJson->Get()->Values.Num());
		for (const TPair<FString, TSharedPtr<FJsonValue>>& definitionsPair : listDefinitionJson->Get()->Values)
		{
			int32 value;
			if (!definitionsPair.Value->TryGetNumber(value))
			{
				UE_LOG(InkPlusPlus, Error, TEXT("JsonSerialisation : failed to get int value out of definition pair"));
				continue;
			}
			items.Add(definitionsPair.Key, value);
		}

		TSharedPtr<Ink::FListDefinition> listDefinition = MakeShared<Ink::FListDefinition>(name, items);
		allDefinitions.Add(listDefinition);
	}

	return MakeShared<Ink::FListDefinitionsOrigin>(allDefinitions);
}
