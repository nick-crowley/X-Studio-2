#include "stdafx.h"
#include "ScriptCommand.h"
#include "ScriptFile.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a standard command from syntax and parameters (ordered by display index)</summary>
      /// <param name="syntax">The syntax.</param>
      /// <param name="params">The parameters, if any (sorted by display index)</param>
      ScriptCommand::ScriptCommand(CommandSyntax& syntax, ParameterArray& params)
         : Syntax(syntax), RefIndex(0), Parameters(params) 
      {
      }

      /// <summary>Create an auxiliary command from syntax and parameters (ordered by display index)</summary>
      /// <param name="syntax">The command syntax.</param>
      /// <param name="ref">The index of the associated standard command</param>
      /// <param name="params">The parameters, if any (sorted by display index)</param>
      ScriptCommand::ScriptCommand(CommandSyntax& syntax, UINT  ref, ParameterArray& params)
         : Syntax(syntax), RefIndex(ref), Parameters(params) 
      {
      }
      
      ScriptCommand::~ScriptCommand()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Identify the type of branching conditional used (if any)</summary>
      /// <returns></returns>
      BranchLogic  ScriptCommand::GetConditional() const
      {
         switch (Syntax.ID)
         {
         case CMD_END:      return BranchLogic::End;
         case CMD_ELSE:     return BranchLogic::Else;
         case CMD_BREAK:    return BranchLogic::Break;
         case CMD_CONTINUE: return BranchLogic::Continue;

         case CMD_COMMAND_COMMENT:
         case CMD_COMMENT: 
         case CMD_NOP:      return BranchLogic::NOP;
         
         default:
            // Find Return parameter, if any
            auto it = find_if(Parameters.begin(), Parameters.end(), [](const ScriptParameter& p) { return p.Syntax.IsRetVar(); });
            
            // RetVar/Discard
            if (it == Parameters.end() || it->Value.Type == ValueType::String)
               return BranchLogic::None;

            // RetVar/Discard
            ReturnValue retVar(it->Value.Int);
            if (retVar.ReturnType == ReturnType::ASSIGNMENT || retVar.ReturnType == ReturnType::DISCARD)
               return BranchLogic::None;
               
            // Conditional
            switch (retVar.Conditional)
            {
            case Conditional::IF:      
            case Conditional::IF_NOT:        return BranchLogic::If;
            case Conditional::WHILE:         
            case Conditional::WHILE_NOT:     return BranchLogic::While;

            case Conditional::ELSE_IF:  
            case Conditional::ELSE_IF_NOT:   return BranchLogic::ElseIf;

            case Conditional::SKIP_IF:  
            case Conditional::SKIP_IF_NOT:   return BranchLogic::SkipIf;

            default:
               return BranchLogic::None;
            }
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

