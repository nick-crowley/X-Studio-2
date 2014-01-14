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
                         
            try
            {
               // Produce parse tree
               auto tree = ReadExpression(InputBegin);

               // DEBUG: Print
               Console::WriteLn(L"Output: %s", tree->debugPrint().c_str() );
               //Console::WriteLn(L"PreOrder (Not Used): %s", tree->printTraversal(Traversal::PreOrder).c_str());
               Console::WriteLn(L"Infix: %s", tree->printTraversal(Traversal::InOrder).c_str());
               Console::WriteLn(L"Postfix: %s", tree->printTraversal(Traversal::PostOrder).c_str());
               Console::WriteLn(L"");
            }
            catch (ScriptSyntaxException& e)
            {
               Console::WriteLn(e.Message);
            }
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


         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadExpression(TokenIterator& pos)
         {
            // Expression = Sum
            return ReadSum(pos);
         }

         // Input: Iterator positioned at next token (or EOF)
         // Out (Match): Iterator positioned after last token
         // Out (Fail): Iterator not moved


         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadSum(TokenIterator& pos)
         {
            Expression* sum = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Product (('+' / '-') Product)*
            try
            {
               // Read: product   (may throw)
               sum = ReadProduct(pos);

               // Match: operator  [nothrow]
               while ((op = MatchOperator(pos, L"+")) || (op = MatchOperator(pos, L"-")))
                  // Read: product  (may throw)
                  sum = new BinaryExpression(*op, sum, ReadProduct(++pos));   // Adv. Consume operator

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

         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadProduct(TokenIterator& pos)
         {
            Expression* product = nullptr;
            ScriptToken* op = nullptr;

            // Rule: Sum = Unary (('+' / '-') Unary)*
            try
            {
               // Read: Unary   (may throw)
               product = ReadUnary(pos);

               // Match: operator  [nothrow]
               while ((op = MatchOperator(pos, L"*")) || (op = MatchOperator(pos, L"/")) || (op = MatchOperator(pos, L"mod")))
                  // Read: Unary  (may throw)
                  product = new BinaryExpression(*op, product, ReadUnary(++pos));  // Adv. Consume operator

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

         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadUnary(TokenIterator& pos)
         {
            ScriptToken* op = nullptr;

            // Match: Operator  [nothrow]
            if ((op = MatchOperator(pos, L"!")) || (op = MatchOperator(pos, L"-")))
               // Read: Value  (may throw)
               return new UnaryExpression(*op, ReadValue(++pos));    // Adv. Consume operator

            // Read: Value  (may throw)
            return ReadValue(pos);
         }

         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadValue(TokenIterator& pos)
         {
            ScriptToken *val = nullptr,
                        *open = nullptr,
                        *close = nullptr;
            Expression  *expr = nullptr;

            // Match: Literal  [nothrow]
            if (val = MatchLiteral(pos)) 
            {
               ++pos;
               return new LiteralValue(*val);
            }

            // Read: Bracket   [nothrow]
            if ((open = MatchOperator(pos, L"(")) == nullptr)
               throw ScriptSyntaxException(HERE, L"Missing opening bracket");
            
            // Read: Expression  (may throw)
            expr = ReadExpression(++pos);   // Adv. then match

            // Read: Bracket   [nothrow]
            if (close = MatchOperator(pos, L")")) 
            {
               ++pos;
               return new BracketedExpression(*open, expr, *close);
            }
            
            // Failure:
            delete expr;
            throw ScriptSyntaxException(HERE, L"Missing closing bracket");
         }

      }
   }
}
