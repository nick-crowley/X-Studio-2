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
               ErrorToken(const wstring& msg, UINT line, const CHARRANGE& r) : TokenBase(r.cpMin, r.cpMax), Error(msg), Line(line)
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
               // ------------------------ TYPES --------------------------
            public:
               typedef vector<CommandTree>::const_iterator  NodeIterator;

               // --------------------- CONSTRUCTION ----------------------
            public:
               CommandNode();
               CommandNode(const CommandTree& parent, const ScriptCommand& cmd, const CommandLexer& lex, UINT line);
               ~CommandNode();

               // ------------------------ STATIC -------------------------

               // --------------------- PROPERTIES ------------------------

               // ---------------------- ACCESSORS ------------------------		
            public:
               bool  Contains(BranchLogic l) const;
               NodeIterator  Find(BranchLogic l) const;
               bool  IsRoot() const;
               void  Print(int depth = 0) const;
               void  Verify(ErrorArray& err) const;

            protected:
               void  VerifyLogic(ErrorArray& err) const;

               // ----------------------- MUTATORS ------------------------
            public:
               CommandTree  Add(CommandNode* node);

               // -------------------- REPRESENTATION ---------------------

               const CommandNode*   Parent;           // Parent node
               BranchLogic          Logic;            // logic type
               ScriptCommand        Command;          // Command
               UINT                 LineNumber,       // 1-based line number
                                    Index;            // 0-based standard codearray index
               CHARRANGE            LineText;         // Start/end character offsets
               CommandNode*         JumpTarget;       // Destination of jump
               vector<CommandTree>  Children;         // Child commands
            };

            /// <summary>Legacy typedef</summary>
            typedef CommandNode  ScriptTree;

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
            CommandNode*   ParseNode(const CommandTree& parent, LineIterator& line);

            TokenIterator  ReadAssignment(const CommandLexer& lex, TokenIterator& pos);
            Conditional    ReadConditional(const CommandLexer& lex, TokenIterator& pos);
            TokenIterator  ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos);

            CommandNode*   ReadComment(const CommandTree& parent, const CommandLexer& lex, const LineIterator& line);
            CommandNode*   ReadCommand(const CommandTree& parent, const CommandLexer& lex, const LineIterator& line);
            CommandNode*   ReadExpression(const CommandTree& parent, const CommandLexer& lex, const LineIterator& line);

            // -------------------- REPRESENTATION ---------------------

         public:
            ErrorArray    Errors;     // Compilation errors
            CommandTree   Script;     // Compiled script tree

         private:
            const LineArray&  Input;
            const GameVersion Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
