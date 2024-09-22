#include "Utility/InkPlusPlusUtility.h"
#include "Utility/InkPlusPlusLog.h"

#include "Ink/ChoicePoint.h"
#include "Ink/Container.h"
#include "Ink/ControlCommand.h"
#include "Ink/Divert.h"
#include "Ink/Glue.h"
#include "Ink/NativeFunctionCall.h"
#include "Ink/Tag.h"
#include "Ink/Value.h"
#include "Ink/VariableAssignment.h"
#include "Ink/VariableReference.h"
#include "Ink/Void.h"

namespace Ink
{

	template<typename T> 
	TSharedPtr<Ink::FObject> CopyInkObjectAs( TSharedPtr<Ink::FObject> InObj )
	{
		TSharedPtr<T> thisOldObj = StaticCastSharedPtr<T>( InObj );
		TSharedPtr<T> thisNewObj = MakeShared<T>( *thisOldObj.Get() );
		TSharedPtr<Ink::FObject> newObj = StaticCastSharedPtr<Ink::FObject>( thisNewObj) ;
		return newObj;
	}

	TSharedPtr<Ink::FObject> CopyInkObject( TSharedPtr<Ink::FObject> InObj )
	{
		TSharedPtr<Ink::FObject> newObj;

		if(InObj->CanCastTo(Ink::EInkObjectClass::FChoicePoint         	))     
			newObj = CopyInkObjectAs<Ink::FChoicePoint         >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FContainer			)) 
			newObj = CopyInkObjectAs<Ink::FContainer           >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FControlCommand		)) 
			newObj = CopyInkObjectAs<Ink::FControlCommand      >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FDivert				)) 
			newObj = CopyInkObjectAs<Ink::FDivert              >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FGlue				)) 
			newObj = CopyInkObjectAs<Ink::FGlue                >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FNativeFunctionCall	)) 
			newObj = CopyInkObjectAs<Ink::FNativeFunctionCall  >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FTag					)) 
			newObj = CopyInkObjectAs<Ink::FTag                 >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueBool			)) 
			newObj = CopyInkObjectAs<Ink::FValueBool           >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueDivertTarget	)) 
			newObj = CopyInkObjectAs<Ink::FValueDivertTarget   >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueFloat			)) 
			newObj = CopyInkObjectAs<Ink::FValueFloat          >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueInt			)) 
			newObj = CopyInkObjectAs<Ink::FValueInt            >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueList			)) 
			newObj = CopyInkObjectAs<Ink::FValueList           >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueString			)) 
			newObj = CopyInkObjectAs<Ink::FValueString         >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValueVariablePointer)) 
			newObj = CopyInkObjectAs<Ink::FValueVariablePointer>( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FValue				)) 
			newObj = CopyInkObjectAs<Ink::FValue               >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FVariableAssignment	)) 
			newObj = CopyInkObjectAs<Ink::FVariableAssignment  >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FVariableReference	)) 
			newObj = CopyInkObjectAs<Ink::FVariableReference   >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FVoid				)) 
			newObj = CopyInkObjectAs<Ink::FVoid                >( InObj );
		else if(InObj->CanCastTo(Ink::EInkObjectClass::FObject				)) 
			newObj = CopyInkObjectAs<Ink::FObject              >( InObj );
		else 
			UE_LOG( InkPlusPlus, Error,   TEXT( "Attempt to copy Ink object of unknown type") );	

		return newObj;
	}


}