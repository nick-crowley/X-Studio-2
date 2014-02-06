#pragma once

#include "Common.h"
#include "CommandNode.h"
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
            
            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptParser(ScriptFile& file, const LineArray& lines, GameVersion  v);
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
            CommandNodePtr Advance();
            void           ParseRoot();
            void           ParseIf(CommandNodePtr& If);
            void           ParseElse(CommandNodePtr& Else);
            void           ParseSkipIf(CommandNodePtr& SkipIf);
            
            TokenIterator  ReadAssignment(const CommandLexer& lex, TokenIterator& pos);
            Conditional    ReadConditional(const CommandLexer& lex, TokenIterator& pos);
            TokenIterator  ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos);

            CommandNodePtr   ReadComment(const CommandLexer& lex);
            CommandNodePtr   ReadCommand(const CommandLexer& lex);
            CommandNodePtr   ReadExpression(const CommandLexer& lex);
            CommandNodePtr   ReadLine();

            // -------------------- REPRESENTATION ---------------------

         public:
            ErrorArray     Errors;     // Compilation errors
            ScriptFile&    Script;     // Script

         private:
            const LineArray&  Input;
            const GameVersion Version;

            CommandNodePtr  Root;       // Parse tree
            LineIterator    CurrentLine;
            CommandNodePtr  CurrentNode;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
