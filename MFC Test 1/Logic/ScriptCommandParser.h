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
         /// <summary></summary>
         class ScriptCommandParser
         {
            
            typedef TokenArray::const_iterator  TokenIterator;

            

            enum class Operator : UINT { Equals };

            class Match
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               Match(TokenIterator b, TokenIterator e) : Begin(b), End(e)
               {}

               // ---------------------- ACCESSORS ------------------------			

               virtual ScriptParameter  GetParameter(ParameterSyntax s) const PURE;
               int                      size() const { return End - Begin; }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const TokenIterator Begin, End;
            };

            typedef vector<Match>   MatchCollection;

            class CommandHash 
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               CommandHash(TokenIterator b, TokenIterator e) : Hash(GenerateHash(b,e)), Remainder(SeparateParams(b,e))
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
               const TokenArray  Remainder;
            };

            class NoOperationRule : public Match
            {
            public:
               NoOperationRule(TokenIterator end) : Match(end, end)
               {}

               // ---------------------- ACCESSORS ------------------------			

               ScriptCommand  GetCommand() const
               {
                  return ScriptCommand(SyntaxLib.Find(CMD_NOP), ParameterArray());
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               Conditional  Conditional;
            };

            class ConditionalRule : public Match
            {
            public:
               ConditionalRule(Conditional c, TokenIterator b, TokenIterator e) : Match(b,e), Conditional(c)
               {}

               // ---------------------- ACCESSORS ------------------------			

               ScriptParameter  GetParameter(ParameterSyntax s) const
               {
                  return ScriptParameter(s, DataType::VARIABLE, ReturnValue(Conditional).Value);
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               Conditional  Conditional;
            };

            class CommandRule : public Match
            {
            public:
               CommandRule(CommandSyntax s, TokenArray tok, TokenIterator b, TokenIterator e) : Match(b,e), Syntax(s), Tokens(tok)
               {}

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               CommandSyntax  Syntax;
               TokenArray     Tokens;
            };

            class ExpressionRule : public CommandRule
            {
            public:
               ExpressionRule(TokenIterator b, TokenIterator e) : CommandRule(SyntaxLib.Find(CMD_EXPRESSION), b,e)
               {}

               // ---------------------- ACCESSORS ------------------------			

               ScriptParameter  GetParameter(ParameterSyntax s) const
               {
                  throw InvalidOperationException();
               }

               virtual list<ScriptParameter>  GetParameters() const PURE;

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------
            };

            class BinaryExpressionRule : public ExpressionRule
            {
            public:
               BinaryExpressionRule(ExpressionRule* l, Operator o, ExpressionRule* r, TokenIterator b, TokenIterator e) 
                  : ExpressionRule(b,e), Left(l), Right(r), Operator(o)
               {}
               ~BinaryExpressionRule() 
               {
                  delete Left;
                  delete Right;
               }

               // ---------------------- ACCESSORS ------------------------			

               list<ScriptParameter>  GetParameters() const
               {
                  return Left->GetParameters() 
                      + ScriptParameter(ParameterType::EXPRESSION, DataType::OPERATOR, (UINT)Operator)
                      + Right->GetParameters();
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ExpressionRule *Left, *Right;
               Operator        Operator;
            };

            class BracketExpressionRule : public ExpressionRule
            {
            public:
               BracketExpressionRule(ExpressionRule* exp, TokenIterator b, TokenIterator e) 
                  : ExpressionRule(b,e), Expression(exp)
               {}
               ~BracketExpressionRule() 
               {
                  delete Expression;
               }

               // ---------------------- ACCESSORS ------------------------			

               list<ScriptParameter>  GetParameters() const
               {
                  return Expression->GetParameters();
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ExpressionRule *Expression;
            };

            class UnaryExpressionRule : public ExpressionRule
            {
            public:
               UnaryExpressionRule(ExpressionRule* exp, Operator o, TokenIterator b, TokenIterator e) 
                  : ExpressionRule(b,e), Expression(exp), Operator(o)
               {}
               ~UnaryExpressionRule() 
               {
                  delete Expression;
               }

               // ---------------------- ACCESSORS ------------------------			

               list<ScriptParameter>  GetParameters() const
               {
                  return ScriptParameter(ParameterType::EXPRESSION, DataType::OPERATOR, (UINT)Operator)
                      + Expression->GetParameters();
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ExpressionRule *Expression;
               Operator        Operator;
            };


            class LiteralExpressionRule : public ExpressionRule
            {
            public:
               LiteralExpressionRule(ScriptToken t, TokenIterator b, TokenIterator e) 
                  : ExpressionRule(b,e), Token(t)
               {}
               ~LiteralExpressionRule()
               {}

               // ---------------------- ACCESSORS ------------------------			

               list<ScriptParameter>  GetParameters() const
               {
                  switch (Token.Type)
                  {
                  case TokenType::Constant:
                  case TokenType::Number:
                     return ScriptParameter(ParameterType::EXPRESSION, DataType::INTEGER, Token.Text);
                  case TokenType::Variable:
                     return ScriptParameter(ParameterType::EXPRESSION, DataType::VARIABLE, Token.Text);
                  }
                  
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ScriptToken  Token;
            };

            class ConditionalCommandRule
            {
            public:
               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ConditionalRule  Conditional;
               CommandRule      Command;
            };

            class ConditionalExpressionRule
            {
            public:
               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ConditionalRule  Conditional;
               ExpressionRule   Expression;
            };

            class AssignmentCommandRule
            {
            public:
               AssignmentRule  Assignment;
               CommandRule     Command;
            };

            class AssignmentExpressionRule
            {
            public:
               AssignmentRule  Assignment;
               ExpressionRule  Expression;
            };

            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandParser(const wstring& line);
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

            void  MatchConditional(TokenIterator pos, MatchCollection m);
            void  MatchCommand(TokenIterator pos, MatchCollection m);

            // -------------------- REPRESENTATION ---------------------

         private:
            ScriptCommandLexer  Lexer;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
