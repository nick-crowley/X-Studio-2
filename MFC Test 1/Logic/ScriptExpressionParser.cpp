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
         {
         }


         ScriptExpressionParser::~ScriptExpressionParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

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
            BinaryExpression* sum = nullptr;
            Expression *left = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Product (('+' / '-') Product)*

            // Match: product
            if (left = MatchProduct(pos))
               sum = new BinaryExpression(left);
            else
               throw "Missing product";
            
            // Match: operator  [nothrow]
            while ((op = MatchOperator(pos, L"+")) || (op = MatchOperator(pos, L"-")))
            {
               try
               {  // Match: Product  (may throw)
                  if (Expression* right = MatchProduct(++pos))   // Consume operator
                     sum->Add(new RightHandSide(*op, right));
                  else 
                     throw "Operator but no product";
               }
               catch (...)
               {  // Error: Cleanup 
                  delete sum;
                  throw;
               }
            }

            // Success:
            return sum;
         }

         ScriptExpressionParser::Expression*  ScriptExpressionParser::MatchProduct(TokenIterator& pos)
         {
            BinaryExpression* product = nullptr;
            Expression* left = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Unary (('+' / '-') Unary)*

            // Match: Unary   (may throw)
            if (left = MatchUnary(pos))
               product = new BinaryExpression(left);
            else
               throw "Missing unary";
            
            // Match: operator  [nothrow]
            while ((op = MatchOperator(pos, L"*")) || (op = MatchOperator(pos, L"/")))
            {
               try
               {  // Match: Unary  (may throw)
                  if (Expression* right = MatchUnary(++pos))  // Adv. Consume operator
                     product->Add(new RightHandSide(*op, right));
                  else 
                     throw "Operator but no unary";
               }
               catch (...)
               {  // Error: Cleanup
                  delete product;
                  throw;
               }
            }

            // Success:
            return product;
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
            if (value = MatchValue(pos))
               return new ValueExpression(value);

            // Failed:
            throw "Missing value";
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
