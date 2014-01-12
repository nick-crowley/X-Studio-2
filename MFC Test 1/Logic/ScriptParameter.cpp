#include "stdafx.h"
#include "ScriptParameter.h"
#include "ScriptFile.h"
#include "StringLibrary.h"


namespace Logic
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
         const WCHAR* format;

         switch (Type)
         {
         case DataType::DT_VARIABLE:
            format = (Syntax.IsRetVar() ? L"$%s =" : L"$%s");

            // Commented: name
            if (Value.Type == ValueType::String)
               Text = StringResource::Format(format, Value.String.c_str());

            // Default: conditional/variable
            else switch (ReturnValue(Value.Int).ReturnType)
            {
            case ReturnType::ASSIGNMENT:     Text = StringResource::Format(format, f.Variables[Value.Int].Name.c_str());   break;
            case ReturnType::DISCARD:        Text = L"";   break;
            case ReturnType::JUMP_IF_TRUE:
            case ReturnType::JUMP_IF_FALSE:  Text = StringLib.Find(KnownPage::CONDITIONALS, (UINT)ReturnValue(Value.Int).Conditional).Text;  break;

            default:  throw InvalidValueException(HERE, GuiString(L"Unrecognised return type 0x%x", Value.Int));
            }
            break;

         case DataType::DT_STRING:
            Text = (Syntax.Type == ParameterType::COMMENT || Syntax.Type == ParameterType::LABEL_NAME ? Value.String 
                    : StringResource::Format(L"'%s'", Value.String.c_str()));
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

         case DataType::DT_SCRIPTDEF:        Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::PARAMETER_TYPES, Value.Int).Text.c_str());  break;
         case DataType::DT_STATIONSERIAL:    Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::STATION_SERIALS, Value.Int).Text.c_str());  break;
         case DataType::DT_TRANSPORTCLASS:   Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::TRANSPORT_CLASSES, Value.Int).Text.c_str());  break;
         case DataType::DT_OPERATOR:         Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::OPERATORS, Value.Int).Text.c_str());  break;
         case DataType::DT_DATATYPE:         Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::DATA_TYPES, Value.Int).Text.c_str());  break;
         case DataType::DT_FLIGHTRETURN:     Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::FLIGHT_RETURNS, Value.Int).Text.c_str());  break;
         case DataType::DT_OBJECTCLASS:      Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::OBJECT_CLASSES, Value.Int).Text.c_str());  break;
         case DataType::DT_OBJECTCOMMAND:    Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::OBJECT_COMMANDS, Value.Int).Text.c_str());  break;
         case DataType::DT_WINGCOMMAND:      Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::WING_COMMANDS, Value.Int).Text.c_str());  break;
         case DataType::DT_RACE:             Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::RACES, Value.Int).Text.c_str());  break;
         case DataType::DT_CONSTANT:         Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::CONSTANTS, Value.Int).Text.c_str());  break;

         case DataType::DT_WARE:
            Text = StringResource::Format(L"{%d:%d}", HIWORD(Value.Int), LOWORD(Value.Int));
            break;
         }

         // RefObj: Append indirection operator
         if (Syntax.IsRefObj())
            Text += L" ->";
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

