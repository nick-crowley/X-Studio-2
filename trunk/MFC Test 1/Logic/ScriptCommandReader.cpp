#include "stdafx.h"
#include "ScriptReader.h"
#include "SyntaxLibrary.h" 

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads an auxiliary command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptReader::AuxiliaryCommandReader::ReadCommand()
      {
         ParameterArray params;
         wstring        str;

         // Read ID & Index
         int index = ReadIntNode(),
             id = ReadIntNode();

         // Lookup syntax
         CommandSyntax syntax = SyntaxLibrary::Find(id, Script.Game);

         // Comment: Read string
         if (id == CMD_COMMENT)
            params.push_back( ScriptParameter(syntax.Parameters[0], DataType::DT_STRING, ReadStringNode()) );

         // Return command
         return ScriptCommand(syntax, index, params);
      }

      /// <summary>Reads a commented command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptReader::CommentedCommandReader::ReadCommand()
      {
         throw NotImplementedException(HERE, L"Commented command reader not implemented");
      }

      /// <summary>Reads an expression command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptReader::ExpressionCommandReader::ReadCommand()
      {
         ParameterArray params, postfix;
         int count;
         
         // Read ID + syntax
         CommandSyntax syntax = SyntaxLibrary::Find(ReadIntNode(), Script.Game);
         ParameterSyntax expr = syntax.Parameters[1];

         // Return value
         params.push_back( ScriptParameter(syntax.Parameters[0], DataType::DT_VARIABLE, ReadIntNode()) );

         // Read postfix tuples
         //ParameterArray postfix(count = ReadIntNode());
         count = ReadIntNode();
         for (int i = 0; i < count; i++)
            postfix.push_back( ScriptParameter(expr, ReadTypeNode(), ReadValueNode()) );

         // Generate parameters
         count = ReadIntNode();
         for (int i = 0; i < count; i++)
         {
            int value = ReadIntNode();    // Minus value indicates operator, position is index into postfix array
            params.push_back( value >= 0 ? postfix[value] : ScriptParameter(expr, DataType::DT_OPERATOR, -value) );
         }

         // Return command
         return ScriptCommand(syntax, params);
      }

      /// <summary>Reads a script-call command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptReader::ScriptCallCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLibrary::Find(ReadIntNode(), Script.Game);

         // Read scriptname / RetVar / refObj
         params.push_back( ScriptParameter(syntax.Parameters[0], DataType::DT_STRING, ReadStringNode()) );
         params.push_back( ScriptParameter(syntax.Parameters[1], DataType::DT_VARIABLE, ReadIntNode()) );
         params.push_back( ScriptParameter(syntax.Parameters[2], ReadTypeNode(), ReadValueNode()) );

         // Read arguments
         int count = ReadIntNode();
         for (int i = 0; i < count; i++)
            params.push_back( ScriptParameter(syntax.Parameters[3], ReadTypeNode(), ReadValueNode()) );

         // Return command
         return ScriptCommand(syntax, params);
      }

      /// <summary>Reads a standard command</summary>
      /// <returns></returns>
      ScriptCommand  ScriptReader::StandardCommandReader::ReadCommand()
      {
         ParameterArray params;

         // Read ID + syntax
         CommandSyntax syntax = SyntaxLibrary::Find(ReadIntNode(), Script.Game);

         // Iterate thru syntax
         for (ParameterSyntax p : syntax.Parameters)
         {
            // TODO: Handle optional parameters here

            // Read parameter
            switch (p.Type)
            {
            case ParameterType::PS_LABEL_NUMBER:   
               params.push_back( ScriptParameter(p, DataType::DT_INTEGER, ReadIntNode()) ); 
               break;

            // Decode RetVar as DT_VARIABLE
            case ParameterType::PS_VARIABLE:       
            case ParameterType::PS_RETURN_OBJECT:
            case ParameterType::PS_RETURN_OBJECT_IF:
            case ParameterType::PS_RETURN_OBJECT_IF_START:
            case ParameterType::PS_INTERRUPT_RETURN_OBJECT_IF: 
               params.push_back( ScriptParameter(p, DataType::DT_VARIABLE, ReadIntNode()) ); 
               break;
            
            // Single string node
            case ParameterType::PS_COMMENT:        
            case ParameterType::PS_SCRIPT_NAME:    
            case ParameterType::PS_LABEL_NAME:     
               params.push_back( ScriptParameter(p, DataType::DT_STRING, ReadStringNode()) );
               break;

            // Parameter as {Type,Value} pair
            default:
               params.push_back( ScriptParameter(p, ReadTypeNode(), ReadValueNode()) );
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
