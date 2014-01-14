#pragma once
#include "Common.h"
#include "ScriptCommandParser.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         class ScriptExpressionParser
         {
            // ------------------------ TYPES --------------------------
            
            class Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               Expression() 
               {}
               virtual ~Expression() 
               {}

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------
            };

            class LiteralValue : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               LiteralValue(ScriptToken t) : Token(t)
               {}

               // Prevent shallow copying/moving
               NO_COPY(LiteralValue);
               NO_MOVE(LiteralValue);

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Token;
            };

            class BracketedExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               BracketedExpression(ScriptToken op, Expression* expr, ScriptToken cl) : Open(op), Expression(expr), Close(cl)
               {}
               ~BracketedExpression() {
                  delete Expression;
                  Expression = nullptr;
               }

               // Prevent shallow copying/moving
               NO_COPY(BracketedExpression);
               NO_MOVE(BracketedExpression);

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Open,
                                  Close;
               Expression*        Expression;
            };

            class UnaryExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               UnaryExpression(ScriptToken o, Expression* v) : Operator(o), Value(v)
               {}
               ~UnaryExpression() {
                  delete Value;
                  Value = nullptr;
               }

               // Prevent shallow copying/moving
               NO_COPY(UnaryExpression);
               NO_MOVE(UnaryExpression);

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Operator;
               Expression*        Value;
            };

            class RightHandSide : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               RightHandSide(ScriptToken o, Expression* r) : Operator(o), Right(r)
               {}
               ~RightHandSide() {
                  delete Right;
                  Right = nullptr;
               }

               // Prevent shallow copying/moving
               NO_COPY(RightHandSide);
               NO_MOVE(RightHandSide);

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken Operator;
               Expression*       Right;
            };

            class BinaryExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               BinaryExpression(Expression* l) : Left(l)
               {}
               ~BinaryExpression() {
                  delete Left;
                  Left = nullptr;
                  for_each(Components.begin(), Components.end(), [](RightHandSide* &r) { delete r; r=nullptr; });
               }

               // Prevent shallow copying/moving
               NO_COPY(BinaryExpression);
               NO_MOVE(BinaryExpression);

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------

               void  Add(RightHandSide* r) {
                  Components.push_back(r);
               }

               // -------------------- REPRESENTATION ---------------------

               Expression*          Left;
               list<RightHandSide*> Components;
            };

         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptExpressionParser(TokenIterator& begin, TokenIterator& end, TokenArray& params);
            virtual ~ScriptExpressionParser();

            DEFAULT_COPY(ScriptExpressionParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptExpressionParser);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            ScriptToken* MatchLiteral(const TokenIterator& pos);
            ScriptToken* MatchOperator(const TokenIterator& pos, const WCHAR* op);

            Expression*  MatchExpression(TokenIterator& pos);
            Expression*  MatchSum(TokenIterator& pos);
            Expression*  MatchProduct(TokenIterator& pos);

            Expression*  MatchUnary(TokenIterator& pos);
            Expression*  MatchValue(TokenIterator& pos);

            // -------------------- REPRESENTATION ---------------------

         private:
            TokenIterator  InputBegin,
                           InputEnd;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
