#include "stdafx.h"
#include "SyntaxHighlight.h"
#include "ScriptFile.h"
#include "ScriptToken.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Scripts
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create highlighter and populate with current colours</summary>
      SyntaxHighlight::SyntaxHighlight()
      {
         // Populate colours 
         Argument = PrefsLib.ArgumentColour;
         Constant = PrefsLib.ConstantColour;
         Colours[TokenType::UnaryOp] = PrefsLib.OperatorColour;
         Colours[TokenType::BinaryOp] = PrefsLib.OperatorColour;
         Colours[TokenType::Comment] = PrefsLib.CommentColour;
         Colours[TokenType::GameObject] = PrefsLib.GameObjectColour;
         Colours[TokenType::Keyword] = PrefsLib.KeywordColour;
         Colours[TokenType::Label] = PrefsLib.LabelColour;
         Colours[TokenType::Number] = PrefsLib.NumberColour;
         Colours[TokenType::Null] = PrefsLib.NullColour;
         Colours[TokenType::ScriptObject] = PrefsLib.ScriptObjectColour;
         Colours[TokenType::String] = PrefsLib.StringColour;
         Colours[TokenType::Text] = PrefsLib.TextColour;
         Colours[TokenType::Variable] = PrefsLib.VariableColour;
      }


      SyntaxHighlight::~SyntaxHighlight()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Get the appropriate colour for a script token.</summary>
      /// <param name="tok">token.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Whitespace token or unknown token-type</exception>
      COLORREF  SyntaxHighlight::GetColour(const Compiler::TokenType t) const
      {
         switch (t)
         {
         // Default: Lookup colour
         case TokenType::Comment:      
         case TokenType::Null: 
         case TokenType::Label:        
         case TokenType::Keyword:      
         case TokenType::Number:     
         case TokenType::String:       
         case TokenType::GameObject:   
         case TokenType::ScriptObject: 
         case TokenType::BinaryOp: 
         case TokenType::UnaryOp: 
         case TokenType::Text:  
         case TokenType::Variable:   
            return Colours.at(t);

         // Whitespace: Error
         case TokenType::Whitespace:   
            throw ArgumentException(HERE, L"tok", L"Whitespace tokens have no colour");
         }

         // Unknown: error
         throw ArgumentException(HERE, L"tok", VString(L"Unknown token type: %d", t));
      }

      /// <summary>Get the appropriate colour for a script token.</summary>
      /// <param name="script">script.</param>
      /// <param name="tok">token.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Whitespace token or unknown token-type</exception>
      COLORREF  SyntaxHighlight::GetColour(const ScriptFile& script, const ScriptToken& tok) const
      {
         // Determine colour
         switch (tok.Type)
         {
         // Variable: Distinguish between arguments/constants/variables
         case TokenType::Variable:   
            if (script.Variables.Contains(tok.ValueText))
            {
               auto& var = script.Variables[tok.ValueText];

               // Argument
               if (var.Type == VariableType::Argument)
                  return Argument;
               // Constant
               else if (var.Constant)
                  return Constant;
            }
            // Fall thru...

         // Default: Lookup colour
         case TokenType::Comment:      
         case TokenType::Null: 
         case TokenType::Label:        
         case TokenType::Keyword:      
         case TokenType::Number:     
         case TokenType::String:       
         case TokenType::GameObject:   
         case TokenType::ScriptObject: 
         case TokenType::BinaryOp: 
         case TokenType::UnaryOp: 
         case TokenType::Text:  
            return Colours.at(tok.Type);

         // Whitespace: Error
         case TokenType::Whitespace:   
            throw ArgumentException(HERE, L"tok", L"Whitespace tokens have no colour");
         }

         // Unknown: error
         throw ArgumentException(HERE, L"tok", VString(L"Unknown token type: %s", tok.Text.c_str()));
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
