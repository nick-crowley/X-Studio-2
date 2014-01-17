#pragma once

#include "Common.h"
#include "ScriptCommandLexer.h"
#include "ScriptCommand.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         typedef vector<wstring>  LineArray;

         /// <summary>Constant line iterator</summary>
         typedef LineArray::const_iterator LineIterator;

         enum class Operator : UINT { Equals, NotEquals, Multiply, Divide, Add, Subtract };

         /// <summary></summary>
         class ScriptParser
         {
            // ------------------------ TYPES --------------------------
         private:
            /// <summary>Simple command or assignment expression</summary>
            class CommandNode
            {
            public:
               CommandNode(const ScriptCommand& cmd) : Command(cmd)
               {}
               virtual ~CommandNode()
               {}

               BranchLogic    Logic;            // logic type
               ScriptCommand  Command;          // Command
               UINT           LineNumber,       // 1-based line number
                              Index;            // 0-based standard codearray index
               bool           Valid;            // Successfully compiled flag
            };

            /// <summary>Shared pointer to a node in the script command tree</summary>
            typedef shared_ptr<CommandNode>  CommandTree;


            /// <summary>Any command that executes a jump: break/continue/hiddenJump + any conditional</summary>
            class JumpNode : public CommandNode
            {
            public:
               ScriptCommand* JumpTarget;    // Destination of jump
            };


            /// <summary>Conditional command: if/skip/else/elseif/while</summary>
            class BranchNode : public JumpNode
            {
            public:
               BranchNode(ScriptCommand cmd) : CommandNode(cmd)
               {}

               void  Add(const CommandTree& cmd)
               {
                  Children.push_back(cmd);
               }

               bool  Contains(BranchLogic l) const
               {
                  return find_if(Children.begin(), Children.end(), [=](const CommandTree& t){ t->Logic == l; }) != Children.end();
               }

               vector<CommandTree>  Children;    // Child commands
            };

            

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptParser(const LineArray& lines, GameVersion  v);
            virtual ~ScriptParser();

            DEFAULT_COPY(ScriptParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptParser);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

         private:
            UINT  GetLineNumber(const LineIterator& line) const;
            BranchLogic  PeekCommand(const LineIterator& line) const;

            // ----------------------- MUTATORS ------------------------
         
         public:
            CommandTree  ReadScript();

         private:
            CommandTree  ReadBranch(LineIterator& line, BranchLogic logic);
            CommandTree  ReadCommand(LineIterator& line, BranchLogic logic);

            // -------------------- REPRESENTATION ---------------------

         private:
            const LineArray&  Input;
            GameVersion       Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
