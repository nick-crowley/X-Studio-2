#include "stdafx.h"
#include "ScriptExpressionParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptExpressionParser::ScriptExpressionParser(TokenIterator& begin, TokenIterator& end, TokenArray& params)
            : InputBegin(begin), InputEnd(end), Params(params)
         {
         }


         ScriptExpressionParser::~ScriptExpressionParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         void  ScriptExpressionParser::Parse()
         {
            // DEBUG: print input
            Console::Write(L"Input: ");
            for (auto it = InputBegin; it != InputEnd; ++it)
               Console::Write(L"%s ", it->Text.c_str());
            Console::WriteLn(L"");
                         
            // Produce parse tree
            auto tree = MatchExpression(InputBegin);

            // DEBUG: Print
            Console::WriteLn(L"Output: %s", tree->debugPrint().c_str() );
            //Console::WriteLn(L"PreOrder (Not Used): %s", tree->printTraversal(Traversal::PreOrder).c_str());
            Console::WriteLn(L"InOrder (Infix): %s", tree->printTraversal(Traversal::InOrder).c_str());
            Console::WriteLn(L"PostOrder (Postfix): %s", tree->printTraversal(Traversal::PostOrder).c_str());
            Console::WriteLn(L"");
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------

         ScriptToken*  ScriptExpressionParser::MatchLiteral(const TokenIterator& pos)
         {
            // Validate position + check type
            if (pos < InputEnd && pos->Type != TokenType::Operator && pos->Type != TokenType::Text)
               return pos._Ptr;
            
            return nullptr;
         }


         ScriptToken* ScriptExpressionParser::MatchOperator(const TokenIterator& pos, const WCHAR* op)
         {
            // Validate position and compare operator
            if (pos < InputEnd && pos->Type == TokenType::Operator && pos->Text == op)
               return pos._Ptr;
            
            return nullptr;
         }


         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchExpression(TokenIterator& pos)
         {
            // Expression = Sum
            return MatchSum(pos);
         }

         // Input: Iterator positioned at next token (or EOF)
         // Out (Match): Iterator positioned after last token
         // Out (Fail): Iterator not moved


         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchSum(TokenIterator& pos)
         {
            Expression* sum = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Product (('+' / '-') Product)*
            try
            {
               // Match: product   (may throw)
               if ((sum = MatchProduct(pos)) == nullptr)
                  throw "Missing unary";

               // Match: operator  [nothrow]
               while ((op = MatchOperator(pos, L"+")) || (op = MatchOperator(pos, L"-")))
               {
                  // Match: product  (may throw)
                  if (Expression* right = MatchProduct(++pos))  // Adv. Consume operator
                     sum = new BinaryExpression(*op, sum, right);
                  else 
                     throw "Operator but no unary";
               }

               // Success:
               return sum;
            }
            catch (...)
            {  
               // Cleanup
               if (sum != nullptr)
                  delete sum;
               throw;
            }
         }

         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchProduct(TokenIterator& pos)
         {
            Expression* product = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Unary (('+' / '-') Unary)*
            try
            {
               // Match: Unary   (may throw)
               if ((product = MatchUnary(pos)) == nullptr)
                  throw "Missing unary";

               // Match: operator  [nothrow]
               while ((op = MatchOperator(pos, L"*")) || (op = MatchOperator(pos, L"/")))
               {
                  // Match: Unary  (may throw)
                  if (Expression* right = MatchUnary(++pos))  // Adv. Consume operator
                     product = new BinaryExpression(*op, product, right);
                  else 
                     throw "Operator but no unary";
               }

               // Success:
               return product;
            }
            catch (...)
            {  
               // Cleanup
               if (product != nullptr)
                  delete product;
               throw;
            }
         }

         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchUnary(TokenIterator& pos)
         {
            Expression*  value = nullptr;
            ScriptToken* op = nullptr;

            // Match: Operator  [nothrow]
            if ((op = MatchOperator(pos, L"!")) || (op = MatchOperator(pos, L"-")))
            {
               // Match: Value  (may throw)
               if (value = MatchValue(++pos))   // Adv. Consume operator
                  return new UnaryExpression(*op, value);
               else
                  throw "Missing unary operator";
            }

            // Match: Value  (may throw)
            if ((value = MatchValue(pos)) == nullptr)
               throw "Missing value";

            // Success
            return value;
         }

         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchValue(TokenIterator& pos)
         {
            ScriptToken *val = nullptr,
                        *open = nullptr,
                        *close = nullptr;
            Expression  *expr = nullptr;

            // Match: Literal  [nothrow]
            if (val = MatchLiteral(pos)) {
               ++pos;
               return new LiteralValue(*val);
            }

            // Match: Bracket   [nothrow]
            if ((open = MatchOperator(pos, L"(")) == nullptr)
               throw "Missing opening bracket";
            
            // Match: Expression  (may throw)
            if ((expr = MatchExpression(++pos)) == nullptr) // Adv. then match
               throw "Missing expression";

            // Match: Bracket   [nothrow]
            if ((close = MatchOperator(pos, L")")) == nullptr) 
            {
               delete expr;
               throw "Missing opening bracket";
            }
          
            // Success:
            ++pos;
            return new BracketedExpression(*open, expr, *close);
         }

      }
   }
}
