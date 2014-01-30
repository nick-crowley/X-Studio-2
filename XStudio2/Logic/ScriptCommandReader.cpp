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
         wstring        str;

         // Read ID & Index
         int index = ReadIntNode(L"command reference index"),
             id = ReadIntNode(L"auxiliary command ID");

         // Lookup syntax
         CommandSyntax syntax = SyntaxLib.Find(id, Script.Game);

         // Comment: Read string
         if (id == CMD_COMMENT)
            params.push_back( ScriptParameter(syntax.Parameters[0], DataType::STRING, ReadStringNode(L"comment")) );

         // Return command
         return ScriptCommand(syntax, index, params);
      }

      /// <summary>Reads a commented command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::CommentedCommandReader::ReadCommand()
      {
         throw NotImplementedException(HERE, L"Commented command reader not implemented");
      }

      /// <summary>Reads an expression command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::ExpressionCommandReader::ReadCommand()
      {
         ParameterArray params, postfix;
         int count;
         
         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"expression ID"), Script.Game);
         ParameterSyntax expr = syntax.Parameters[1];

         // Return value
         params.push_back( ScriptParameter(syntax.Parameters[0], DataType::VARIABLE, ReadIntNode(L"expression return value")) );

         // Read postfix tuples
         count = ReadIntNode(L"expression postfix parameter count");
         for (int i = 0; i < count; i++)
            postfix.push_back( ReadParameter(expr, L"expression postfix parameter") );

         // Generate parameters
         count = ReadIntNode(L"expression infix parameter count");
         for (int i = 0; i < count; i++)
         {
            int value = ReadIntNode(L"expression infix index");    // +ve indicies are operators.  -ve indicies are a one-based index into the postfix array
            params.push_back( value < 0 ? postfix[-value-1] : ScriptParameter(expr, DataType::OPERATOR, value) );
         }

         // Return command
         return ScriptCommand(syntax, params);
      }

      /// <summary>Reads a script-call command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::ScriptCallCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"script call ID"), Script.Game);

         // Read scriptname / RetVar / refObj
         params.push_back( ScriptParameter(syntax.Parameters[0], DataType::STRING, ReadStringNode(L"script call target")) );
         params.push_back( ScriptParameter(syntax.Parameters[1], DataType::VARIABLE, ReadIntNode(L"script call return value")) );
         params.push_back( ReadParameter(syntax.Parameters[2], L"script call reference object") );

         // Read arguments
         int count = ReadIntNode(L"script call argument count");
         for (int i = 0; i < count; i++)
            params.push_back( ReadParameter(ParameterSyntax::ScriptCallArgument, L"script call argument") );

         // Return command
         return ScriptCommand(syntax, params);
      }

      /// <summary>Reads a standard command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptFileReader::StandardCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLib.Find(ReadIntNode(L"standard command ID"), Script.Game);

         // Iterate thru syntax
         for (ParameterSyntax p : syntax.Parameters)
         {
            // TODO: Handle optional parameters here

            // Read parameter
            switch (p.Type)
            {
            case ParameterType::LABEL_NUMBER:   
               params.push_back( ScriptParameter(p, DataType::INTEGER, ReadIntNode(L"standard label number")) ); 
               break;

            // Decode RetVar as DT_VARIABLE
            case ParameterType::VARIABLE:       
            case ParameterType::RETURN_VALUE:
            case ParameterType::RETURN_VALUE_IF:
            case ParameterType::RETURN_VALUE_IF_START:
            case ParameterType::INTERRUPT_RETURN_VALUE_IF: 
               params.push_back( ScriptParameter(p, DataType::VARIABLE, ReadIntNode(L"standard retvar/variable")) ); 
               break;
            
            // Single string node
            case ParameterType::COMMENT:        
            case ParameterType::SCRIPT_NAME:    
            case ParameterType::LABEL_NAME:     
               params.push_back( ScriptParameter(p, DataType::STRING, ReadStringNode(L"standard label/script/comment")) );
               break;

            // Parameter as {Type,Value} pair
            default:
               params.push_back( ReadParameter(p, L"standard parameter") );
               break;
            }
         }

         // Return command
         return ScriptCommand(syntax, params);
      }
      
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
