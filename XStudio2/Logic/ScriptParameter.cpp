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
         : Syntax(ModifySyntax(s)), Token(t), Type(IdentifyDataType(t)), Value(t.ValueText), Text(t.Text)
      {}

      /// <summary>Create a conditional retVar parameter from a PARSED CONDITIONAL</summary>
      /// <param name="s">syntax</param>
      /// <param name="c">The conditional.</param>
      ScriptParameter::ScriptParameter(const ParameterSyntax& s, Conditional c) 
         : Syntax(s), Type(DataType::VARIABLE), Value(ReturnValue(c, 0xffff).EncodedValue), Text(L"conditional")
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
         return StringLib.TryFind(KnownPage::DATA_TYPES, (UINT)d, str) ? str->Text : StringResource::Missing((UINT)d);
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

         case TokenType::Operator:
            return DataType::OPERATOR;

         case TokenType::Number:
            return DataType::INTEGER;

         case TokenType::Null:
            return DataType::Null;

         
         default:
         case TokenType::Text:
         case TokenType::Keyword:
         case TokenType::Whitespace:
            throw InvalidOperationException(HERE, GuiString(L"Cannot create parameters from '%s'", tok.Text.c_str()));
         }
      }

      /// <summary>Tweaks the syntax of parsed parameters.</summary>
      /// <param name="s">syntax</param>
      /// <returns>syntax</returns>
      const ParameterSyntax&  ScriptParameter::ModifySyntax(const ParameterSyntax& s)
      {
         // Convert label number (used by goto/gosub) into label name
         return s.Type != ParameterType::LABEL_NUMBER ? s : ParameterSyntax::LabelNameParameter;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Generates value from text</summary>
      /// <param name="script">script used for variable ID lookup</param>
      void  ScriptParameter::Generate(ScriptFile& script)
      {
         const ScriptObject* obj = nullptr;

         switch (Type)
         {
         // Variable: Conditional/RetVar/Variable
         case DataType::VARIABLE:
            // Variable: Resolve ID
            if (!Syntax.IsRetVar())
               Value = script.Variables[Value.String].ID;
            
            // RetVar/Discard: Resolve/Encode ID
            else if (Value.Type == ValueType::String || ReturnValue(Value.Int).Conditional == Conditional::DISCARD)
               Value = ReturnValue(ReturnType::ASSIGNMENT, script.Variables[Value.String].ID).EncodedValue;
            
            // Conditional: Encode conditional + jump destination
            else
               Value = ReturnValue(ReturnValue(Value.Int).Conditional, 0xffff).EncodedValue;
            break;

         // String: LabelDeclaration/Comment/String
         case DataType::STRING:
            break;

         // Integer: LabelNumber/Number
         case DataType::INTEGER:
            if (Syntax.Type != ParameterType::LABEL_NUMBER)
               Value = _wtoi(Value.String.c_str());
            break;

         // Null: Zero
         case DataType::Null:
            Value = 0;
            break;

         // Operator: Format according to type
         //case DataType::OPERATOR:
         //   switch (Operator op = (Operator)Value.LowWord)
         //   {
         //   // HACK: Substitute for octal entity
         //   case Operator::Add:           format = L"+";    break; 
         //   // Unary
         //   case Operator::Minus:
         //   case Operator::LogicalNot:
         //   case Operator::BitwiseNot:
         //   case Operator::OpenBracket:   //format = L" %s";  break; 
         //   case Operator::CloseBracket:  format = L"%s";  break; 
         //   // Binary 
         //   default:                      format = L" %s "; break; 
         //   }
         //   // Lookup text
         //   Text = GuiString(format, StringLib.Find(KnownPage::OPERATORS, Value.LowWord).Text.c_str());  
         //   break;

         // Various: Add HIWORD?
         case DataType::CONSTANT:
         case DataType::DATATYPE:
            Value = (int)ScriptObjectLib.Find(Value.String).ID; 
            break;

         // Various: Lookup
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
               Text = StringResource::Format(format, Value.String.c_str());

            // Default: conditional/variable
            else switch (ReturnValue(Value.Int).ReturnType)
            {
            case ReturnType::DISCARD:       Text = L"";   break;
            case ReturnType::ASSIGNMENT:    Text = StringResource::Format(format, script.Variables[Value.Int].Name.c_str());   break;
            case ReturnType::JUMP_IF_TRUE:
            case ReturnType::JUMP_IF_FALSE: Text = StringLib.Find(KnownPage::CONDITIONALS, (UINT)ReturnValue(Value.Int).Conditional).Text;  break;

            default:  throw InvalidValueException(HERE, GuiString(L"Unrecognised return type 0x%x", Value.Int));
            }
            break;

         // String: Label/Comment/String
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

		// ------------------------------ PROTECTED METHODS -----------------------------
      
		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

