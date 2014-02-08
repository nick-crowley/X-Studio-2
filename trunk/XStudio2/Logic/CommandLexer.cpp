#include "stdafx.h"
#include "CommandLexer.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         CommandLexer::CommandLexer(const wstring& line, bool  skipWhitespace) 
            : LineStart(line.begin()), LineEnd(line.end()), Position(LineStart), SkipWhitespace(skipWhitespace), Input(line), Tokens(Parse())
         {
            // DEBUG:
            /*Console.WriteLnf(L"\nLexing command: %s", line.c_str());
            for (const ScriptToken& t : Tokens)
               Console.WriteLnf(L"Token: '%s'", t.Text.c_str());
            Console.WriteLnf();*/
         }


         CommandLexer::~CommandLexer()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         /// <summary>Parses all the input text</summary>
         /// <returns>Token array</returns>
         TokenArray CommandLexer::Parse()
         {
            TokenArray tokens;

            // Read until EOL
            while (ValidPosition)
            {
               // Whitespace: Skip
               if (SkipWhitespace && MatchWhitespace()) 
                  ReadWhitespace(Position);

               // Whitespace: Read
               else if (!SkipWhitespace && MatchWhitespace())
                  tokens.push_back( ReadWhitespace(Position) );

               // Comment: 
               else if (tokens.count() == 1 && tokens[0].Text == L"*")
                  tokens.push_back( ReadComment(Position) );

               // Number:
               else if (MatchNumber())
                  tokens.push_back( ReadNumber(Position) );      

               // Remainder: 
               else switch (*Position)
               {
               case L'$':   tokens.push_back( ReadVariable(Position) );    break;
               case L'{':   tokens.push_back( ReadGameObject(Position) );  break;
               case L'\'':  tokens.push_back( ReadString(Position) );      break;

               default: 
                  tokens.push_back( ReadAmbiguous(Position, tokens) );   
                  break;
               }
            }

            return tokens;
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
         
         /// <summary>Creates a token from text between 'start' and current position</summary>
         /// <param name="start">The start.</param>
         /// <param name="t">Token type</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::MakeToken(CharIterator start, TokenType t) const
         {
            return ScriptToken(t, start-LineStart, Position-LineStart, wstring(start, Position));
         }

         /// <summary>Check if current char matches a character</summary>
         /// <param name="ch">The character to test</param>
         /// <returns></returns>
         bool  CommandLexer::MatchChar(WCHAR ch) const
         {
            return ValidPosition && *Position == ch;
         }

         /// <summary>Check if current position matches a string (CASE SENSITIVE)</summary>
         /// <param name="str">The string to test</param>
         /// <returns></returns>
         bool  CommandLexer::MatchChars(const WCHAR* str) const
         {
            // Match each char
            for (UINT i = 0; str[i]; ++i)
               if (Position+i >= LineEnd || Position[i] != str[i])
                  return false; // EOL/Mismatch

            // Match
            return true;
         }

         /// <summary>Check if current position matches a string (CASE SENSITIVE)</summary>
         /// <param name="pos">The position to test</param>
         /// <param name="str">The string to test</param>
         /// <returns></returns>
         bool  CommandLexer::MatchChars(const CharIterator pos, const WCHAR* str) const
         {
            // Match each char
            for (UINT i = 0; str[i]; ++i)
               if (pos+i >= LineEnd || pos[i] != str[i])
                  return false; // EOL/Mismatch

            // Match
            return true;
         }

         /// <summary>Check if current char is the opening bracket of a script object</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchConstant() const
         {
            // Match: '[', alpha
            return ValidPosition && MatchChar(L'[') && Position+1 <= LineEnd && iswalpha(Position[1]);
         }

         /// <summary>Check if current character is a digit</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchDigit() const
         {
            return ValidPosition && iswdigit(*Position);
         }

         /// <summary>Check if current character is a +ve/-ve number</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchNumber() const
         {
            return MatchDigit() || MatchChar(L'-') && Position+1 <= LineEnd && iswdigit(Position[1]);  // minus immediately preceeding digit
         }

         /// <summary>Check if current char is an operator</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchOperator() const
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
               // (Mine)
               case L'%':
               // (Invalid syntax)
               case L'}':
                  return true;

               // and/or/mod
               case L'A':  return MatchChars(L"AND");
               case L'O':  return MatchChars(L"OR");
               case L'M':  return MatchChars(L"MOD");
               }

            return false;
         }

         
         /// <summary>Check if current char is text.</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchText() const
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
               // (mine)
               case L'%':
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
         bool  CommandLexer::MatchVariable() const
         {
            // Match alphanumeric/dot/underscore  (nb: not dollar)
            return ValidPosition && (iswalnum(*Position) || MatchChar(L'.') || MatchChar(L'_'));
         }

         /// <summary>Check if current char is whitespace</summary>
         /// <returns></returns>
         bool  CommandLexer::MatchWhitespace() const
         {
            // Match whitespace
            return ValidPosition && iswspace(*Position);
         }

         /// <summary>Consume character and move to the next, if available</summary>
         /// <returns>True if found, false if end-of-line</returns>
         bool  CommandLexer::ReadChar()
         {
            return ValidPosition && ++Position < LineEnd;
         }
         
         /// <summary>Reads a ScriptObject, Operator or Text, some of which share first letters</summary>
         /// <param name="start">Current position (first character)</param>
         /// <param name="prev">Tokens previously parsed</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::ReadAmbiguous(CharIterator start, const TokenArray& prev)
         {
            // Constant: Avoid interpreting '[' as an operator
            if (MatchConstant())
               return ReadConstant(start);

            // Operator: 
            if (MatchOperator())
               return ReadOperator(start);

            // Text: anything else 
            return ReadText(start, prev);
         }

         /// <summary>Reads a script object</summary>
         /// <param name="start">Current position (opening bracket)</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::ReadConstant(CharIterator start)
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
         ScriptToken  CommandLexer::ReadComment(CharIterator start)
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
         ScriptToken  CommandLexer::ReadGameObject(CharIterator start)
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
         ScriptToken  CommandLexer::ReadString(CharIterator start)
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

         /// <summary>Reads the positive or negative number</summary>
         /// <param name="start">Current position (leading digit or negative sign)</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::ReadNumber(CharIterator start)
         {
            // Consume first digit or negative operator
            ReadChar();

            // Consume digits
            while (MatchDigit() && ReadChar())
            {}
            
            // Return NUMBER
            return MakeToken(start, TokenType::Number);
         }

         /// <summary>Reads an operator</summary>
         /// <param name="start">Current position (first character)</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::ReadOperator(CharIterator start) 
         {
            // Read single char 
            switch (*Position)
            {
            // BINARY
            case L'+':
            case L'*':
            case L'/':
            case L'^':
            case L'%':
               ReadChar();
               return MakeToken(start, TokenType::BinaryOp);

            // UNARY
            case L'(':
            case L')':  
            case L'~': 
            // Used by commands, class as unary
            case L',': 
            case L':':
            case L'[':
            case L']':
            // Invalid placement
            case L'}':
               ReadChar();
               return MakeToken(start, TokenType::UnaryOp);

            // BINARY: Read multi-char operators
            default:
               switch (*Position)
               {
               // CUSTOM AND/OR: Bitwise/Logical and/or
               case L'&':
               case L'|':
                  ReadChar();
                  // Read two characters if they're the same
                  if (MatchChar(Position[-1]))
                     ReadChar();
                  return MakeToken(start, TokenType::BinaryOp);

               // RefObj/Subtract/Minus
               case L'-':  
                  ReadChar();               
                  // Dereference operator
                  if (MatchChar('>'))              // NB: '-' is always returned as binary subtract  (unary minus is identified by parser)
                     ReadChar();
                  break;

               // Comparisons
               case L'<':  // < or <=
               case L'>':  // > or >=
               case L'!':  // ! or !=
               case L'=':  // = or ==
                  ReadChar();
                  // Read comparison if present
                  if (MatchChar('='))
                     ReadChar();
                  
                  // Logical Not: Return as unary operator
                  else if (Position[-1] == '!')
                     return MakeToken(start, TokenType::UnaryOp);
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

            // Return BINARY
            return MakeToken(start, TokenType::BinaryOp);
         }


         
         /// <summary>Reads a text/keyword/label/null token</summary>
         /// <param name="start">Starting position (first character)</param>
         /// <param name="prev">Tokens previously parsed</param>
         /// <returns></returns>
         ScriptToken  CommandLexer::ReadText(CharIterator start, const TokenArray& prev)
         {
            bool Keyword = false;

            // Read remaining text
            while (MatchText() && ReadChar())
            {}

            // LABEL: first token, followed by ':'
            if (MatchChar(L':') && prev.count() == 0)
               return MakeToken(start, TokenType::Label);

            // GOTO LABEL: second token, preceeded by goto/gosub
            if (prev.count() == 1 && (prev[0].Text == L"goto" || prev[0].Text == L"gosub"))
               return MakeToken(start, TokenType::Label);

            // NULL: 
            if (MatchChars(start, L"null"))
               return MakeToken(start, TokenType::Null);

            // Identify keywords
            switch (Position - start)
            {
            case 2: Keyword = MatchChars(start, L"if") || MatchChars(start, L"do");             break;
            case 3: Keyword = MatchChars(start, L"end") || MatchChars(start, L"not");           break;
            case 4: Keyword = MatchChars(start, L"else") || MatchChars(start, L"skip") || MatchChars(start, L"goto");   break;
            case 5: Keyword = MatchChars(start, L"while") || MatchChars(start, L"break") || MatchChars(start, L"gosub") || MatchChars(start, L"start"); break;
            case 6: Keyword = MatchChars(start, L"return") || MatchChars(start, L"endsub");     break;
            case 8: Keyword = MatchChars(start, L"continue");                                   break;
            }
            
            // Return KEYWORD/TEXT
            return MakeToken(start, Keyword ? TokenType::Keyword : TokenType::Text);
         }


         ScriptToken  CommandLexer::ReadVariable(CharIterator start)
         {
            // Dollar sign
            ReadChar();

            // Consume name
            while (MatchVariable() && ReadChar())
            {}

            // Return as OPERATOR if name is missing, otherwise VARIABLE
            return MakeToken(start, Position-start > 0 ? TokenType::Variable : TokenType::UnaryOp);
         }


         ScriptToken  CommandLexer::ReadWhitespace(CharIterator start)
         {
            // Consume whitespace
            while (MatchWhitespace() && ReadChar())
            {}

             // Return as WHITESPACE
            return MakeToken(start, TokenType::Whitespace);
         }

         
      }
   }
}