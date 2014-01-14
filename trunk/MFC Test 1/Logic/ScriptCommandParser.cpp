#include "stdafx.h"
#include "ScriptCommandParser.h"
#include "ScriptExpressionParser.h"
#include "SyntaxLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptCommandParser::ScriptCommandParser(const wstring& line, GameVersion  v) : Lexer(line), Version(v)
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

         ScriptCommand  ScriptCommandParser::GenerateCommand(UINT id, Conditional c, TokenArray& params)
         {
            throw NotImplementedException();
         }
         ScriptCommand  ScriptCommandParser::GenerateCommand(UINT id, const wstring& retVar, TokenArray& params)
         {
            throw NotImplementedException();
         }
         ScriptCommand  ScriptCommandParser::GenerateCommand(UINT id, TokenArray& params)
         {
            throw NotImplementedException();
         }

         bool  ScriptCommandParser::Match(const TokenIterator& pos, TokenType  type)
         {
            return pos < Lexer.Tokens.end() && pos->Type == type;
         }

         bool  ScriptCommandParser::Match(const TokenIterator& pos, TokenType  type, const TCHAR* txt)
         {
            return Match(pos, type) && pos->Text == txt;
         }

         ScriptCommand  ScriptCommandParser::MatchLine()
         {
            TokenIterator  pos = Lexer.Tokens.begin();
            TokenArray     params;
            wstring        retVar;
            Conditional    condition;
            UINT           id;

            // NOP:
            if (MatchNOP(pos))
               return GenerateCommand(CMD_NOP, params);

            // Match: Command/Expr with assignment
            else if (MatchAssignment(pos, retVar))
            {
               if (MatchCommand(pos, id, params) || MatchExpression(pos, params))
                  return GenerateCommand(id, retVar, params);

               throw error;
            }
            // Match: Command/Expr with conditional
            else if (MatchConditional(pos, condition))
            {
               if (MatchCommand(pos, id, params) || MatchExpression(pos, params))
                  return GenerateCommand(id, condition, params);

               throw error;
            }
            // Match: Command without assign/conditional
            else if (MatchCommand(pos, id, params))
               return GenerateCommand(id, params);

            // Unrecognised
            throw error;
         }

         

         bool ScriptCommandParser::MatchAssignment(TokenIterator& pos, wstring& var)
         {
            // var =
            return Match(pos, TokenType::Variable) && Match(pos+1, TokenType::Operator, L"=") ? (var=pos->Text, ++pos, true) : false;
         }

         bool ScriptCommandParser::MatchConditional(TokenIterator& pos, Conditional& c)
         {
            // if, if not
            if (Match(pos, TokenType::Keyword, L"if")) 
               return Match(pos+1, TokenType::Keyword, L"not") ? (++pos, c=Conditional::IF, true) : (c=Conditional::IF_NOT, true);

            // while, while not
            if (Match(pos, TokenType::Keyword, L"while")) 
               return Match(pos+1, TokenType::Keyword, L"not") ? (++pos, c=Conditional::WHILE, true) : (c=Conditional::WHILE_NOT, true);

            // skip if
            if (Match(pos, TokenType::Keyword, L"skip")) 
               return Match(pos+1, TokenType::Keyword, L"if") ? (++pos, c=Conditional::SKIP_IF, true) : false;

            // do if
            if (Match(pos, TokenType::Keyword, L"do")) 
               return Match(pos+1, TokenType::Keyword, L"if") ? (++pos, c=Conditional::SKIP_IF_NOT, true) : false;

            // failed
            return false;
         }

         bool ScriptCommandParser::MatchNOP(TokenIterator& pos)
         {
            // Match zero tokens
            return pos == Lexer.Tokens.begin() && pos == Lexer.Tokens.end();
         }

         bool ScriptCommandParser::MatchCommand(TokenIterator& pos, UINT& id, TokenArray& params)
         {
            // Clear params
            params.clear();
            id = CMD_NONE;

            // Hash remaining tokens
            CommandHash hash(pos, Lexer.Tokens.end());
            
            // Lookup hash, copy parameters and consume all tokens
            return id = SyntaxLib.Identify(hash.Hash, Version) ? (pos=Lexer.Tokens.end(), params=hash.Parameters, true) : false;
         }

         bool ScriptCommandParser::MatchExpression(TokenIterator& pos, TokenArray& params)
         {
            return ScriptExpressionParser(pos, Lexer.Tokens.end(), params).Parse();
         }

         
      }
   }
}
