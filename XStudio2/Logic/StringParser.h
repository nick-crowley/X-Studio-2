#pragma once

#include "Common.h"

namespace Logic
{
   namespace Language
   {
      /// <summary>Paragraph alignment</summary>
      enum class Alignment { Left, Centre, Right, Justify };

      /// <summary>Column spacing of a rich-text string</summary>
      enum class ColumnSpacing { Default, Single, Double, Triple };

      /// <summary>Character within a rich-text string</summary>
      class RichChar
      {
      public:
         RichChar(wchar ch, Colour c, UINT format) : Char(ch), Colour(c), Format(format)
         {}

         wchar  Char;
         UINT   Format;
         Colour Colour;
      };

      /// <summary>Paragraph of text within a rich-text string</summary>
      class RichParagraph
      {
      public:
         RichParagraph() : Align(Alignment::Left)
         {}

         list<RichChar> Text;
         Alignment      Align;
      };

      /// <summary>Result of parsing a Language string with rich-text formatting</summary>
      class RichString
      {
      public:
         list<RichParagraph> Paragraphs;
         ColumnSpacing       Spacing;
         UINT                ColumnWidth;
         wstring             Author,
                             Title;
      };
      
      /// <summary></summary>
      class StringParser
      {
         // ------------------------ TYPES --------------------------
      private:
         class TagStack : public deque<wstring>
         {
         public:
            TagStack();

            bool Matches(const wstring& tag);
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         StringParser(const wstring& str);
         virtual ~StringParser();

         DEFAULT_COPY(StringParser);	// Default copy semantics
         DEFAULT_MOVE(StringParser);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Parse();

         // -------------------- REPRESENTATION ---------------------
      public:
         RichString Output;

      private:
         wstring  Input;
         TagStack State;
      };

   }
}

using namespace Logic::Language;