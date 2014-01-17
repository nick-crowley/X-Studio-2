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
         case DataType::VARIABLE:
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

         case DataType::STRING:
            Text = (Syntax.Type == ParameterType::COMMENT || Syntax.Type == ParameterType::LABEL_NAME ? Value.String 
                    : StringResource::Format(L"'%s'", Value.String.c_str()));
            break;

         case DataType::INTEGER:
            Text = StringResource::Format(L"%d", Value.Int);
            break;

         case DataType::Null:
            Text = L"null";
            break;

         case DataType::SECTOR:
            Text = StringResource::Format(L"[%d]", Value.Int);
            break;

         case DataType::OPERATOR:
            switch ((Operator)Value.Int)
            {
            case Operator::CloseBracket:  Text = StringResource::Format(L"%s ", StringLib.Find(KnownPage::OPERATORS, Value.Int).Text.c_str());  break;
            case Operator::Minus:
            case Operator::LogicalNot:
            case Operator::BitwiseNot:
            case Operator::OpenBracket:   Text = StringResource::Format(L" %s", StringLib.Find(KnownPage::OPERATORS, Value.Int).Text.c_str());   break;
            default:                      Text = StringResource::Format(L" %s ", StringLib.Find(KnownPage::OPERATORS, Value.Int).Text.c_str());  break;
            }
            break;

         case DataType::SCRIPTDEF:        Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::PARAMETER_TYPES, Value.Int).Text.c_str());  break;
         case DataType::STATIONSERIAL:    Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::STATION_SERIALS, Value.Int).Text.c_str());  break;
         case DataType::TRANSPORTCLASS:   Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::TRANSPORT_CLASSES, Value.Int).Text.c_str());  break;
         case DataType::DATATYPE:         Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::DATA_TYPES, Value.Int).Text.c_str());  break;
         case DataType::FLIGHTRETURN:     Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::FLIGHT_RETURNS, Value.Int).Text.c_str());  break;
         case DataType::OBJECTCLASS:      Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::OBJECT_CLASSES, Value.Int).Text.c_str());  break;
         case DataType::OBJECTCOMMAND:    Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::OBJECT_COMMANDS, Value.Int).Text.c_str());  break;
         case DataType::WINGCOMMAND:      Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::WING_COMMANDS, Value.Int).Text.c_str());  break;
         case DataType::RACE:             Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::RACES, Value.Int).Text.c_str());  break;
         case DataType::CONSTANT:         Text = StringResource::Format(L"[%s]", StringLib.Find(KnownPage::CONSTANTS, Value.Int).Text.c_str());  break;

         case DataType::WARE:
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

