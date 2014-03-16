#pragma once


#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Types of tokens produced by the script lexer</summary>
         enum class TokenType { Text, Number, String, GameObject, ScriptObject, Keyword, Variable, Null, Label, BinaryOp, UnaryOp, Comment, Whitespace };

         /// <summary>Get token type string</summary>
         GuiString  GetString(TokenType t);

         /// <summary>Common base class for all token types</summary>
         class TokenBase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            TokenBase() : Start(0), End(0)
            {}
            TokenBase(UINT s, UINT e) : Start(s), End(e)
            {}
            virtual ~TokenBase()
            {}

            DEFAULT_COPY(TokenBase);	// Default copy semantics
            DEFAULT_MOVE(TokenBase);	// Default move semantics
            
            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(UINT,Length,GetLength);

            // ---------------------- ACCESSORS ------------------------			
         public:
            bool  Contains(UINT pos) const  { return pos >= Start && pos < End; }
            UINT  GetLength() const         { return End - Start; }

            // -------------------- REPRESENTATION ---------------------
         public:
            const UINT  Start, 
                        End;
         };

         /// <summary>Token produced by MSCI script command lexer</summary>
         class ScriptToken : public TokenBase
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptToken() : Type(TokenType::Whitespace)
            {}
            ScriptToken(TokenType t, UINT s, UINT e, const wstring& txt) : TokenBase(s, e), Type(t), Text(txt)
            {}
            virtual ~ScriptToken()
            {}

            DEFAULT_COPY(ScriptToken);	// Default copy semantics
            DEFAULT_MOVE(ScriptToken);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(wstring,ValueText,GetValueText);

            // ---------------------- ACCESSORS ------------------------			

            /// <summary>Gets the text without any formatting operators, eg. (,{,',$ etc.</summary>
            /// <returns></returns>
            wstring GetValueText() const
            {
               switch (Type)
               {
               case TokenType::GameObject:
               case TokenType::ScriptObject:
               case TokenType::String:
                  return SubString(1, Length-2);

               case TokenType::Variable:
                  return SubString(1, Length-1);

               default:
                  return Text;
               }
            }

            /// <summary>Query whether token represents a parameter.</summary>
            /// <returns></returns>
            bool  IsParameter() const
            {
               switch (Type)
               {
               case TokenType::BinaryOp:  
               case TokenType::UnaryOp:  
               case TokenType::Keyword:  
               case TokenType::Text:      
                  return false;

               default: 
                  return true;
               }
            }

            /// <summary>Get substring of text</summary>
            /// <param name="offset">The starting offset.</param>
            /// <param name="len">The length</param>
            /// <returns></returns>
            wstring SubString(UINT offset, UINT len) const
            {
               return Text.substr(offset, len);
            }

            /// <summary>Compare two tokens</summary>
            bool operator==(const ScriptToken& r) const
            {
               return Type==r.Type && Text==r.Text && Start==r.Start && End==r.End;
            }
            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         public:
            const TokenType  Type;
            const wstring    Text;
         };

         /// <summary>Write script token to the console</summary>
         ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptToken& tok);

         /// <summary>Vector of script tokens</summary>
         class TokenArray : public vector<ScriptToken>
         {
         public:
            TokenArray() 
            {}

            /// <summary>Get number of non-whitespace tokens</summary>
            /// <returns></returns>
            size_type count() const
            {
               return count_if(cbegin(), cend(), [](const ScriptToken& t) {return t.Type != TokenType::Whitespace;} );
            }
            
            /// <summary>Finds the token at a character index.</summary>
            /// <param name="pos">character index.</param>
            /// <returns>Token if found, otherwise nullptr</returns>
            const ScriptToken* Find(UINT pos) const
            {
               auto tok = find_if(cbegin(), cend(), [pos](const ScriptToken& t) {return t.Contains(pos);} );
               return tok != cend() ? &*tok : nullptr;
            }

            /// <summary>Append token</summary>
            TokenArray& operator+=(const ScriptToken& tok)
            {
               push_back(tok);
               return *this;
            }

         };

         /// <summary>List of script tokens</summary>
         typedef list<ScriptToken> TokenList;

         /// <summary>Script token array iterator</summary>
         typedef TokenArray::const_iterator  TokenIterator;
      }
   }
}

using namespace Logic::Scripts::Compiler;
