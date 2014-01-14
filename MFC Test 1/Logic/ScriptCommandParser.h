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
         typedef TokenArray::const_iterator  TokenIterator;

         /// <summary></summary>
         class ScriptCommandParser
         {
            enum class Operator : UINT { Equals };

            class CommandHash 
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               CommandHash(TokenIterator b, TokenIterator e) : Hash(GenerateHash(b,e)), Parameters(SeparateParams(b,e))
               {}

               // ------------------------ STATIC -------------------------
            private:
               static wstring  GenerateHash(TokenIterator begin, TokenIterator end) 
               {
                  wstring h;
                  // Assemble text into hash
                  for (TokenIterator it = begin; it != end; ++it)
                     if (it->Type == TokenType::Text)
                        h += it->Text;
                  return h;
               }

               static TokenArray  SeparateParams(TokenIterator begin, TokenIterator end) 
               {
                  TokenArray arr;
                  // Separate tokens
                  for (TokenIterator it = begin; it != end; ++it)
                     if (it->Type != TokenType::Text)
                        arr.push_back(*it);
                  return arr;
               }

               // ---------------------- ACCESSORS ------------------------	

               bool  operator==(const CommandHash& r) { return Hash == r.Hash; }
               bool  operator!=(const CommandHash& r) { return Hash != r.Hash; }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------
            public:
               const wstring     Hash;
               const TokenArray  Parameters;
            };

            //class ExpressionRule : public CommandRule
            //{
            //public:
            //   ExpressionRule(TokenIterator b, TokenIterator e) : CommandRule(SyntaxLib.Find(CMD_EXPRESSION), b,e)
            //   {}

            //   // ---------------------- ACCESSORS ------------------------			

            //   ScriptParameter  GetParameter(ParameterSyntax s) const
            //   {
            //      throw InvalidOperationException();
            //   }

            //   virtual list<ScriptParameter>  GetParameters() const PURE;

            //   // ----------------------- MUTATORS ------------------------

            //   // -------------------- REPRESENTATION ---------------------
            //};

            //class BinaryExpressionRule : public ExpressionRule
            //{
            //public:
            //   BinaryExpressionRule(ExpressionRule* l, Operator o, ExpressionRule* r, TokenIterator b, TokenIterator e) 
            //      : ExpressionRule(b,e), Left(l), Right(r), Operator(o)
            //   {}
            //   ~BinaryExpressionRule() 
            //   {
            //      delete Left;
            //      delete Right;
            //   }

            //   // ---------------------- ACCESSORS ------------------------			

            //   list<ScriptParameter>  GetParameters() const
            //   {
            //      return Left->GetParameters() 
            //          + ScriptParameter(ParameterType::EXPRESSION, DataType::OPERATOR, (UINT)Operator)
            //          + Right->GetParameters();
            //   }

            //   // ----------------------- MUTATORS ------------------------

            //   // -------------------- REPRESENTATION ---------------------

            //   ExpressionRule *Left, *Right;
            //   Operator        Operator;
            //};

            //class BracketExpressionRule : public ExpressionRule
            //{
            //public:
            //   BracketExpressionRule(ExpressionRule* exp, TokenIterator b, TokenIterator e) 
            //      : ExpressionRule(b,e), Expression(exp)
            //   {}
            //   ~BracketExpressionRule() 
            //   {
            //      delete Expression;
            //   }

            //   // ---------------------- ACCESSORS ------------------------			

            //   list<ScriptParameter>  GetParameters() const
            //   {
            //      return Expression->GetParameters();
            //   }

            //   // ----------------------- MUTATORS ------------------------

            //   // -------------------- REPRESENTATION ---------------------

            //   ExpressionRule *Expression;
            //};

            //class UnaryExpressionRule : public ExpressionRule
            //{
            //public:
            //   UnaryExpressionRule(ExpressionRule* exp, Operator o, TokenIterator b, TokenIterator e) 
            //      : ExpressionRule(b,e), Expression(exp), Operator(o)
            //   {}
            //   ~UnaryExpressionRule() 
            //   {
            //      delete Expression;
            //   }

            //   // ---------------------- ACCESSORS ------------------------			

            //   list<ScriptParameter>  GetParameters() const
            //   {
            //      return ScriptParameter(ParameterType::EXPRESSION, DataType::OPERATOR, (UINT)Operator)
            //          + Expression->GetParameters();
            //   }

            //   // ----------------------- MUTATORS ------------------------

            //   // -------------------- REPRESENTATION ---------------------

            //   ExpressionRule *Expression;
            //   Operator        Operator;
            //};


            //class LiteralExpressionRule : public ExpressionRule
            //{
            //public:
            //   LiteralExpressionRule(ScriptToken t, TokenIterator b, TokenIterator e) 
            //      : ExpressionRule(b,e), Token(t)
            //   {}
            //   ~LiteralExpressionRule()
            //   {}

            //   // ---------------------- ACCESSORS ------------------------			

            //   list<ScriptParameter>  GetParameters() const
            //   {
            //      switch (Token.Type)
            //      {
            //      case TokenType::Constant:
            //      case TokenType::Number:
            //         return ScriptParameter(ParameterType::EXPRESSION, DataType::INTEGER, Token.Text);
            //      case TokenType::Variable:
            //         return ScriptParameter(ParameterType::EXPRESSION, DataType::VARIABLE, Token.Text);
            //      }
            //      
            //   }

            //   // ----------------------- MUTATORS ------------------------

            //   // -------------------- REPRESENTATION ---------------------

            //   ScriptToken  Token;
            //};

            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandParser(const wstring& line, GameVersion  v);
            virtual ~ScriptCommandParser();

            DEFAULT_COPY(ScriptCommandParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptCommandParser);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            ScriptCommand  Parse();

         private:
            ScriptCommand  MatchLine();

            bool  MatchAssignment(TokenIterator& pos, wstring& var);
            bool  MatchConditional(TokenIterator& pos, Conditional& c);
            bool  MatchCommand(TokenIterator& pos, UINT& id, TokenArray& params);
            bool  MatchExpression(TokenIterator& pos, TokenArray& params);
            bool  MatchNOP(TokenIterator& pos);

            bool  Match(const TokenIterator& pos, TokenType  type);
            bool  Match(const TokenIterator& pos, TokenType  type, const TCHAR* txt);

            ScriptCommand GenerateCommand(UINT id, TokenArray& params);
            ScriptCommand GenerateCommand(UINT id, const wstring& retVar, TokenArray& params);
            ScriptCommand GenerateCommand(UINT id, Conditional c, TokenArray& params);

            // -------------------- REPRESENTATION ---------------------

         private:
            ScriptCommandLexer  Lexer;
            GameVersion         Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
