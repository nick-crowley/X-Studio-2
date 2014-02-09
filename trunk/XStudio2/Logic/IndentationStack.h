#pragma once

#include "Common.h"
#include "ScriptCommand.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Calculates code indentation</summary>
      class IndentationStack : deque<BranchLogic>
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
         PROPERTY_GET(int,Size,GetSize);

         // ---------------------- ACCESSORS ------------------------			
      public:
         int  GetSize() const
         {
            return size();
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Called before displaying command</summary>
         /// <param name="cmd">The command.</param>
         void PreDisplay(const ScriptCommand& cmd)
         {
            // Indentation calculations
            switch (cmd.Logic)
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
         void PostDisplay(const ScriptCommand& cmd)
         {
            // Indentation calculations
            switch (cmd.Logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               push_back(cmd.Logic);
               break;

            case BranchLogic::SkipIf:
               push_back(cmd.Logic); 
               return;
            }

            // Pop 'SkipIf' after next standard command (or break/continue)
            if (!empty() && back() == BranchLogic::SkipIf && !cmd.Commented && (cmd.Is(CommandType::Standard) || cmd.Is(CMD_BREAK) || cmd.Is(CMD_CONTINUE)))
               pop_back();
         }

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::Scripts;
