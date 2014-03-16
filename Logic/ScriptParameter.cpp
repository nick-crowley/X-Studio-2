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

      /// <summary>Create a retVar, refObj, standard, expression, or argument parameter from a PARSED SCRIPT TOKEN</summary>
      /// <param name="s">syntax</param>
      /// <param name="tok">script token</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, const ScriptToken& t) 
         : Syntax(s), Token(t), Type(IdentifyDataType(t)), Value(t.ValueText), Text(t.Text)
      {}

      /// <summary>Create an argument parameter for a PARSED VARG SCRIPT CALL</summary>
      /// <param name="name">Argument name token</param>
      /// <param name="val">Value token</param>
      ScriptParameter::ScriptParameter(const ScriptToken& name, const ScriptToken& val) 
         : Syntax(ParameterSyntax::ScriptCallArgument), 
           Type(IdentifyDataType(val)), 
           Token(val), 
           Value(val.ValueText), 
           Text(val.Text), 
           ArgName(name)
      {}

      /// <summary>Create a conditional retVar parameter from a PARSED CONDITIONAL</summary>
      /// <param name="s">syntax</param>
      /// <param name="c">The conditional.</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, Conditional c) 
         : Syntax(s), Type(DataType::VARIABLE), Value(ReturnValue(c, EMPTY_JUMP).EncodedValue), Text(L"conditional")
      {}
   
      /// <summary>Create a parameter from a string/int value READ FROM FILE</summary>
      /// <param name="s">syntax</param>
      /// <param name="t">data type</param>
      /// <param name="val">The value.</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, DataType t, ParameterValue val) : Syntax(s), Type(t), Value(val)
      {}

      /// <summary>Create a parameter from a string value READ FROM FILE</summary>
      /// <param name="s">syntax</param>
      /// <param name="t">data type</param>
      /// <param name="val">The value.</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, DataType t, const wstring& val) : Syntax(s), Type(t), Value(val) 
      {}

      /// <summary>Create a parameter from an int value READ FROM FILE</summary>
      /// <param name="s">syntax</param>
      /// <param name="t">data type</param>
      /// <param name="val">The value.</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, DataType t, UINT val) : Syntax(s), Type(t), Value(val) 
      {}

      ScriptParameter::~ScriptParameter()
      {}

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get datatype string</summary>
      GuiString  GetString(DataType d)
      {
         const LanguageString* str;
         return StringLib.TryFind(KnownPage::DATA_TYPES, (UINT)d, str) ? str->Text : GuiString::Missing((UINT)d);
      }

      /// <summary>Get value type string</summary>
      GuiString  GetString(ValueType v)
      {
         return v == ValueType::Int ? L"Int" 
            : v == ValueType::String ? L"String" : L"Invalid";
      }

      /// <summary>Get return type string</summary>
      GuiString  GetString(ReturnType t)
      {
         switch (t)
         {
         case ReturnType::ASSIGNMENT:     return L"ASSIGNMENT";
         case ReturnType::DISCARD:        return L"DISCARD";
         case ReturnType::JUMP_IF_TRUE:   return L"JUMP_IF_TRUE";
         case ReturnType::JUMP_IF_FALSE:  return L"JUMP_IF_FALSE";
         }
         return L"Invalid";
      }

      /// <summary>Get conditional string</summary>
      GuiString  GetString(Conditional c)
      {
         return StringLib.Find(KnownPage::CONDITIONALS, (UINT)c).Text;
      }

      /// <summary>Identify data-type from a token type</summary>
      /// <param name="type">token type.</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidOperationException">Unsupported token type</exception>
      DataType  ScriptParameter::IdentifyDataType(ScriptToken tok)
      {
         const ScriptObject* obj = nullptr;

         // Examine token
         switch (tok.Type)
         {
         case TokenType::Comment:
         case TokenType::Label:
         case TokenType::String: 
            return DataType::STRING;

         case TokenType::Variable:
            return DataType::VARIABLE;

         case TokenType::GameObject:
            return DataType::WARE;

         // Lookup script object to identify data-type
         case TokenType::ScriptObject:
            return ScriptObjectLib.TryFind(tok.ValueText, obj) ? obj->GetDataType() : DataType::CONSTANT;

         case TokenType::BinaryOp:
         case TokenType::UnaryOp:
            return DataType::OPERATOR;

         case TokenType::Number:
            return DataType::INTEGER;

         case TokenType::Null:
            return DataType::Null;

         
         default:
         case TokenType::Text:
         case TokenType::Keyword:
         case TokenType::Whitespace:
            throw InvalidOperationException(HERE, VString(L"Cannot create parameters from '%s'", tok.Text.c_str()));
         }
      }

      /// <summary>Write parameter value to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const ParameterValue& val)
      {
         auto szValue = (val.Type == ValueType::String ? val.String : VString(L"%d", val.Int));
         return c << "{ParameterValue: Type=" << GetString(val.Type) << " Value=" << szValue << "}";
      }

      /// <summary>Write script parameter to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptParameter& p)
      {
         return c << "{ScriptParameter: Text=" << p.Text << " Value=" << p.Value << " Token=" << p.Token << " Syntax=" << p.Syntax << "}";
      }

      /// <summary>Write return value to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const ReturnValue& val)
      {
         return c << L"{ReturnValue: ReturnType=" << GetString(val.ReturnType) << " Conditional='" << GetString(val.Conditional) << "' Destination=" << val.Destination << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Generates value from text</summary>
      /// <param name="script">script used for variable ID lookup</param>
      /// <param name="jumpDestination">Zero-based jump destination</param>
      /// <param name="commented">Whether parameter belongs to a command comment</param>
      /// <exception cref="Logic::AlgorithmException">Invalid jump destination</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid script object ID</exception>
      void  ScriptParameter::Generate(ScriptFile& script, UINT jumpDestination, bool commented)
      {
         const ScriptObject* obj = nullptr;
         GameObjectLibrary::ObjectID  ware;

         // Commented: Use zero for all jump destinations
         if (commented)
            jumpDestination = NULL;

         switch (Type)
         {
         // Variable: Conditional/RetVar/Variable
         case DataType::VARIABLE:
            // Variable: Resolve ID
            if (!Syntax.IsRetVar())
            {
               Type = DataType::LIVE_VARIABLE;
               // Leave as string if commented
               if (!commented)
                  Value = script.Variables[Value.String].ID;
            }
            // RetVar: Resolve and encode ID
            else if (Value.Type == ValueType::String)
            {
               if (!commented)   // Leave as string if commented
                  Value = ReturnValue(ReturnType::ASSIGNMENT, script.Variables[Value.String].ID).EncodedValue;
            }
            // Condition: encode appropriately
            else switch (Conditional condition = ReturnValue(Value.Int).Conditional)
            {
            // Discard/Start: no jump destination or variable ID
            case Conditional::DISCARD:
            case Conditional::START:
               Value = ReturnValue(condition, 0).EncodedValue;
               break;

            // None: I don't think this should ever happen
            case Conditional::NONE:
               throw AlgorithmException(HERE, L"Unexpected retVar 'NONE' conditional");
            
            // Conditional: Encode conditional + jump destination
            default:
               if (jumpDestination == EMPTY_JUMP)
                  throw AlgorithmException(HERE, L"Missing jump destination");

               // Encode jump destination into existing conditional
               Value = ReturnValue(condition, jumpDestination).EncodedValue;
            }
            break;

         // String: Label/Comment/String/Commented-LabelNumber
         case DataType::STRING:
            break;

         // Integer: Number/Uncommented-LabelNumber
         case DataType::INTEGER:
            // Goto/Gosub: Set jump address
            if (Syntax.Type == ParameterType::LABEL_NUMBER)
            {
               if (jumpDestination == EMPTY_JUMP)
                  throw AlgorithmException(HERE, L"Missing jump destination");
            
               // Set label number
               Value = jumpDestination;
            }
            else
               // Number: Str->Int
               Value = _wtoi(Value.String.c_str());
            break;

         // Null: Zero
         case DataType::Null:
            Value = 0;
            break;

         // Operator: 
         case DataType::OPERATOR:
            // ADD/MINUS/SUBTRACT: identify manually
            if (Token.Text == L"-")
               Value = (Token.Type == TokenType::BinaryOp ? (int)Operator::Subtract : (int)Operator::Minus);
            else if (Token.Text == L"+")
               Value = (int)Operator::Add;
            // LOGICAL AND/OR: Identify c++ operators I've added manually
            else if (Token.Text == L"||")
               Value = (int)Operator::LogicalOr;
            else if (Token.Text == L"&&")
               Value = (int)Operator::LogicalAnd;
            // MODULUS: I've added '%' manually, and MOD is lower case in ScriptObjectLib
            else if (Token.Text == L"%" || Token.Text == L"MOD")
               Value = (int)Operator::Modulus;
            else
               Value = ScriptObjectLib.Find(Value.String).ID; 

            // Unary: Add HIWORD
            if (Token.Type == TokenType::UnaryOp)
               Type = DataType::UNARY_OPERATOR;
            break;

         // Constant: Lookup + modify type
         case DataType::CONSTANT:
            Type = DataType::LIVE_CONSTANT;
            Value = (int)ScriptObjectLib.Find(Value.String).ID; 
            break;

         // Various: Lookup
         case DataType::DATATYPE:
         case DataType::FLIGHTRETURN:     
         case DataType::OBJECTCLASS:      
         case DataType::OBJECTCOMMAND:    
         case DataType::RACE:             
         case DataType::SCRIPTDEF:        
         case DataType::SECTOR:   
         case DataType::TRANSPORTCLASS:   
         case DataType::WINGCOMMAND:      
            Value = (int)ScriptObjectLib.Find(Value.String).ID;   
            break;

         // StationSerial: Lookup/encode
         case DataType::STATIONSERIAL:    
            Value = (int)StationSerialConverter::ToSerial( ScriptObjectLib.Find(Value.String).ID ); 
            break;

         // Relation: Lookup/encode
         case DataType::RELATION:
            Value = (int)RelationIDConverter::ToRelation( ScriptObjectLib.Find(Value.String).ID ); 
            break;

         // Ware: Lookup/Encode
         case DataType::WARE:
            // Encode ware placeholders '{SSTYPE_SHIP@232}' manually
            if (GameObjectLib.ParsePlaceholder(Value.String, ware))
               Value = GameObject::GetEncodedValue(ware.Type, ware.SubType);
            else
               Value = GameObjectLib.Find(Value.String).EncodedValue;
            break;
         }
      }

      /// <summary>Populates text property from parameter value</summary>
      /// <param name="script">Script used for variable name lookup</param>
      void   ScriptParameter::Translate(ScriptFile& script)
      {
         const WCHAR* format;

         switch (Type)
         {
         // Variable: Conditional/RetVar/Variable
         case DataType::VARIABLE:
            format = (Syntax.IsRetVar() ? L"$%s =" : L"$%s");

            // Commented: name
            if (Value.Type == ValueType::String)
               Text = GuiString::Format(format, Value.String.c_str());

            // Assignment: variable ID
            else if (ReturnValue(Value.Int).ReturnType == ReturnType::ASSIGNMENT)
               Text = GuiString::Format(format, script.Variables[(BYTE)Value.Int].Name.c_str());

            // Conditional/Discard:
            else switch (ReturnValue(Value.Int).Conditional)
            {
            case Conditional::NONE:
            case Conditional::DISCARD: Text = L"";       break;
            case Conditional::START:   Text = L"start";  break;   // HACK: library version is upper case
            case Conditional::IF:   
            case Conditional::IF_NOT:   
            case Conditional::WHILE:   
            case Conditional::WHILE_NOT:   
            case Conditional::ELSE_IF:   
            case Conditional::ELSE_IF_NOT:   
            case Conditional::SKIP_IF:   
            case Conditional::SKIP_IF_NOT:   Text = GetString(ReturnValue(Value.Int).Conditional); break;
            default:                   
               throw InvalidValueException(HERE, VString(L"Unrecognised return value: 0x%x", Value.Int));
            }
            break;

         // String: Label/Comment/String
         case DataType::STRING:
            if (Syntax.Type == ParameterType::COMMENT || Syntax.Type == ParameterType::LABEL_NAME)
               Text = Value.String;
            else
               Text = GuiString::Format(L"'%s'", Value.String.c_str());
            break;

         // Integer
         case DataType::INTEGER:
            Text = GuiString::Format(L"%d", Value.Int);
            break;

         // Null
         case DataType::Null:
            Text = L"null";
            break;

         // Operator: Format according to type
         case DataType::OPERATOR:
            switch (Operator op = (Operator)Value.LowWord)
            {
            case Operator::Minus:         Text = L"-";      return;   // HACK: Not present in script object lib
            case Operator::Add:           Text = L" + ";    return;   // HACK: Substitute for octal entity
            case Operator::Subtract:      Text = L" - ";    return;   // HACK: Not present in script object lib
            case Operator::Modulus:       Text = L" % ";    return;   // My addition
            case Operator::LogicalAnd:    Text = L" && ";   return;   // My addition   TODO: Preferences flag to determine which syntax to use
            case Operator::LogicalOr:     Text = L" || ";   return;   // My addition
            case Operator::LogicalNot:
            case Operator::BitwiseNot:
            case Operator::OpenBracket:   
            case Operator::CloseBracket:  format = L"%s";   break;   // Unary
            default:                      format = L" %s "; break;   // Binary 
            }
            // Lookup text
            Text = VString(format, ScriptObjectLib.Find(ScriptObjectGroup::Operator, Value.LowWord).Text.c_str()); 
            break;

         // Various: Strip HIWORD, then lookup ID
         case DataType::CONSTANT:
         case DataType::DATATYPE:
            Text = ScriptObjectLib.Find(Type, Value.LowWord).DisplayText;   
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
            Text = ScriptObjectLib.Find(Type, Value.Int).DisplayText;   
            break;

         // StationSerial: Map ID then convert
         case DataType::STATIONSERIAL:    
            Text = ScriptObjectLib.Find(Type, StationSerialConverter::ToStringID((Serial)Value.Int)).DisplayText;   
            break;

         // Relation: Map ID then convert
         case DataType::RELATION:
            Text = ScriptObjectLib.Find(Type, RelationIDConverter::ToStringID((Relation)Value.Int)).DisplayText;   
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
                  Text = VString(L"{%s@%d}", obj->Text.c_str(), Value.LowWord);
               else
                  Text = VString(L"{%s@%d}", GetString((MainType)Value.HighWord).c_str(), Value.LowWord);
            }
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

