
#pragma once
#include "Logic/RichString.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class RichTextRenderer
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary>Drawing flags</summary>   
         enum class Flags { None, Inverted, Selected };

      private:
         /// <summary>Collection of recently used fonts, used for optimizing drawing</summary>   
         class FontMap : public map<UINT, Logic::Language::FontPtr>
         {
         public:
            FontMap()
            {}

            /// <summary>Check whether map contains a font of given formatting</summary>
            /// <param name="f">Formatting</param>
            /// <returns></returns>
            bool Contains(UINT f)
            {
               return find(f) != end();
            }
         };

         /// <summary>Line rectangle</summary>   
         class LineRect : public CRect
         {
         public:
            /// <summary>Creates a line rectangle</summary>
            /// <param name="page">Total drawing area.</param>
            /// <param name="lineHeight">Height of a line.</param>
            LineRect(const CRect& page, int lineHeight)
               : CRect(page.left, page.top, page.right, page.top+lineHeight), LineHeight(lineHeight)
            {}

            /// <summary>Advances rectangle to the next line.</summary>
            /// <returns>New line rectangle</returns>
            const CRect& Advance()
            {
               OffsetRect(0, LineHeight);
               return *this;
            }

         private:
            int  LineHeight;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         RichTextRenderer();
         virtual ~RichTextRenderer();

         DEFAULT_COPY(RichTextRenderer);	// Default copy semantics
         DEFAULT_MOVE(RichTextRenderer);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static void  DrawLine(CDC* dc, CRect line, const RichString& str, Flags flags = Flags::None);
         static int   DrawLines(CDC* dc, CRect& rect, const RichString& str, UINT flags);

      protected:
         static PhraseList  GetPhrases(const RichString& str);
         static PhraseList  GetWords(const RichParagraph& para);
         static int         MeasureLine(CDC* dc, PhraseIterator& start, const PhraseIterator& end, const CRect& line);
         static void        RenderLine(CDC* dc, const PhraseIterator& pos, const PhraseIterator& end, Flags flags);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace GUI::Utils;
