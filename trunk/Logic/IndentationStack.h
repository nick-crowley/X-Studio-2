#pragma once

#include "ScriptCommand.h"
#include "CommandNode.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Calculates code indentation</summary>
      class LogicExport IndentationStack : protected deque<BranchLogic>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         IndentationStack() : WithinSub(false)
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

      protected:
         /// <summary>Get item count plus subroutine indentation (if any).</summary>
         /// <returns></returns>
         size_type  size() const
         {
            return __super::size() + (WithinSub ? 1 : 0);
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Call before displaying command</summary>
         /// <param name="cmd">command.</param>
         void PreDisplay(const ScriptCommand& cmd)
         {
            PreDisplay(cmd.Syntax.ID, cmd.Logic);
         }

         /// <summary>Call before displaying command</summary>
         /// <param name="cmd">command node.</param>
         void PreDisplay(const CommandNodePtr& cmd)
         {
            PreDisplay(cmd->Syntax.ID, cmd->Logic);
         }
         
         /// <summary>Call after displaying command</summary>
         /// <param name="cmd">command.</param>
         /// <param name="subroutine">whether command defines a subroutine.</param>
         void PostDisplay(const ScriptCommand& cmd, bool subroutine)
         {
            PostDisplay(cmd.Syntax.ID, cmd.Logic, cmd.Syntax.Type, cmd.Commented, subroutine);
         }

         /// <summary>Call after displaying command</summary>
         /// <param name="cmd">command.</param>
         /// <param name="subroutine">whether command defines a subroutine.</param>
         void PostDisplay(const CommandNodePtr& cmd, bool subroutine)
         {
            PostDisplay(cmd->Syntax.ID, cmd->Logic, cmd->Syntax.Type, cmd->CmdComment, subroutine);
         }

      protected:
         /// <summary>Called before displaying command</summary>
         /// <param name="id">id.</param>
         /// <param name="logic">logic.</param>
         void PreDisplay(UINT id, BranchLogic logic)
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
            
            // EndSub: 
            if (empty() && id == CMD_END_SUB)
               WithinSub = false;
         }

         /// <summary>Called after displaying command.</summary>
         /// <param name="id">command id.</param>
         /// <param name="logic">branching logic.</param>
         /// <param name="type">command type.</param>
         /// <param name="commented">whether a commented command [false for comments]</param>
         /// <param name="subroutine">Whether label defintiion is a subroutine defintiion.</param>
         void PostDisplay(UINT id, BranchLogic logic, CommandType type, bool commented, bool subroutine)
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

            // Subroutine
            if (subroutine)
               WithinSub = true;

            // Pop 'SkipIf' after next uncommented standard command (or break/continue)
            if (!empty() && back() == BranchLogic::SkipIf)
               if (!commented && (type == CommandType::Standard || id == CMD_BREAK || id == CMD_CONTINUE))
                  pop_back();
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         bool  WithinSub;
      };

   }
}

using namespace Logic::Scripts;
