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
         enum class Traversal  { InOrder, PostOrder };

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

               virtual wstring  debugPrint() const PURE;
               virtual wstring  printTraversal(Traversal t) const PURE;

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
               wstring  debugPrint() const
               {
                  return StringResource::Format(L"{Literal: %s}", Token.Text.c_str());
               }

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  printTraversal(Traversal t) const
               {
                  return StringResource::Format(L" %s ", Token.Text.c_str());
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
               wstring  debugPrint() const
               {
                  return StringResource::Format(L"{Bracketed: ( %s ) }", Expression->debugPrint().c_str());
               }

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  printTraversal(Traversal t) const
               {
                  return StringResource::Format(L" (%s) ", Expression->printTraversal(t).c_str());
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
               wstring  debugPrint() const
               {
                  return StringResource::Format(L"{Unary: %s %s}", Operator.Text.c_str(), Value->debugPrint().c_str());
               }

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  printTraversal(Traversal t) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   return StringResource::Format(L"%s%s", Operator.Text.c_str(), Value->printTraversal(t).c_str());
                  case Traversal::PostOrder: return StringResource::Format(L"%s%s", Value->printTraversal(t).c_str(), Operator.Text.c_str());
                  }
                  throw "Invalid traversal";
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Operator;
               Expression*        Value;
            };


            class BinaryExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               BinaryExpression(ScriptToken op, Expression* l, Expression* r) : Operator(op), Left(l), Right(r)
               {}
               ~BinaryExpression() {
                  delete Left;  Left = nullptr;
                  delete Right; Right = nullptr;
               }

               // Prevent shallow copying/moving
               NO_COPY(BinaryExpression);
               NO_MOVE(BinaryExpression);

               // ---------------------- ACCESSORS ------------------------			

               /// <summary>Prints contents to the console</summary>
               wstring  debugPrint() const
               {
                  return StringResource::Format(L"{Binary: %s %s %s}", Left->debugPrint().c_str(), 
                                                                       Operator.Text.c_str(), 
                                                                       Right->debugPrint().c_str());
               }

               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  printTraversal(Traversal t) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   return Left->printTraversal(t) + Operator.Text + Right->printTraversal(t);
                  case Traversal::PostOrder: return Left->printTraversal(t) + Right->printTraversal(t) + Operator.Text;
                  }

                  throw "Invalid traversal";
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               Expression        *Left,
                                 *Right;
               const ScriptToken  Operator;
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
            bool         MatchLiteral(const TokenIterator& pos);
            bool         MatchOperator(const TokenIterator& pos, const WCHAR* op);
            bool         MatchOperator(const TokenIterator& pos, UINT precedence);

            const ScriptToken& ReadLiteral(TokenIterator& pos);
            const ScriptToken& ReadOperator(TokenIterator& pos);

            Expression*  ReadExpression(TokenIterator& pos);
            /*Expression*  ReadComparison(TokenIterator& pos);
            Expression*  ReadBitwise(TokenIterator& pos);
            Expression*  ReadLogical(TokenIterator& pos);
            Expression*  ReadSum(TokenIterator& pos);
            Expression*  ReadProduct(TokenIterator& pos);*/

            Expression*  ReadBinaryExpression(TokenIterator& pos, UINT precedence);
            Expression*  ReadUnaryExpression(TokenIterator& pos);
            Expression*  ReadValue(TokenIterator& pos);

            // -------------------- REPRESENTATION ---------------------

         private:
            const UINT  MIN_PRECEDENCE = 0, MAX_PRECEDENCE = 9;

            TokenIterator& InputBegin,
                           InputEnd;
            TokenArray&    Params;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
