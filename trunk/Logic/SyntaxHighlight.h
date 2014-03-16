#pragma once


namespace Logic
{
   namespace Scripts
   {
      /// <summary>Forward declarations</summary>
      class ScriptFile;
      FORWARD_DECLARATION(Compiler,class ScriptToken)
      FORWARD_DECLARATION(Compiler,enum class TokenType)

      /// <summary>Provides a lookup table for determining appropriate script token colours</summary>
      class SyntaxHighlight
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Colour map</summary>
         typedef map<Compiler::TokenType,COLORREF>  ColourMap;

         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxHighlight();
         virtual ~SyntaxHighlight();

         DEFAULT_COPY(SyntaxHighlight);	// Default copy semantics
         DEFAULT_MOVE(SyntaxHighlight);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         COLORREF  GetColour(const Compiler::TokenType t) const;
         COLORREF  GetColour(const ScriptFile& script, const Compiler::ScriptToken& tok) const;

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      protected:
         ColourMap  Colours;
         COLORREF   Argument;
      };

   }
}

using namespace Logic::Scripts;
