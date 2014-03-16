#pragma once
#include "Common.h"
#include "ScriptParser.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Expression tree traversals</summary>
         enum class Traversal  { InOrder, PostOrder };

         
         /// <summary>Occurs when a syntax error in detected in a script</summary>
         class ExpressionParserException : public ExceptionBase
         {
         public:
            /// <summary>Create an exception for an entire line</summary>
            /// <param name="src">Location of throw</param>
            /// <param name="msg">Message</param>
            ExpressionParserException(wstring  src, wstring msg) 
               : ExceptionBase(src, msg)
            {}

            /// <summary>Create an exception for a specific token</summary>
            /// <param name="src">Location of throw</param>
            /// <param name="pos">Token position</param>
            /// <param name="msg">Message</param>
            ExpressionParserException(wstring  src, const TokenIterator& pos, wstring msg) 
               : ExceptionBase(src, msg), Position(pos)
            {}

            TokenIterator Position;
         };

         /// <summary>Parses expression script commands</summary>
         class ExpressionParser
         {
            // ------------------------ TYPES --------------------------

            /// <summary>Base class for all expression tree nodes</summary>
            class Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               Expression() 
               {}
               virtual ~Expression() 
               {}

               // ---------------------- ACCESSORS ------------------------			

               // Debug:
               virtual wstring  debugPrint() const PURE;
               virtual wstring  debugPrintTraversal(Traversal t) const PURE;

               // Get tokens from tree
               virtual void     getTokenArray(Traversal t, TokenArray& tokens) const PURE;

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------
            };

            /// <summary>Shared pointer to an expression tree node</summary>
            typedef shared_ptr<Expression> ExpressionTree;

            /// <summary>Expression tree node for a literal value</summary>
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
                  return GuiString::Format(L"{Literal: %s}", Token.Text.c_str());
               }
               
               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrintTraversal(Traversal t) const
               {
                  return GuiString::Format(L" %s ", Token.Text.c_str());
               }

               /// <summary>Populates a token array</summary>
               /// <param name="t">Order of traversal</param>
               /// <param name="tokens">Array to populate</param>
               void  getTokenArray(Traversal t, TokenArray& tokens) const
               {
                  tokens += Token;
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Token;
            };

            /// <summary>Expression tree node for a bracketed sub-expression</summary>
            class BracketedExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               BracketedExpression(ScriptToken op, ExpressionTree expr, ScriptToken cl) : Open(op), Expression(expr), Close(cl)
               {}
               ~BracketedExpression()
               {}

               // Prevent shallow copying/moving
               NO_COPY(BracketedExpression);
               NO_MOVE(BracketedExpression);

               // ---------------------- ACCESSORS ------------------------			

               /// <summary>Prints contents to the console</summary>
               wstring  debugPrint() const
               {
                  return GuiString::Format(L"{Bracketed: %s}", Expression->debugPrint().c_str());
               }
               
               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               wstring  debugPrintTraversal(Traversal t) const
               {
                  return GuiString::Format(L" (%s) ", Expression->debugPrintTraversal(t).c_str());
               }

               /// <summary>Populates a token array</summary>
               /// <param name="t">Order of traversal</param>
               /// <param name="tokens">Array to populate</param>
               void  getTokenArray(Traversal t, TokenArray& tokens) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   tokens += Open;   Expression->getTokenArray(t, tokens);   tokens += Close; break;
                  case Traversal::PostOrder: Expression->getTokenArray(t, tokens);  break;
                  default: 
                     throw ArgumentException(HERE, L"t", VString(L"Unrecognised traversal type %d", t));
                  }
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Open,
                                  Close;
               ExpressionTree     Expression;
            };

            /// <summary>Expression tree node for an unary expression</summary>
            class UnaryExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               UnaryExpression(ScriptToken o, ExpressionTree v) : Operator(o), Value(v)
               {}
               ~UnaryExpression()
               {}

               // Prevent shallow copying/moving
               NO_COPY(UnaryExpression);
               NO_MOVE(UnaryExpression);

               // ---------------------- ACCESSORS ------------------------			

               /// <summary>Prints contents to the console</summary>
               wstring  debugPrint() const
               {
                  return GuiString::Format(L"{Unary: %s %s}", Operator.Text.c_str(), Value->debugPrint().c_str());
               }
               
               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               /// <exception cref="Logic::ArgumentException">Unrecognised traversal type</exception>
               wstring  debugPrintTraversal(Traversal t) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   return GuiString::Format(L"%s%s", Operator.Text.c_str(), Value->debugPrintTraversal(t).c_str());
                  case Traversal::PostOrder: return GuiString::Format(L"%s%s", Value->debugPrintTraversal(t).c_str(), Operator.Text.c_str());
                  default:
                     throw ArgumentException(HERE, L"t", VString(L"Unrecognised traversal type %d", t));
                  }
               }

               /// <summary>Populates a token array</summary>
               /// <param name="t">Order of traversal</param>
               /// <param name="tokens">Array to populate</param>
               /// <exception cref="Logic::ArgumentException">Unrecognised traversal type</exception>
               void  getTokenArray(Traversal t, TokenArray& tokens) const
               {
                  // Replace unary minus with
                  //ScriptToken op = (Operator.Text != L"-" ? Operator : ScriptToken(TokenType::Operator,Operator.Start,Operator.End,L"_"));

                  switch (t)
                  {
                  case Traversal::InOrder:   tokens += Operator;              Value->getTokenArray(t, tokens);  break;
                  case Traversal::PostOrder: Value->getTokenArray(t, tokens); tokens += Operator;               break;
                  default:  
                     throw ArgumentException(HERE, L"t", VString(L"Unrecognised traversal type %d", t));
                  }
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               const ScriptToken  Operator;
               ExpressionTree     Value;
            };

            /// <summary>Expression tree node for a binary expression</summary>
            class BinaryExpression : public Expression
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               BinaryExpression(ScriptToken op, ExpressionTree l, ExpressionTree r) : Operator(op), Left(l), Right(r)
               {}
               ~BinaryExpression()
               {}

               // Prevent shallow copying/moving
               NO_COPY(BinaryExpression);
               NO_MOVE(BinaryExpression);

               // ---------------------- ACCESSORS ------------------------			

               /// <summary>Prints contents to the console</summary>
               wstring  debugPrint() const
               {
                  return GuiString::Format(L"{Binary: %s %s %s}", Left->debugPrint().c_str(), 
                                                                       Operator.Text.c_str(), 
                                                                       Right->debugPrint().c_str());
               }
               
               /// <summary>Prints contents to the console</summary>
               /// <param name="t">The traversal type</param>
               /// <exception cref="Logic::ArgumentException">Unrecognised traversal type</exception>
               wstring  debugPrintTraversal(Traversal t) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   return Left->debugPrintTraversal(t) + Operator.Text + Right->debugPrintTraversal(t);
                  case Traversal::PostOrder: return Left->debugPrintTraversal(t) + Right->debugPrintTraversal(t) + Operator.Text;
                  default:
                     throw ArgumentException(HERE, L"t", VString(L"Unrecognised traversal type %d", t));
                  }
               }

               /// <summary>Populates a token array</summary>
               /// <param name="t">Order of traversal</param>
               /// <param name="tokens">Array to populate</param>
               /// <exception cref="Logic::ArgumentException">Unrecognised traversal type</exception>
               void  getTokenArray(Traversal t, TokenArray& tokens) const
               {
                  switch (t)
                  {
                  case Traversal::InOrder:   Left->getTokenArray(t, tokens);  tokens += Operator;  Right->getTokenArray(t, tokens);   break;
                  case Traversal::PostOrder: Left->getTokenArray(t, tokens);  Right->getTokenArray(t, tokens); tokens += Operator;    break;
                  default:  
                     throw ArgumentException(HERE, L"t", VString(L"Unrecognised traversal type %d", t));
                  }
               }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------

               ExpressionTree     Left,
                                  Right;
               const ScriptToken  Operator;
            };

         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ExpressionParser(TokenIterator& begin, const TokenIterator end);
            virtual ~ExpressionParser();

            // Default copy semantics
            DEFAULT_COPY(ExpressionParser);	
            // Default move semantics
            DEFAULT_MOVE(ExpressionParser);	

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

         protected:
            void  Parse(TokenIterator& start);

         private:
            bool  MatchLiteral(const TokenIterator& pos);
            bool  MatchOperator(const TokenIterator& pos, const WCHAR* op);
            bool  MatchOperator(const TokenIterator& pos, UINT precedence);

            const ScriptToken& ReadLiteral(TokenIterator& pos);
            const ScriptToken& ReadOperator(TokenIterator& pos);

            ExpressionTree  ReadExpression(TokenIterator& pos);
            ExpressionTree  ReadBinaryExpression(TokenIterator& pos, UINT precedence);
            ExpressionTree  ReadUnaryExpression(TokenIterator& pos);
            ExpressionTree  ReadValue(TokenIterator& pos);

            // -------------------- REPRESENTATION ---------------------

         public:
            TokenArray  InfixParams, 
                        PostfixParams;

         private:
            const UINT  MIN_PRECEDENCE = 0, 
                        MAX_PRECEDENCE = 9;

            const TokenIterator  InputBegin,
                                 InputEnd;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
