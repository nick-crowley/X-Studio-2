#include "stdafx.h"
#include "CommandSyntax.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create command syntax from a declaration</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax(const Declaration& d)
         : Group(d.Group), ID(d.ID), Versions(d.Versions), URL(d.URL), Text(d.Syntax), 
           Type(IdentifyType(d.ID)), Parameters(ParseParams(d.Syntax, d.Params))
      {
      }

      CommandSyntax::~CommandSyntax()
      {
      }

      // -------------------------------- STATIC METHODS -------------------------------

      /// <summary>Identifies command type from the ID</summary>
      /// <param name="id">Command ID</param>
      /// <returns></returns>
      CommandType   CommandSyntax::IdentifyType(const UINT id)
      {
         switch (id)
         {
         case CMD_COMMENT:
         case CMD_COMMAND_COMMENT:
         case CMD_NOP:
         case CMD_END:
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_CONTINUE:
            return CommandType::Auxiliary;

         default:
            return CommandType::Standard;
         }

      }

      /// <summary>Generates a parameter lists from a legacy parameter collection</summary>
      /// <param name="syntax">The command syntax string</param>
      /// <param name="params">Parameter types in physical index order</param>
      /// <returns>Parameter list</returns>
      CommandSyntax::SyntaxArray  CommandSyntax::ParseParams(const wstring& syntax, const list<ParameterType>& params)
      {
         map<int,int>  markers;
         UINT          displayIndex = 0,
                       physicalIndex = 0;
         
         // Iterate thru '$n' syntax parameter markers
         for (auto it = find(syntax.begin(), syntax.end(), L'$'); it != syntax.end(); it = find(++it, syntax.end(), L'$'))
         {
            physicalIndex = *(it+1) - 48;             // Convert number character following '$' into a number
            markers[physicalIndex] = displayIndex++;  // Map physical index to display index
         }

         // Generate parameters
         SyntaxArray   array; //list(params.size());
         physicalIndex = 0;
         for (auto type : params)
         {
            array.push_back( ParameterSyntax(ParameterSyntax::Declaration(type, physicalIndex, markers[physicalIndex])) );
            physicalIndex++;
         }

         return array;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      
   }
}

