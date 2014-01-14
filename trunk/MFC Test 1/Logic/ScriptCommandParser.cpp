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
            //MatchCollection  matches;
            ParameterArray params;
            auto pos = Lexer.Tokens.begin();

            // NOP:
            if (MatchNOP(pos))
               return ScriptCommand(SyntaxLib.Find(CMD_NOP, GameVersion), params);


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

         bool ScriptCommandParser::MatchAssignment(TokenIterator& pos)
         {
            // var =
            return Match(pos, TokenType::Variable) && Match(pos+1, TokenType::Operator, L"=") ? (++pos, true) : false;
         }

         bool ScriptCommandParser::MatchCondition(TokenIterator& pos)
         {
            // if, if not
            if (Match(pos, TokenType::Keyword, L"if")) 
               return Match(pos+1, TokenType::Keyword, L"not") ? (++pos, true) : true;

            // while, while not
            if (Match(pos, TokenType::Keyword, L"while")) 
               return Match(pos+1, TokenType::Keyword, L"not") ? (++pos, true) : true;

            // skip if
            if (Match(pos, TokenType::Keyword, L"skip")) 
               return Match(pos+1, TokenType::Keyword, L"if") ? (++pos, true) : false;

            // do if
            if (Match(pos, TokenType::Keyword, L"do")) 
               return Match(pos+1, TokenType::Keyword, L"if") ? (++pos, true) : false;

            // failed
            return false;
         }

         bool ScriptCommandParser::MatchNOP(TokenIterator& pos)
         {
            // episilon
            return pos == Lexer.Tokens.begin() && pos == Lexer.Tokens.end();
         }

         bool ScriptCommandParser::MatchCommand(TokenIterator& pos)
         {
            // Hash remaining tokens
            CommandHash h(pos, Lexer.Tokens.end());
            
            // Lookup hash, consume all tokens
            return SyntaxLib.Contains(h.Hash) ? (pos=Lexer.Tokens.end(), true) : false;
         }

         bool ScriptCommandParser::MatchExpression(TokenIterator& pos)
         {
            TokenIterator origin = pos;

            // Unary_op expr
            if (Match(pos, TokenType::Operator, L"!") || Match(pos, TokenType::Operator, L"~") || Match(pos, TokenType::Operator, L"-")) 
               return MatchExpression(++pos) ? true : (pos=origin, false);

            // ( expr )
            if (Match(pos, TokenType::Operator, L"("))
               return MatchExpression(++pos) && Match(pos, TokenType::Operator, L")") ? (++pos, true) : (pos=origin, false);

            // expr binary_op expr
            if (MatchExpression(pos) && Match(++pos, TokenType::Operator, L"*")
               return MatchExpression(++pos) && Match(++pos, TokenType::Operator, L")") ? true : (pos=origin, false);
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
            
            // Lookup hash
            if (SyntaxLib.Contains(h.Hash))
               m.push_back( CommandRule(SyntaxLib.Find(h), pos, Lexer.Tokens.end()) );
         }
         

         ScriptCommand  ScriptCommandParser::GenerateCommand(Match m)
         {
         }
      }
   }
}
