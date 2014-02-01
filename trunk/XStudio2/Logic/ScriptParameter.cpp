#include "stdafx.h"
#include "ScriptParameter.h"
#include "ScriptFile.h"
#include "StringLibrary.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptParameter::ScriptParameter(ParameterSyntax s, const ScriptToken& t) : Syntax(s), Token(t)
      {
         Type = IdentifyDataType(t.Type);
         Value = Token.ValueText;
      }

      ScriptParameter::ScriptParameter(ParameterSyntax s, Conditional c) : Syntax(s)
      {
         Type = DataType::VARIABLE;
         Value = ReturnValue(c).Value;
      }
   
      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, ParameterValue val) : Syntax(s), Type(t), Value(val) 
      {}

      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, const wstring& val) : Syntax(s), Type(t), Value(val) 
      {}

      ScriptParameter::ScriptParameter(ParameterSyntax s, DataType t, UINT val) : Syntax(s), Type(t), Value(val) 
      {}

      ScriptParameter::~ScriptParameter()
      {}

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get datatype string</summary>
      GuiString  GetString(DataType d)
      {
         return StringLib.Contains(KnownPage::DATA_TYPES, (UINT)d) ? StringLib.Find(KnownPage::DATA_TYPES, (UINT)d).Text : GuiString(L"Missing string %d", d);
      }

      /// <summary>Converts a token type into a data-type</summary>
      /// <param name="type">token type.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unsupported token type</exception>
      DataType  ScriptParameter::IdentifyDataType(Compiler::TokenType type)
      {
         switch (type)
         {
         case TokenType::Comment:
         case TokenType::Label:
         case TokenType::String: 
            return DataType::STRING;

         case TokenType::Variable:
            return DataType::VARIABLE;

         case TokenType::GameObject:
         case TokenType::ScriptObject:
            return DataType::WARE;

         case TokenType::Number:
            return DataType::INTEGER;

         case TokenType::Null:
            return DataType::Null;

         case TokenType::Text:
            return DataType::UNKNOWN;
         }

         throw ArgumentException(HERE, L"type", GuiString(L"%s is an invalid token type for a parameter", GetString(type).c_str()));
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Populates text property from parameter value</summary>
      /// <param name="f">Script used for variable name lookup</param>
      void   ScriptParameter::Translate(ScriptFile& f)
      {
         const WCHAR* format;

         switch (Type)
         {
         // Variable: Conditional/RetVar/Variable
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

         // String: Comment/String
         case DataType::STRING:
            if (Syntax.Type == ParameterType::COMMENT || Syntax.Type == ParameterType::LABEL_NAME)
               Text = Value.String;
            else
               Text = StringResource::Format(L"'%s'", Value.String.c_str());
            break;

         // Integer
         case DataType::INTEGER:
            Text = StringResource::Format(L"%d", Value.Int);
            break;

         // Null
         case DataType::Null:
            Text = L"null";
            break;

         // Operator: Format according to type
         case DataType::OPERATOR:
            switch (Operator op = (Operator)Value.LowWord)
            {
            // HACK: Substitute for octal entity
            case Operator::Add:           format = L"+";    break; 
            // Unary
            case Operator::Minus:
            case Operator::LogicalNot:
            case Operator::BitwiseNot:
            case Operator::OpenBracket:   //format = L" %s";  break; 
            case Operator::CloseBracket:  format = L"%s";  break; 
            // Binary 
            default:                      format = L" %s "; break; 
            }
            // Lookup text
            Text = GuiString(format, StringLib.Find(KnownPage::OPERATORS, Value.LowWord).Text.c_str());  
            break;

         // Various: Strip HIWORD, then lookup ID
         case DataType::CONSTANT:
         case DataType::DATATYPE:
            Text = ScriptObjectLib.Find(ScriptObject::IdentifyGroup(Type), Value.LowWord).DisplayText;   
            break;

         // Various: Lookup ID
         case DataType::FLIGHTRETURN:     
         case DataType::OBJECTCLASS:      
         case DataType::OBJECTCOMMAND:    
         case DataType::RACE:             
         case DataType::SCRIPTDEF:        
         case DataType::SECTOR:   
         case DataType::TRANSPORTCLASS:   
         case DataType::WINGCOMMAND:      
            Text = ScriptObjectLib.Find(ScriptObject::IdentifyGroup(Type), Value.Int).DisplayText;   
            break;

         // StationSerial: Map ID then convert
         case DataType::STATIONSERIAL:    
            Text = ScriptObjectLib.Find(ScriptObject::IdentifyGroup(Type), StationSerialConverter::ToStringID((Serial)Value.Int)).DisplayText;   
            break;

         // Relation: Map ID then convert
         case DataType::RELATION:
            Text = ScriptObjectLib.Find(ScriptObject::IdentifyGroup(Type), RelationIDConverter::ToStringID((Relation)Value.Int)).DisplayText;   
            break;

         // Ware
         case DataType::WARE:
            if (GameObjectLib.Contains(Value.Int))
               Text = GameObjectLib.Find(Value.Int).DisplayText;
            else  
            {  
               const ScriptObject* obj;

               // Unrecognised placeholder:  {MAIN_TYPE@SUBTYPE}
               if (ScriptObjectLib.TryFind(ScriptObjectGroup::Constant, 200+Value.HighWord, obj))
                  Text = GuiString(L"{%s@%d}", obj->Text.c_str(), Value.LowWord);
               else
                  Text = GuiString(L"{%s@%d}", GetString((MainType)Value.HighWord).c_str(), Value.LowWord);
            }
            break;
         }

         // RefObj: Append indirection operator
         if (Syntax.IsRefObj())
            Text += L" ->";
      }


      /// <summary>Verifies the data-type against the syntax.</summary>
      /// <returns></returns>
      bool  ScriptParameter::Verify() const
      {
         // [NULL] Must not be RetVar
         if (Type == DataType::Null)
            return !Syntax.IsRetVar();

         /// Determine whether datatype is valid for the syntax
         switch (Syntax.Type)
         {
         // [INVALID] should never reach here
	      case ParameterType::UNDETERMINED:
         case ParameterType::CONDITION:  
         case ParameterType::LABEL_NUMBER:
            throw ArgumentException(HERE, L"Group", GuiString(L"Invalid parameter syntax: %s", GetString(Syntax.Type).c_str()));


         // [USER CUSTOMIZED] Unable to determine, allow anything
         default:
            return true;


         // [UNIVERSAL] These can be any datatype
         case ParameterType::VALUE:
         case ParameterType::EXPRESSION:
         case ParameterType::PARAMETER: 
            return true;

         // [FLIGHT RETURN] Stored using their own datatypes
	      case ParameterType::FLIGHTRETCODE:
            return Type == DataType::FLIGHTRETURN || Type == DataType::VARIABLE;

         // [LABEL NAME, SCRIPT NAME, STRING, COMMENT] Always held as strings
	      case ParameterType::LABEL_NAME:
	      case ParameterType::SCRIPT_NAME:
         case ParameterType::COMMENT:
         case ParameterType::STRING:
            return Type == DataType::STRING;

         // [NUMBER] Integers only
         case ParameterType::NUMBER:
            return Type == DataType::INTEGER;

         // [REFERENCE OBJECT] Constants and Variables only
	      case ParameterType::REFERENCE_OBJECT:
            return Type == DataType::VARIABLE || Type == DataType::CONSTANT;
   
         // [RELATIONS] Stored using their own datatypes
	      case ParameterType::RELATION:
            return Type == DataType::RELATION || Type == DataType::VARIABLE;

         // [RETURN OBJECT] Always held as integers
	      case ParameterType::RETURN_VALUE:
	      case ParameterType::RETURN_VALUE_IF:
	      case ParameterType::RETURN_VALUE_IF_START:
	      case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            return Type == DataType::INTEGER;

         // [SCRIPT REFERENCE TYPE]
         case ParameterType::SCRIPT_REFERENCE_TYPE:
            return Type == DataType::SCRIPTDEF || Type == DataType::VARIABLE;

         // [SECTOR POSITION] What about DataType::ARRAY?
	      case ParameterType::SECTOR_POSITION:
            return Type == DataType::VARIABLE;

         // [VARIABLE]
	      case ParameterType::VARIABLE:
            return Type == DataType::VARIABLE;



         
         // [ARRAY / VARIABLE]
	      case ParameterType::ARRAY:
            return Type == DataType::VARIABLE;

         // [BOOLEAN / VARIABLE]
         case ParameterType::VARIABLE_BOOLEAN:  /// Compatible with: [TRUE] and [FALSE]
            return Type == DataType::CONSTANT || Type == DataType::VARIABLE;

         // [CONSTANT / VARIABLE]
	      case ParameterType::VARIABLE_CONSTANT:
            return Type == DataType::CONSTANT || Type == DataType::VARIABLE;

         // [COMMAND / SIGNAL] 
	      case ParameterType::OBJECTCOMMAND:     
	      case ParameterType::OBJECTCOMMAND_OBJECTSIGNAL:
	      case ParameterType::OBJECTSIGNAL:
            return Type == DataType::OBJECTCOMMAND || Type == DataType::VARIABLE;

         // [DATATYPE / VARIABLE]
	      case ParameterType::VARIABLE_DATATYPE:
            return Type == DataType::DATATYPE || Type == DataType::VARIABLE;

         // [ENVIRONMENT / VARIABLE] 
         case ParameterType::VARIABLE_ENVIRONMENT: /// Compatible with: [ENVIRONMENT]
            return Type == DataType::CONSTANT || Type == DataType::VARIABLE;

         // [GLOBAL PARAMETER] Stored as string
         case ParameterType::VARIABLE_GLOBAL_PARAMETER:     /// NEW: Work in progress
            return Type == DataType::STRING || Type == DataType::VARIABLE;

         // [NUMBER / VARIABLE] Integer, Constant or Variable
         case ParameterType::VARIABLE_NUMBER:      /// Compatible with: [TRUE], [FALSE], [MAX]?
            return Type == DataType::INTEGER || Type == DataType::CONSTANT || Type == DataType::VARIABLE;

         // [OBJECT CLASS / VARIABLE] Stored using it's own datatypes
	      case ParameterType::VARIABLE_CLASS:
            return Type == DataType::OBJECTCLASS || Type == DataType::VARIABLE; // || Type == DataType::CONSTANT;

         // [PASSENGER / VARIABLE] What about DataType::PASSENGER?
         case ParameterType::VARIABLE_PASSENGER:
         case ParameterType::VARIABLE_PASSENGER_OF_SHIP:
            return Type == DataType::VARIABLE;

         // [QUEST / VARIABLE]      
         case ParameterType::VARIABLE_QUEST:    /// Compatible with: Quest.xxx constants
            return Type == DataType::VARIABLE || Type == DataType::QUEST || Type == DataType::CONSTANT;

         // [RACE / VARIABLE]
         case ParameterType::VARIABLE_RACE:     /// Possibly related: [OWNER]
            return Type == DataType::VARIABLE || Type == DataType::RACE || Type == DataType::CONSTANT;

         // [SECTOR / VARIABLE] 
         case ParameterType::VARIABLE_SECTOR:   /// Compatible with: [SECTOR]
            return Type == DataType::VARIABLE || Type == DataType::SECTOR || Type == DataType::CONSTANT;

         // [SHIP TYPE / STATION TYPE]
         case ParameterType::VARIABLE_SHIPTYPE:
	      case ParameterType::VARIABLE_SHIPTYPE_STATIONTYPE:
         case ParameterType::VARIABLE_STATIONTYPE:
            return Type == DataType::OBJECTCLASS || Type == DataType::VARIABLE || Type == DataType::WARE;

         // [SHIP / STATION / VARIABLE] Ship, station, constant or variable
         case ParameterType::VARIABLE_SHIP:              /// Compatible with: [SHIP]
         case ParameterType::VARIABLE_PLAYER_SHIP:
	      case ParameterType::VARIABLE_SHIP_STATION:
         case ParameterType::VARIABLE_PLAYER_SHIP_STATION:
	      case ParameterType::STATION_CARRIER:
	      case ParameterType::STATION_CARRIERDOCKAT:
         case ParameterType::VARIABLE_PLAYER_STATION_CARRIER:
	      case ParameterType::VARIABLE_PLAYER_STATION:
	      case ParameterType::VARIABLE_STATION:
	      case ParameterType::VARIABLE_STATIONPRODUCT:
	      case ParameterType::VARIABLE_STATIONRESOURCE:
	         return Type == DataType::SHIP || Type == DataType::STATION || Type == DataType::CONSTANT || 
                   Type == DataType::WARE || Type == DataType::VARIABLE;

         // [STATION SERIAL / VARAIBLE]
         case ParameterType::VARIABLE_STATIONSERIAL:
            return Type == DataType::VARIABLE || Type == DataType::STATIONSERIAL;

         // [STRING / VARIABLE] Variable, string or constant
         case ParameterType::VARIABLE_STRING:
            return Type == DataType::STRING || Type == DataType::VARIABLE || Type == DataType::CONSTANT;

         // [TRANSPORT CLASS / VARIABLE]
	      case ParameterType::VARIABLE_TRANSPORTCLASS:
            return Type == DataType::VARIABLE || Type == DataType::TRANSPORTCLASS; 

         // [UNIVERSE OBJECTS]
	      case ParameterType::VARIABLE_ASTEROID:
	      case ParameterType::VARIABLE_WARPGATE:
            return Type == DataType::VARIABLE;

         // [WARE / VARIABLE]
	      case ParameterType::VARIABLE_STATIONWARE:
	      case ParameterType::VARIABLE_HOMEBASEPRODUCT:
	      case ParameterType::VARIABLE_HOMEBASERESOURCE:
	      case ParameterType::VARIABLE_HOMEBASEWARE:
         case ParameterType::VARIABLE_ALLWARES:
	      case ParameterType::VARIABLE_FLYINGWARE:
	      case ParameterType::VARIABLE_WARE:
	      case ParameterType::VARIABLE_WARE_OF_SHIP:
            return Type == DataType::WARE || Type == DataType::VARIABLE;

         // [WING / VARIABLE]
         case ParameterType::VARIABLE_WING:     /// Compatible with Wing.xxx constants
            return Type == DataType::CONSTANT || Type == DataType::VARIABLE;
   
         // [WING COMMAND / VARIABLE]
         case ParameterType::VARIABLE_WING_COMMAND:
            return Type == DataType::WINGCOMMAND || Type == DataType::VARIABLE;




         // [SHIP AND WARE/PASSENGER] 
	      case ParameterType::VARIABLE_SHIP_AND_WARE:
         case ParameterType::VARAIBLE_SHIP_AND_PASSENGER:
         // [JUMP-DRIVE GATE/SECTOR] 
         case ParameterType::VARIABLE_JUMPDRIVE_GATE:
         case ParameterType::VARIABLE_JUMPDRIVE_SECTOR:
         // [FLEET COMMANDER] 
         case ParameterType::VARIABLE_FLEET_COMMANDER:   
            return Type == DataType::WARE || Type == DataType::VARIABLE;
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------
      
		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

