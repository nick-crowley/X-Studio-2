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
            typedef vector<ScriptToken>::const_iterator  TokenIterator;

            typedef vector<Match>   MatchCollection;

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
               CommandRule(CommandSyntax s, TokenIterator b, TokenIterator e) : Match(b,e), Syntax(s)
               {}

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               CommandSyntax  Syntax;
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
