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
               ErrorToken(const wstring& msg, UINT line, const ScriptToken& t) : TokenBase(t), Error(msg), Line(line)
               {}
               ErrorToken(const wstring& msg, UINT line, UINT start, UINT end) : TokenBase(start,end), Error(msg), Line(line)
               {}

               const wstring Error;
               const UINT    Line;
            };

            /// <summary>Vector of error tokens</summary>
            class ErrorArray : public vector<ErrorToken> 
            {
            public:
               ErrorArray()
               {}
               ErrorArray(const ErrorToken& e) {
                  push_back(e);
               }

               ErrorArray& operator+=(const ErrorToken& t)
               {
                  push_back(t);
                  return *this;
               }
            };

            /// <summary>Represents a script command and its descendants, if any</summary>
            class CommandNode
            {
            public:
               CommandNode(const ScriptCommand& cmd, UINT line);
               ~CommandNode();

               BranchLogic          Logic;            // logic type
               ScriptCommand        Command;          // Command
               UINT                 LineNumber,       // 1-based line number
                                    Index;            // 0-based standard codearray index
               CommandNode*         JumpTarget;       // Destination of jump
               vector<CommandTree>  Children;         // Child commands
               
               /// <summary>Add child node</summary>
               /// <param name="cmd">The command node</param>
               void  Add(const CommandTree& cmd);

               /// <summary>Check children for presence of certain branch logic</summary>
               /// <param name="l">logic</param>
               /// <returns></returns>
               bool  Contains(BranchLogic l) const;


               void  Verify(ErrorArray& err);

               /// <summary>Debug print</summary>
               /// <param name="depth">The depth.</param>
               void  Print(int depth = 0) const;
            };

         public:
            /// <summary>Tree of command nodes</summary>
            class ScriptTree
            {
            public:
               ScriptTree();
               ~ScriptTree();

               /// <summary>Add a command to the tree</summary>
               /// <param name="t">Command</param>
               void  Add(CommandTree t);

               /// <summary>Verify the parsed script</summary>
               void  Verify(ErrorArray&);

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
         private:
            ErrorToken  MakeError(const wstring& msg, const LineIterator& line, const CommandLexer& lex) const;
            ErrorToken  MakeError(const wstring& msg, const LineIterator& line, const TokenIterator& tok) const;

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

         private:
            UINT  GetLineNumber(const LineIterator& line) const;

            bool  MatchArrayIndex(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchConditional(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchComment(const CommandLexer& lex) const;
            bool  MatchCommand(const CommandLexer& lex) const;
            bool  MatchExpression(const CommandLexer& lex) const;

            // ----------------------- MUTATORS ------------------------
         private:
            void           Parse();
            void           ParseBranch(CommandTree& branch, LineIterator& line);
            CommandTree    ParseNode(LineIterator& line);

            TokenIterator  ReadAssignment(const CommandLexer& lex, TokenIterator& pos);
            Conditional    ReadConditional(const CommandLexer& lex, TokenIterator& pos);
            TokenIterator  ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos);

            CommandNode*   ReadComment(const CommandLexer& lex, const LineIterator& line);
            CommandNode*   ReadCommand(const CommandLexer& lex, const LineIterator& line);
            CommandNode*   ReadExpression(const CommandLexer& lex, const LineIterator& line);

            // -------------------- REPRESENTATION ---------------------

         public:
            ErrorArray     Errors;     // Compilation errors
            ScriptTree     Script;     // Compiled script tree

         private:
            const LineArray&  Input;
            const GameVersion Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
