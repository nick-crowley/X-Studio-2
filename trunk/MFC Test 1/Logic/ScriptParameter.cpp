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

         switch (Type)
         {
         case DataType::DT_VARIABLE:
            Text = Value.Type == ValueType::String ? Value.String 
                 : Value.Int >= 0 ? f.Variables[Value.Int].Name
                 : L"if ";
            Text.insert(Text.begin(), L'$');
            break;

         case DataType::DT_STRING:
            Text = (Syntax.Type == ParameterType::PS_COMMENT ? Value.String : StringResource::Format(L"'%s'", Value.String.c_str()));
            break;

         case DataType::DT_INTEGER:
            Text = StringResource::Format(L"%d", Value.Int);
            break;

         case DataType::DT_NULL:
            Text = L"null";
            break;

         case DataType::DT_SECTOR:
            Text = StringResource::Format(L"[%d]", Value.Int);
            break;

         case DataType::DT_SCRIPTDEF:
         case DataType::DT_STATIONSERIAL:
         case DataType::DT_TRANSPORTCLASS:
         case DataType::DT_OPERATOR:
         case DataType::DT_DATATYPE:
         case DataType::DT_FLIGHTRETURN:
         case DataType::DT_OBJECTCLASS:
         case DataType::DT_OBJECTCOMMAND:
         case DataType::DT_WINGCOMMAND:
         case DataType::DT_RACE:
         case DataType::DT_CONSTANT:
            Text = StringResource::Format(L"[%d]", Value.Int);
            break;

         case DataType::DT_WARE:
            Text = StringResource::Format(L"{%d:%d}", HIWORD(Value.Int), LOWORD(Value.Int));
            break;
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

