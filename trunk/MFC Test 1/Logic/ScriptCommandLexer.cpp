#include "stdafx.h"
#include "ScriptCommandLexer.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptCommandLexer::ScriptCommandLexer(const wstring& line) 
            : LineStart(line.begin()), LineEnd(line.end()), Position(LineStart), Tokens(Parse())
         {
            // DEBUG:
            /*Console::WriteLn(L"\nLexing command: %s", line.c_str());
            for (const ScriptToken& t : Tokens)
               Console::WriteLn(L"Token: '%s'", t.Text.c_str());
            Console::WriteLn();*/
         }


         ScriptCommandLexer::~ScriptCommandLexer()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         /// <summary>Parses all the input text</summary>
         /// <returns>Token array</returns>
         TokenArray ScriptCommandLexer::Parse()
         {
            TokenArray output;

            // Read until EOL
            while (ValidPosition)
            {
               // Whitespace: Skip
               /*switch (*Position)
               {
               case '\t':
               case '\r':
               case '\n': 
               case '\v': 
               case ' ':    
                  ReadWhitespace(); 
                  continue;
               }*/

               // Whitespace: Skip
               if (MatchWhitespace()) 
                  ReadWhitespace();

               // Comment: 
               else if (output.size() == 1 && output[0].Text == L"*")
                  output.push_back( ReadComment(Position) );

               // Number:
               else if (MatchNumber())
                  output.push_back( ReadNumber(Position) );      

               // Remainder: 
               else switch (*Position)
               {
               /*case L'0': case L'5': 
               case L'1': case L'6':
               case L'2': case L'7':
               case L'3': case L'8':
               case L'4': case L'9':   
                  output.push_back( ReadNumber(Position) );      
                  break;*/

               case L'$':   output.push_back( ReadVariable(Position) );    break;
               case L'{':   output.push_back( ReadGameObject(Position) );  break;
               case L'\'':  output.push_back( ReadString(Position) );      break;

               default: 
                  output.push_back( ReadAmbiguous(Position) );   
                  break;
               }
            }

            return output;
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
         
         /// <summary>Creates a token from text between 'start' and current position</summary>
         /// <param name="start">The start.</param>
         /// <param name="t">Token type</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::MakeToken(StringIterator start, TokenType t) const
         {
            return ScriptToken(t, start-LineStart, Position-LineStart, wstring(start, Position));
         }

         /// <summary>Check if current char matches a character</summary>
         /// <param name="ch">The character to test</param>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchChar(WCHAR ch) const
         {
            return ValidPosition && *Position == ch;
         }

         /// <summary>Check if current position matches a string (CASE SENSITIVE)</summary>
         /// <param name="str">The string to test</param>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchChars(const WCHAR* str) const
         {
            // Match each char
            for (UINT i = 0; str[i]; ++i)
               if (Position+i >= LineEnd || Position[i] != str[i])
                  return false; // EOL/Mismatch

            // Match
            return true;
         }

         /// <summary>Check if current char is the opening bracket of a script object</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchConstant() const
         {
            // Match: '[', alpha
            return ValidPosition && MatchChar(L'[')
                && Position+1 >= LineEnd && iswalpha(Position[1]);
         }

         /// <summary>Check if current character is a number</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchNumber() const
         {
            return ValidPosition && iswdigit(*Position);
         }

         /// <summary>Check if current char is an operator</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchOperator() const
         {
            if (ValidPosition)
               // Check first char of operator
               switch (*Position)
               {
               // (for Expressions)
               case L'=':
               case L'<':
               case L'>':
               case L'!':
               case L'~':
               case L'-':
               case L'+':
               case L'*':
               case L'/':
               case L'&':
               case L'^':
               case L'|':
               case L'(':
               case L')':  
               // (for Commands)
               case L',':
               case L':':
               case L'[':
               case L']':
                  return true;

               // and/or/mod
               case L'A':  return MatchChars(L"AND");
               case L'o':  return MatchChars(L"OR");
               case L'M':  return MatchChars(L"MOD");
               }

            return false;
         }

         
         /// <summary>Check if current char is text.</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchText() const
         {
            // Exclude whitespace and known operators, while allowing all other characters
            if (ValidPosition)
               switch (*Position)
               {
               // (for Expressions)
               case L'=':
               case L'<':
               case L'>':
               case L'!':
               case L'~':
               case L'-':
               case L'+':
               case L'*':
               case L'/':
               case L'&':
               case L'^':
               case L'|':
               case L'(':
               case L')':  
               // (for Commands)
               case L',':
               case L':':
               case L'[':
               case L']':
               case L'$':  // Variable
               case L'{':  // Game object
               case L'}':  // Game object
               case L'\'': // String
                  break;

               // Allow multi-lingual alphanumeric.  Exclude whitespace 
               default:
                  return !MatchWhitespace();
               }

            // Failed
            return false;
         }
         
         /// <summary>Check if current char is variable name</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchVariable() const
         {
            // Match alphanumeric/dot/underscore  (nb: not dollar)
            return ValidPosition && (iswalnum(*Position) || MatchChar(L'.') || MatchChar(L'_'));
         }

         /// <summary>Check if current char is whitespace</summary>
         /// <returns></returns>
         bool  ScriptCommandLexer::MatchWhitespace() const
         {
            // Match whitespace
            return ValidPosition && iswspace(*Position);
         }

         /// <summary>Consume character and move to the next, if available</summary>
         /// <returns>True if found, false if end-of-line</returns>
         bool  ScriptCommandLexer::ReadChar()
         {
            return ValidPosition && ++Position < LineEnd;
         }
         
         /// <summary>Reads a ScriptObject, Operator or Text, some of which share first letters</summary>
         /// <param name="start">Current position (first character)</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::ReadAmbiguous(StringIterator start)
         {
            // Constant: Avoid interpreting '[' as an operator
            if (MatchConstant())
               return ReadConstant(start);

            // Operator: 
            if (MatchOperator())
               return ReadOperator(start);

            // Text: anything else 
            return ReadText(start);
         }

         /// <summary>Reads the script object</summary>
         /// <param name="start">Current position (opening bracket)</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::ReadConstant(StringIterator start)
         {
            // Consume all, excluding trailing bracket
            while (ReadChar() && !MatchChar(L']'))
            {}
            
            // Consume backet
            if (MatchChar(L']'))
               ReadChar();

            // Return COMMENT
            return MakeToken(start, TokenType::ScriptObject);
         }

         /// <summary>Reads a comment</summary>
         /// <param name="start">Current position (opening bracket)</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::ReadComment(StringIterator start)
         {
            // Consume entire line
            while (ReadChar())
            {}
            
            // Return COMMENT
            return MakeToken(start, TokenType::Comment);
         }


         /// <summary>Reads the game object.</summary>
         /// <param name="start">Current position (opening bracket)</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::ReadGameObject(StringIterator start)
         {
            // Consume all, excluding trailing bracket
            while (ReadChar() && !MatchChar(L'}'))
            {}

            // Consume backet
            if (MatchChar(L'}'))
               ReadChar();
            
            // Return GAME OBJECT
            return MakeToken(start, TokenType::GameObject);
         }
         
         /// <summary>Reads the string</summary>
         /// <param name="start">Current position (leading apostrophe)</param>
         /// <returns></returns>
         ScriptToken  ScriptCommandLexer::ReadString(StringIterator start)
         {
            // Consume all, excluding trailing apostrophe
            while (ReadChar() && !MatchChar(L'\''))
            {}

            // Consume apostrophe
            if (MatchChar(L'\''))
               ReadChar();
            
            // Return STRING
            return MakeToken(start, TokenType::String);
         }


         ScriptToken  ScriptCommandLexer::ReadNumber(StringIterator start)
         {
            // Consume digits
            while (MatchNumber() && ReadChar())
            {}
            
            // Return NUMBER
            return MakeToken(start, TokenType::Number);
         }

         
         ScriptToken  ScriptCommandLexer::ReadOperator(StringIterator start) 
         {
            // Read single char 
            switch (*Position)
            {
            // (for Expressions)
            case L'~':
            case L'+':
            case L'*':
            case L'/':
            case L'&':
            case L'^':
            case L'|':
            case L'(':
            case L')':  
            // (for Commands)
            case L',':
            case L':':
            case L'[':
            case L']':
               ReadChar();
               break;

            // Read multi-char operators
            default:
               switch (*Position)
               {
               // RefObj
               case L'-':  // - or ->
                  ReadChar();
                  // Read RefObj if present
                  if (MatchChar('>'))
                     ReadChar();
                  break;

               // Comparisons
               case L'<':  // < or <=
               case L'>':  // > or >=
               case L'!':  // ! or !=
               case L'=':  // = or ==
                  ReadChar();
                  // Read double-char if a comparison
                  if (MatchChar('='))
                     ReadChar();
                  break;

               // OR: Read double
               case L'O':  
                  Position += 2;
                  break;

               // AND/MOD: Read triple
               case L'A':  
               case L'M':  
                  Position += 3; 
                  break;

               // Error: should never reach here
               default:
                  throw ArgumentException(HERE, L"Position", L"Unable to read previously matched operator");
               }
            }

            // Return OPERATOR
            return MakeToken(start, TokenType::Operator);
         }


         
         ScriptToken  ScriptCommandLexer::ReadText(StringIterator start)
         {
            // Read remaining text
            while (MatchText() && ReadChar())
            {}
            
            // Return TEXT
            return MakeToken(start, TokenType::Text);
         }


         ScriptToken  ScriptCommandLexer::ReadVariable(StringIterator start)
         {
            // Dollar sign
            ReadChar();

            // Consume name
            while (MatchVariable() && ReadChar())
            {}

            // Return as TEXT if name is missing, otherwise VARIABLE
            return MakeToken(start, Position-start > 0 ? TokenType::Variable : TokenType::Number);
         }


         void  ScriptCommandLexer::ReadWhitespace()
         {
            // Consume whitespace
            while (MatchWhitespace() && ReadChar())
            {}
         }

         
      }
   }
}