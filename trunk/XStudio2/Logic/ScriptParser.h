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
         typedef LineArray::const_iterator  LineIterator;

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
               /// <summary>Create error for token</summary>
               ErrorToken(const wstring& msg, UINT line, const ScriptToken& t) : TokenBase(t), Error(msg), Line(line)
               {}
               /// <summary>Create error for entire line</summary>
               ErrorToken(const wstring& msg, UINT line, const CHARRANGE& r) : TokenBase(r.cpMin, r.cpMax), Error(msg), Line(line)
               {}
               /// <summary>Create error for character range</summary>
               ErrorToken(const wstring& msg, UINT line, UINT start, UINT end) : TokenBase(start,end), Error(msg), Line(line)
               {}

               const wstring Error;
               const UINT    Line;
            };

            /// <summary>Vector of error tokens</summary>
            class ErrorArray : public vector<ErrorToken> 
            {
            public:
               /// <summary>Create empty array</summary>
               ErrorArray()
               {}
               /// <summary>Create array from single token</summary>
               ErrorArray(const ErrorToken& e) {
                  push_back(e);
               }

               /// <summary>Add an error to the array</summary>
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
               typedef vector<CommandTree>        NodeArray;
               typedef NodeArray::const_iterator  NodeIterator;

               // --------------------- CONSTRUCTION ----------------------
            public:
               CommandNode();
               //CommandNode(const ScriptCommand& cmd, const CommandLexer& lex, UINT line);
               CommandNode(const CommandLexer& lex, UINT line);
               ~CommandNode();

               // ------------------------ STATIC -------------------------

               // --------------------- PROPERTIES ------------------------

               // ---------------------- ACCESSORS ------------------------		
            public:
               bool  Contains(BranchLogic l) const;
               NodeIterator  Find(BranchLogic l) const;
               bool  IsRoot() const;
               void  Print(int depth = 0) const;
               void  Verify(ErrorArray& err);

            protected:
               void  VerifyNode(ErrorArray& err);
               void  VerifyLogic(ErrorArray& err) const;
               void  VerifyParameters(ErrorArray& err);
               
               // ----------------------- MUTATORS ------------------------
            public:
               CommandTree  Add(CommandTree node);
               void Assemble(ErrorArray& err);

               // -------------------- REPRESENTATION ---------------------

               CommandNode*   Parent;           // Parent node
               NodeArray      Children;         // Child commands

               BranchLogic    Logic;            // branching logic
               ParameterArray Output;           // script parameters in physical order
               const CommandSyntax& Syntax;
               CommandNode*   JumpTarget;       // Destination of jump
               UINT           Index;            // 0-based standard codearray index

               CommandLexer   Lexer;
               TokenArray     Arguments,
                              Parameters,
                              Postfix;
               TokenIterator  RetVar,
                              RefObj;
               Conditional    Condition;
               const UINT     LineNumber;       // 1-based line number
               const CHARRANGE Extent;          // Start/end character offsets
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
         
            // --------------------- PROPERTIES ------------------------
         private:
            PROPERTY_GET(UINT,LineNumber,GetLineNumber);

            // ---------------------- ACCESSORS ------------------------			
         private:
            UINT        GetLineNumber() const;
            ErrorToken  MakeError(const wstring& msg, const CommandLexer& lex) const;
            ErrorToken  MakeError(const wstring& msg, const TokenIterator& tok) const;

            bool  MatchArrayIndex(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchConditional(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchScriptArgument(const CommandLexer& lex, TokenIterator& pos) const;
            bool  MatchComment(const CommandLexer& lex) const;
            bool  MatchCommand(const CommandLexer& lex) const;
            bool  MatchExpression(const CommandLexer& lex) const;

            // ----------------------- MUTATORS ------------------------
         private:
            CommandTree    Advance();
            void           ParseRoot();
            void           ParseIf(CommandTree& If);
            void           ParseElse(CommandTree& Else);
            void           ParseSkipIf(CommandTree& SkipIf);
            
            TokenIterator  ReadAssignment(const CommandLexer& lex, TokenIterator& pos);
            Conditional    ReadConditional(const CommandLexer& lex, TokenIterator& pos);
            TokenIterator  ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos);

            CommandTree   ReadComment(const CommandLexer& lex);
            CommandTree   ReadCommand(const CommandLexer& lex);
            CommandTree   ReadExpression(const CommandLexer& lex);
            CommandTree   ReadLine();

            // -------------------- REPRESENTATION ---------------------

         public:
            ErrorArray    Errors;     // Compilation errors
            CommandTree   Script;     // Script parse tree

         private:
            const LineArray&  Input;
            const GameVersion Version;

            LineIterator  CurrentLine;
            CommandTree   CurrentNode;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
