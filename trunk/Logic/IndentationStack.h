#pragma once

#include "ScriptCommand.h"
#include "CommandNode.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Calculates code indentation</summary>
      class IndentationStack : protected deque<BranchLogic>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         IndentationStack()
         {}

         DEFAULT_COPY(IndentationStack);	// Default copy semantics
         DEFAULT_MOVE(IndentationStack);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(Indent,Indentation,GetIndentation);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets the current indentation, formatted according to script indentation preference</summary>
         /// <returns></returns>
         Indent GetIndentation() const
         {
            return Indent(PrefsLib.ScriptIndentation * size());
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Call before displaying command</summary>
         /// <param name="cmd">command.</param>
         void PreDisplay(const ScriptCommand& cmd)
         {
            PreDisplay(cmd.Logic);
         }

         /// <summary>Call before displaying command</summary>
         /// <param name="cmd">command node.</param>
         void PreDisplay(const CommandNodePtr& cmd)
         {
            PreDisplay(cmd->Logic);
         }
         
         /// <summary>Call after displaying command</summary>
         /// <param name="cmd">command.</param>
         void PostDisplay(const ScriptCommand& cmd)
         {
            PostDisplay(cmd.Syntax.ID, cmd.Logic, cmd.Syntax.Type, cmd.Commented);
         }

         /// <summary>Call after displaying command</summary>
         /// <param name="cmd">command.</param>
         void PostDisplay(const CommandNodePtr& cmd)
         {
            PostDisplay(cmd->Syntax.ID, cmd->Logic, cmd->Syntax.Type, cmd->CmdComment);
         }

      protected:
         /// <summary>Called before displaying command</summary>
         /// <param name="logic">logic.</param>
         void PreDisplay(BranchLogic logic)
         {
            // Indentation calculations
            switch (logic)
            {
            case BranchLogic::ElseIf:
            case BranchLogic::Else:    
            case BranchLogic::End:      
               if (!empty())    // shouldn't be empty, but just in case
                  pop_back();       
               break;
            }
         }


         /// <summary>Called after displaying command</summary>
         /// <param name="cmd">The command.</param>
         void PostDisplay(UINT id, BranchLogic logic, CommandType type, bool commented)
         {
            // Indentation calculations
            switch (logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               push_back(logic);
               break;

            case BranchLogic::SkipIf:
               push_back(logic); 
               return;
            }

            // Pop 'SkipIf' after next standard command (or break/continue)
            if (!empty() && !commented 
                && back() == BranchLogic::SkipIf 
                && (type == CommandType::Standard || id == CMD_BREAK || id == CMD_CONTINUE))
               pop_back();
         }


         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::Scripts;
