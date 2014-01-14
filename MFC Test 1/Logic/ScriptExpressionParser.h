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
         enum class Traversal  { InOrder, PreOrder, PostOrder };

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

               virtual wstring  debugPrint(Traversal t) const PURE;

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

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrint(Traversal t) const
               {
                  //Console::WriteLn(L"LiteralValue: %s", Token.Text.c_str());
                  return StringResource::Format(L"{Literal: %s }", Token.Text.c_str());
               }

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

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrint(Traversal t) const
               {
                  /*Console::WriteLn(L"BracketedExpression: ");
                  Expression->debugPrint(t);*/
                  return StringResource::Format(L"{Bracketed: (%s) }", Expression->debugPrint(t).c_str());
               }

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

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrint(Traversal t) const
               {
                  /*Console::WriteLn(L"UnaryExpression: %s", Operator.Text.c_str());
                  Value->debugPrint(t);*/
                  return StringResource::Format(L"{Unary: %s %s }", Operator.Text.c_str(), Value->debugPrint(t).c_str());
               }

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

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrint(Traversal t) const
               {
                  return StringResource::Format(L"{RHS: %s %s }", Operator.Text.c_str(), Right->debugPrint(t).c_str());
               }

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

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrint(Traversal t) const
               {
                  wstring sz = StringResource::Format(L"{Binary: %s ", Left->debugPrint(t).c_str());

                  for (RightHandSide* r : Components)
                     sz += StringResource::Format(L"%s ", r->debugPrint(t).c_str());

                  return sz + L"}";
               }

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

         public:
            void         Parse();

         private:
            ScriptToken* MatchLiteral(const TokenIterator& pos);
            ScriptToken* MatchOperator(const TokenIterator& pos, const WCHAR* op);

            Expression*  MatchExpression(TokenIterator& pos);
            Expression*  MatchSum(TokenIterator& pos);
            Expression*  MatchProduct(TokenIterator& pos);

            Expression*  MatchUnary(TokenIterator& pos);
            Expression*  MatchValue(TokenIterator& pos);

            // -------------------- REPRESENTATION ---------------------

         private:
            TokenIterator& InputBegin,
                           InputEnd;
            TokenArray&    Params;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
