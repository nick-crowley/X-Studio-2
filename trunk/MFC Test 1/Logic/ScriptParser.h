#pragma once

#include "Common.h"
#include "CommandLexer.h"
#include "ScriptCommand.h"
#include "SyntaxLibrary.h"
#include <algorithm>

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

         /// <summary>Generates a parse tree from MSCI scripts</summary>
         /// <remarks>TODO: Handle the START keyword</remarks>
         class ScriptParser
         {
            // ------------------------ TYPES --------------------------
         public:
            class CommandNode;

            /// <summary>Shared pointer to a node in the script command tree</summary>
            typedef shared_ptr<CommandNode>  CommandTree;

            /// <summary>Script error</summary>
            class ErrorToken
            {
            public:
               /*ErrorToken(const ScriptToken& t) : Start(t.Start), End(t.End), Text(t.Text)
               {}*/
               ErrorToken(UINT line, UINT start, UINT end) : Line(line), Start(start), End(end)
               {}

               const UINT    Line,
                             Start, 
                             End;
               //const wstring Text;
            };

            /// <summary>Vector of error tokens</summary>
            typedef vector<ErrorToken>  ErrorArray;

            /// <summary>Simple command or assignment expression</summary>
            class CommandNode
            {
            public:
               /*CommandNode() 
                  : Logic(BranchLogic::None), LineNumber(1), Index(0), JumpTarget(nullptr), Command(SyntaxLib.Unknown, ParameterArray())
               {}*/
               CommandNode(const ScriptCommand& cmd, UINT line, const ErrorArray& err) 
                  : LineNumber(line), Index(0), JumpTarget(nullptr), Command(cmd), Errors(err)
               {
                  Logic = Command.Logic;
               }
               /*virtual ~CommandNode()
               {}*/

               BranchLogic    Logic;            // logic type
               ScriptCommand  Command;          // Command
               UINT           LineNumber,       // 1-based line number
                              Index;            // 0-based standard codearray index
               //bool           Valid;            // Successfully compiled flag
               ErrorArray     Errors;           // Compilation errors

               // JumpNode
               ScriptCommand* JumpTarget;    // Destination of jump
               
               // BranchNode
               void  Add(const CommandTree& cmd)
               {
                  Children.push_back(cmd);
               }

               bool  Contains(BranchLogic l) const
               {
                  return find_if(Children.begin(), Children.end(), [=](const CommandTree& t){ return t->Logic == l; }) != Children.end();
               }

               void  GetErrors(ErrorArray& err)
               {
                  // Copy errors
                  for (const auto& e : Errors)
                     err.push_back(e);

                  // Recurse into children
                  for (const auto& c : Children)
                     c->GetErrors(err);
               }

               void  Print(int depth = 0) const
               {
                  wstring tab(depth, (WCHAR)L' ');

                  Console.Writef(L"%03d: %s%s : ", LineNumber, tab.c_str(), GetString(Logic));
                  Console.WriteLn(Command.Syntax == SyntaxLib.Unknown ? Command.Text : Command.Syntax.Text);
                  
                  for (auto c : Children)
                     c->Print(depth+1);
               }

               vector<CommandTree>  Children;    // Child commands
            };

         public:
            class ScriptTree
            {
            public:
               void  Add(CommandTree t)   
               { 
                  Commands.push_back(t); 
               }

               ErrorArray GetErrors()
               {
                  ErrorArray err;
                  for (const CommandTree& t : Commands)
                     t->GetErrors(err);
                  return err;
               }

            private:
               vector<CommandTree> Commands;
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

            bool  MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchConditional(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchComment(const CommandLexer& lex) const;
            bool  MatchCommand(const CommandLexer& lex) const;
            bool  MatchExpression(const CommandLexer& lex) const;

            // ----------------------- MUTATORS ------------------------
         
         public:
            ScriptTree  ParseScript();

         private:
            void           ParseBranch(CommandTree& branch, LineIterator& line);
            CommandTree    ParseNode(LineIterator& line);

            TokenIterator  ReadAssignment(const CommandLexer& lex, TokenIterator& pos);
            Conditional    ReadConditional(const CommandLexer& lex, TokenIterator& pos);
            TokenIterator  ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos);

            ScriptCommand  ReadComment(const CommandLexer& lex, const LineIterator& line);
            CommandTree  ReadCommand(const CommandLexer& lex, const LineIterator& line);
            ScriptCommand  ReadExpression(const CommandLexer& lex, const LineIterator& line);

            // -------------------- REPRESENTATION ---------------------

         private:
            const LineArray&  Input;
            GameVersion       Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
