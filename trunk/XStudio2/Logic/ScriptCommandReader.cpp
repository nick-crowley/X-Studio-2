#include "stdafx.h"
#include "ScriptFileReader.h"
#include "SyntaxLibrary.h" 

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads an auxiliary command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::AuxiliaryCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read Index + ID
         int index = ReadIntNode(L"command reference index");
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"auxiliary command ID"), Script.Game);

         // Comment: Read text
         if (syntax.ID == CMD_COMMENT)
            params += ReadParameter(syntax.Parameters[0], L"comment");

         // Return command
         return ScriptCommand(syntax, index, params, false);
      }

      /// <summary>Reads a commented command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::CommentedCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read Index, ID
         int index = ReadIntNode(L"commented reference index"),
                id = ReadIntNode(L"commented ID");
         
         // Lookup syntax for underlying command 
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"commented command ID"), Script.Game);

         // Read parameters
         switch (syntax.ID)
         {
         case CMD_EXPRESSION:
            // RetVar
            params += ReadParameter(syntax.Parameters[0], L"commented expression retVar");

            // Read parameters
            for (int i = 0, count = ReadIntNode(L"commented expression parameter count"); i < count; i++)
               params += ReadParameter(ParameterSyntax::ExpressionParameter, L"commented expression parameter");
            break;

         case CMD_CALL_SCRIPT_VAR_ARGS:
            // scriptname / RetVar / refObj
            params += ReadParameter(syntax.Parameters[0], L"commented scriptcall script-name");
            params += ReadParameter(syntax.Parameters[1], L"commented scriptcall retVar");
            params += ReadParameter(syntax.Parameters[2], L"commented scriptcall refObj");

            // Read parameters
            for (int i = 0, count = ReadIntNode(L"commented scriptcall argument count"); i < count; i++)
               params += ReadParameter(ParameterSyntax::ScriptCallArgument, L"commented scriptcall argument");
            break;
         
         default:
            // Read parameters by syntax
            for (ParameterSyntax p : syntax.Parameters)
               params += ReadParameter(p, L"commented parameter");
            break;
         }

         // Return command
         return ScriptCommand(syntax, index, params, true);
      }

      /// <summary>Reads an expression command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::ExpressionCommandReader::ReadCommand()
      {
         ParameterArray params, postfix;
         
         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"expression ID"), Script.Game);

         // Return value
         params += ReadParameter(syntax.Parameters[0], L"expression return value");

         // Postfix tuples
         for (int i = 0, count = ReadIntNode(L"expression postfix parameter count"); i < count; i++)
            postfix += ReadParameter(ParameterSyntax::ExpressionParameter, L"expression postfix parameter");

         // Interlace infix nodes with postfix tuples
         for (int i = 0, count = ReadIntNode(L"expression infix parameter count"); i < count; i++)
         {
            int value = ReadIntNode(L"expression infix index");    // +ve indicies are operators.  -ve indicies are a one-based index into the postfix array
            params += (value < 0 ? postfix[-value-1] : ScriptParameter(ParameterSyntax::ExpressionParameter, DataType::OPERATOR, value));
         }

         // Return command
         return ScriptCommand(syntax, params, false);
      }

      /// <summary>Reads a script-call command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::ScriptCallCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"script call ID"), Script.Game);

         // Read scriptname / RetVar / refObj
         params += ReadParameter(syntax.Parameters[0], L"script call target");
         params += ReadParameter(syntax.Parameters[1], L"script call return value");
         params += ReadParameter(syntax.Parameters[2], L"script call reference object");

         // Read arguments
         int count = ReadIntNode(L"script call argument count");
         for (int i = 0; i < count; i++)
            params += ReadParameter(ParameterSyntax::ScriptCallArgument, L"script call argument");

         // Return command
         return ScriptCommand(syntax, params, false);
      }

      /// <summary>Reads a standard command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::StandardCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"standard command ID"), Script.Game);

         // Read parameters
         for (ParameterSyntax p : syntax.Parameters)
            params += ReadParameter(p, L"standard parameter");

         // Return command
         return ScriptCommand(syntax, params, false);
      }
      
		// ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Reads a standard command</summary>
      /// <returns></returns>
      ScriptParameter ScriptFileReader::ScriptCommandReader::ReadParameter(ParameterSyntax s, const wchar* help)
      {
         DataType dt;

         // Read parameter
         switch (s.Type)
         {
         // Single int node
         case ParameterType::LABEL_NUMBER:   
            return ScriptParameter(s, DataType::STRING, ReadValueNode(help)); 

         // Old 'var' parameter
         case ParameterType::VARIABLE:
         // Decode RetVar as DT_VARIABLE
         case ParameterType::RETURN_VALUE:
         case ParameterType::RETURN_VALUE_IF:
         case ParameterType::RETURN_VALUE_IF_START:
         case ParameterType::INTERRUPT_RETURN_VALUE_IF: 
            return ScriptParameter(s, DataType::VARIABLE, ReadValueNode(help));   // int by default, string if comment
            
         // Single string node
         case ParameterType::COMMENT:        
         case ParameterType::SCRIPT_NAME:    
         case ParameterType::LABEL_NAME:     
            return ScriptParameter(s, DataType::STRING, ReadStringNode(help));

         // Parameter as {Type,Value} pair
         default:
            dt = ReadTypeNode(help);
            return ScriptParameter(s, dt, ReadValueNode(help));
         }
      }

      /// <summary>Reads the next node as a string.</summary>
      /// <param name="help">error text</param>
      /// <returns></returns>
      wstring  ScriptFileReader::ScriptCommandReader::ReadStringNode(const wchar* help)
      { 
         return Reader.ReadString(ReadNode(), help);   
      }

      /// <summary>Reads the next node as an int.</summary>
      /// <param name="help">error text</param>
      /// <returns></returns>
      int  ScriptFileReader::ScriptCommandReader::ReadIntNode(const wchar* help)
      { 
         return Reader.ReadInt(ReadNode(), help);
      }

      /// <summary>Reads the next node as an int/string.</summary>
      /// <param name="help">error text</param>
      /// <returns></returns>
      ParameterValue  ScriptFileReader::ScriptCommandReader::ReadValueNode(const wchar* help)
      { 
         return Reader.ReadValue(ReadNode(), help);
      }

      /// <summary>Reads the next node as a datatype.</summary>
      /// <param name="help">error text</param>
      /// <returns></returns>
      DataType  ScriptFileReader::ScriptCommandReader::ReadTypeNode(const wchar* help)
      { 
         return (DataType)LOWORD(ReadIntNode(help));
      }

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Get the next node.</summary>
      /// <returns></returns>
      XmlNodePtr  ScriptFileReader::ScriptCommandReader::ReadNode()
      { 
         return Command->childNodes->item[NodeIndex++]; 
      }
   }
}
