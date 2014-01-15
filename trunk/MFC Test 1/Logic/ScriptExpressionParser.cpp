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

         bool  ScriptExpressionParser::MatchLiteral(const TokenIterator& pos)
         {
            // Validate position 
            if (pos < InputEnd) 
               // check type
               switch (pos->Type)
               {
               case TokenType::Variable:
               case TokenType::Constant:
               case TokenType::Number:
               case TokenType::String:
               case TokenType::GameObject:
               case TokenType::ScriptObject:
                  return true;
               }

            return false;
         }

         bool ScriptExpressionParser::MatchOperator(const TokenIterator& pos, const WCHAR* op)
         {
            // Validate position and compare operator
            return pos < InputEnd && pos->Type == TokenType::Operator && pos->Text == op;
         }

         bool ScriptExpressionParser::MatchOperator(const TokenIterator& pos, UINT precedence)
         {
            // Validate position. Ensure operator
            if (pos >= InputEnd || pos->Type != TokenType::Operator)
               return false;

            // Precedence table taken from X2 scripting manual
            switch (precedence)
            {
            case 0: return pos->Text == L"OR";
            case 1: return pos->Text == L"AND";
            case 2: return pos->Text == L"|";
            case 3: return pos->Text == L"^";
            case 4: return pos->Text == L"&";
            case 5: return pos->Text == L"==" || pos->Text == L"!=";
            case 6: return pos->Text == L"<" || pos->Text == L">" || pos->Text == L"<=" || pos->Text == L">=";
            case 7: return pos->Text == L"+" || pos->Text == L"-";
            case 8: return pos->Text == L"*" || pos->Text == L"/" || pos->Text == L"mod";
            case 9: return pos->Text == L"~" || pos->Text == L"-" || pos->Text == L"!";
            }

            throw ArgumentException(HERE, L"precedence", GuiString(L"Invalid precedence %d", precedence));
         }

         const ScriptToken&  ScriptExpressionParser::ReadLiteral(TokenIterator& pos)
         {
            // Validate position/type
            if (pos >= InputEnd || pos->Type == TokenType::Operator || pos->Type == TokenType::Text)
               throw InvalidOperationException(HERE, L"Not a literal");

            // Read literal. Advance position
            return *(pos++);
         }


         const ScriptToken&  ScriptExpressionParser::ReadOperator(TokenIterator& pos)
         {
            // Verify operator
            if (pos >= InputEnd || pos->Type != TokenType::Operator)
               throw InvalidOperationException(HERE, L"Not an operator");

            // Read operator. Advance position
            return *(pos++);
         }


         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadExpression(TokenIterator& pos)
         {
            // Expression = Comparison
            return ReadBinaryExpression(pos, MIN_PRECEDENCE);
         }

         // Input: Iterator positioned at next token (or EOF)
         // Out (Match): Iterator positioned after last token
         // Out (Fail): Iterator not moved

         
         //ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadComparison(TokenIterator& pos)
         //{
         //   Expression* expr = nullptr;

         //   // Rule: Comparison = Bitwise ((& / | / ^) Bitwise)*
         //   try
         //   {
         //      // Read: Bitwise   (may throw)
         //      expr = ReadBitwise(pos);

         //      // Match: operator  
         //      while (MatchOperator(pos, L"==") || MatchOperator(pos, L"!=") || MatchOperator(pos, L">")
         //             || MatchOperator(pos, L"<") || MatchOperator(pos, L">=") || MatchOperator(pos, L"<="))
         //      {  // Read: operator, Bitwise  (may throw)
         //         auto op = ReadOperator(pos);
         //         expr = new BinaryExpression(op, expr, ReadBitwise(pos));   
         //      }

         //      // Success:
         //      return expr;
         //   }
         //   catch (...)
         //   {  // Cleanup
         //      if (expr != nullptr)
         //         delete expr;
         //      throw;
         //   }
         //}

         //ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadBitwise(TokenIterator& pos)
         //{
         //   Expression* expr = nullptr;

         //   // Rule: Bitwise = Logical ((& / | / ^) Logical)*
         //   try
         //   {
         //      // Read: Logical   (may throw)
         //      expr = ReadLogical(pos);

         //      // Match: operator  
         //      while (MatchOperator(pos, L"&") || MatchOperator(pos, L"|") || MatchOperator(pos, L"^")) 
         //      {  // Read: operator, Logical  (may throw)
         //         auto op = ReadOperator(pos);
         //         expr = new BinaryExpression(op, expr, ReadLogical(pos));   
         //      }

         //      // Success:
         //      return expr;
         //   }
         //   catch (...)
         //   {  // Cleanup
         //      if (expr != nullptr)
         //         delete expr;
         //      throw;
         //   }
         //}

         //ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadLogical(TokenIterator& pos)
         //{
         //   Expression* expr = nullptr;

         //   // Rule: Logical = Sum ((AND / OR) Sum)*
         //   try
         //   {
         //      // Read: Sum   (may throw)
         //      expr = ReadSum(pos);

         //      // Match: operator  
         //      while (MatchOperator(pos, L"AND") || MatchOperator(pos, L"OR")) 
         //      {  // Read: operator, Sum  (may throw)
         //         auto op = ReadOperator(pos);
         //         expr = new BinaryExpression(op, expr, ReadSum(pos));   
         //      }

         //      // Success:
         //      return expr;
         //   }
         //   catch (...)
         //   {  // Cleanup
         //      if (expr != nullptr)
         //         delete expr;
         //      throw;
         //   }
         //}

         //ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadSum(TokenIterator& pos)
         //{
         //   Expression* expr = nullptr;

         //   // Rule: Sum = Product (('+' / '-') Product)*
         //   try
         //   {
         //      // Read: product   (may throw)
         //      expr = ReadProduct(pos);

         //      // Match: operator  
         //      while (MatchOperator(pos, L"+") || MatchOperator(pos, L"-")) 
         //      {  // Read: operator, product  (may throw)
         //         auto op = ReadOperator(pos);
         //         expr = new BinaryExpression(op, expr, ReadProduct(pos));   
         //      }

         //      // Success:
         //      return expr;
         //   }
         //   catch (...)
         //   {  
         //      // Cleanup
         //      if (expr != nullptr)
         //         delete expr;
         //      throw;
         //   }
         //}

         //ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadProduct(TokenIterator& pos)
         //{
         //   Expression* expr = nullptr;

         //   // Rule: Sum = Unary (('+' / '-') Unary)*
         //   try
         //   {
         //      // Read: Unary   (may throw)
         //      expr = ReadUnary(pos);

         //      // Match: operator  
         //      while (MatchOperator(pos, L"*") || MatchOperator(pos, L"/") || MatchOperator(pos, L"mod")) 
         //      {  // Read: operator, Unary  (may throw)
         //         auto op = ReadOperator(pos);
         //         expr = new BinaryExpression(op, expr, ReadUnary(pos));  
         //      }

         //      // Success:
         //      return expr;
         //   }
         //   catch (...)
         //   {  
         //      // Cleanup
         //      if (expr != nullptr)
         //         delete expr;
         //      throw;
         //   }
         //}

         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadBinaryExpression(TokenIterator& pos, UINT precedence)
         {
            Expression* expr = nullptr;

            // Rule: BinaryExpr = Operand (operator Operand)*
            //       Operand = Value / UnaryExpr / MultDivExpr / SumExpr / BitwiseExpr / LogicalExpr / ComparisonExpr
            try
            {
               // Read: expression-of-higher-precedence / Value
               expr = (precedence < MAX_PRECEDENCE ? ReadBinaryExpression(pos, precedence+1) : ReadUnaryExpression(pos));    // throws

               // Match: operator  
               while (MatchOperator(pos, precedence))
               {  
                  // Read: operator
                  auto op = ReadOperator(pos);                                                        

                  // Read: expression-of-higher-precedence / Value
                  auto rhs = (precedence < MAX_PRECEDENCE ? ReadBinaryExpression(pos, precedence+1) : ReadUnaryExpression(pos));  // throws
                  expr = new BinaryExpression(op, expr, rhs);     // throws
               }

               // Success:
               return expr;
            }
            catch (...)
            {  // Cleanup
               if (expr != nullptr)
                  delete expr;
               throw;
            }
         }

         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadUnaryExpression(TokenIterator& pos)
         {
            // Rule: Unary = (! / - / ~)? Value

            // Match: Operator  
            if (MatchOperator(pos, MAX_PRECEDENCE)) 
            {   // Read: operator, Value  (may throw)
               const ScriptToken& op = ReadOperator(pos);
               return new UnaryExpression(op, ReadValue(pos));    
            }

            // Read: Value  (may throw)
            return ReadValue(pos);
         }

         
         ScriptExpressionParser::Expression*  ScriptExpressionParser::ReadValue(TokenIterator& pos)
         {
            Expression* expr = nullptr;

            // Rule: Value = Literal / '(' Expression ')'

            // Match: Literal  
            if (MatchLiteral(pos)) 
               return new LiteralValue(ReadLiteral(pos));

            // Read: Bracket   [nothrow]
            if (!MatchOperator(pos, L"(")) 
            {
               // Failed: Unexpected EOF
               if (pos >= InputEnd)
                  throw ScriptSyntaxException(HERE, L"Missing operand");

               // Failed: Unexpected token
               throw ScriptSyntaxException(HERE, GuiString(L"Unexpected '%s'", pos->Text.c_str()));
            }
            
            // Read: Expression  (may throw)
            const ScriptToken& open = ReadOperator(pos);
            expr = ReadExpression(pos);   // Adv. then match

            // Read: Bracket   [nothrow]
            if (MatchOperator(pos, L")")) 
               return new BracketedExpression(open, expr, ReadOperator(pos));
            
            // Failure: Missing closing bracket
            delete expr;
            throw ScriptSyntaxException(HERE, L"Missing closing bracket");
         }

      }
   }
}
