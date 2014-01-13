#include "stdafx.h"
#include "ScriptCommandParser.h"
#include "SyntaxLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptCommandParser::ScriptCommandParser(const wstring& line) : Lexer(line)
         {
         }


         ScriptCommandParser::~ScriptCommandParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         ScriptCommand  ScriptCommandParser::Parse()
         {
            return MatchLine();
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------

         ScriptCommand  ScriptCommandParser::MatchLine()
         {
            MatchCollection  matches;
            auto pos = Lexer.Tokens.begin();

            // Match alternatives
            MatchAssignmentCommand(pos, matches);
            MatchAssignmentExpression(pos, matches);

            // Ensure match found
            if (matches.empty())
               throw NoMatchException();

            // Ensure nothing unexpected following match
            if (matches.front().size() < Lexer.Tokens.size())
               throw UnexpectedTokenException();

            // Create command from best match
            return GenerateCommand(matches.front());
         }

         void  ScriptCommandParser::MatchConditional(TokenIterator pos, MatchCollection m)
         {
            if (pos->Text == L"skip" && (pos+1)->Text == L"if" && (pos+2)->Text == "not")
               m.push_back(ConditionalRule(Conditional::SKIP_IF_NOT, pos, pos+3));

            if (pos->Text == L"if" && (pos+1)->Text == L"not")
               m.push_back(ConditionalRule(Conditional::IF_NOT, pos, pos+1));
         }

         void  ScriptCommandParser::MatchCommand(TokenIterator pos, MatchCollection m)
         {
            CommandHash h(pos, Lexer.Tokens.end());
            
            if (SyntaxLib.Contains(h))
               m.push_back( CommandRule(SyntaxLib.Find(h), pos, Lexer.Tokens.end()) );
         }
         

         ScriptCommand  ScriptCommandParser::GenerateCommand(Match m)
         {
         }
      }
   }
}
