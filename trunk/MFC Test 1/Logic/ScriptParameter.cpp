#include "stdafx.h"
#include "ScriptParameter.h"
#include "ScriptFile.h"

namespace Library
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, ParameterValue val) : Syntax(s), Type(t), Value(val) 
      { 
         //Translate(); 
      }

      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, const wstring& val) : Syntax(s), Type(t), Value(val) 
      { 
         //Translate(); 
      }

      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, UINT val) : Syntax(s), Type(t), Value(val) 
      { 
         //Translate(); 
      }

      ScriptParameter::~ScriptParameter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      void   ScriptParameter::Translate(ScriptFile& f)
      {
         if (Value.Type == ValueType::String)
            Text = Value.String;
         else
            Text = GuiString(L"%d", Value.Int);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

