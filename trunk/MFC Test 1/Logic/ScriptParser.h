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
         /// <summary>Vector of strings</summary>
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
            class ErrorToken : public TokenBase
            {
            public:
               /*ErrorToken(const ScriptToken& t) : Start(t.Start), End(t.End), Text(t.Text)
               {}*/
               ErrorToken(UINT line, UINT start, UINT end) : TokenBase(start,end), Line(line)
               {}

               const UINT  Line;
            };

            /// <summary>Vector of error tokens</summary>
            typedef vector<ErrorToken>  ErrorArray;

            /// <summary>Represents a script command and its descendants, if any</summary>
            class CommandNode
            {
            public:
               CommandNode(const ScriptCommand& cmd, UINT line, const ErrorArray& err) 
                  : LineNumber(line), Index(0), JumpTarget(nullptr), Command(cmd), Errors(err), Logic(cmd.Logic)
               {}

               BranchLogic    Logic;            // logic type
               ScriptCommand  Command;          // Command
               UINT           LineNumber,       // 1-based line number
                              Index;            // 0-based standard codearray index
               ErrorArray     Errors;           // Compilation errors
               ScriptCommand* JumpTarget;       // Destination of jump
               vector<CommandTree>  Children;    // Child commands
               
               /// <summary>Add child node</summary>
               /// <param name="cmd">The command node</param>
               void  Add(const CommandTree& cmd)
               {
                  Children.push_back(cmd);
               }

               /// <summary>Check children for presence of certain branch logic</summary>
               /// <param name="l">logic</param>
               /// <returns></returns>
               bool  Contains(BranchLogic l) const
               {
                  return find_if(Children.begin(), Children.end(), [=](const CommandTree& t){ return t->Logic == l; }) != Children.end();
               }

               /// <summary>Get errors collection</summary>
               /// <param name="err">collection to populate</param>
               void  GetErrors(ErrorArray& err)
               {
                  // Copy errors
                  for (const auto& e : Errors)
                     err.push_back(e);

                  // Recurse into children
                  for (const auto& c : Children)
                     c->GetErrors(err);
               }

               /// <summary>Debug print</summary>
               /// <param name="depth">The depth.</param>
               void  Print(int depth = 0) const
               {
                  wstring tab(depth, (WCHAR)L' ');

                  Console.Writef(L"%03d: %s%s : ", LineNumber, tab.c_str(), GetString(Logic));
                  Console.WriteLn(Command.Syntax == SyntaxLib.Unknown ? Command.Text : Command.Syntax.Text);
                  
                  for (auto c : Children)
                     c->Print(depth+1);
               }
            };

         public:
            /// <summary>Tree of command nodes</summary>
            class ScriptTree
            {
            public:
               /// <summary>Add a command to the tree</summary>
               /// <param name="t">command</param>
               void  Add(CommandTree t)
               { 
                  Commands.push_back(t); 
               }

               /// <summary>Gets the errors collection</summary>
               /// <returns></returns>
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
            CommandTree    ReadCommand(const CommandLexer& lex, const LineIterator& line);
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
