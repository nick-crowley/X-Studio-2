
#pragma once
#include "Logic/RichString.h"
#include "Logic/RtfStringWriter.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class RichTextRenderer
      {
         // ------------------------ TYPES --------------------------
      private:
         class RichPhrase;

         /// <summary>Shared pointer to a font</summary>   
         typedef shared_ptr<CFont>  FontPtr;

         /// <summary>List of phrases</summary>   
         typedef list<RichPhrase>  PhraseList;

         typedef PhraseList::iterator  PhraseIterator;

         /// <summary>Block of text with contiguous formatting</summary>   
         class RichPhrase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            RichPhrase(RichCharacter ch) : Format(ch.Format), Colour(ch.Colour), Skip(false), Rect(0,0,0,0)
            {
               Text.push_back(ch.Char);
            }
            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Gets the RGB colour</summary>
            /// <param name="invert">invert black/white for a dark background.</param>
            /// <returns></returns>
            COLORREF  GetColour(bool invert) const
            {
               Logic::Colour col = Colour;

               // Invert black/white
               if (invert)
                  switch (Colour)
                  {
                  case Colour::Default:
                  case Colour::White:   
                  case Colour::Silver:  col = Colour::Black;  break;
                  case Colour::Black:   col = Colour::White;  break;
                  }
               
               // Set colour
               return RtfStringWriter::ToRGB(col);
            }

            /// <summary>Creates a new font to represent this phrase</summary>
            /// <param name="from">Basis for new font.</param>
            /// <returns></returns>
            FontPtr GetFont(const LOGFONT& from) const
            {
               FontPtr f = FontPtr(new CFont());
               LOGFONT lf = from;

               // Convert formatting into properties
               lf.lfItalic = (Format & CFE_ITALIC ? TRUE : FALSE);
               lf.lfWeight = (Format & CFE_BOLD ? FW_BOLD : FW_NORMAL);
               lf.lfUnderline = (Format & CFE_UNDERLINE ? TRUE : FALSE);

               // Create font
               f->CreateFontIndirectW(&lf);
               return f;
            }

            /// <summary>Gets the text extent.</summary>
            /// <param name="pDC">The dc.</param>
            /// <returns></returns>
            CSize  GetSize(CDC* pDC) const
            {
               return pDC->GetTextExtent(Text.c_str(), Text.length());
            }

            /// <summary>Determines whether text is whitespace.</summary>
            /// <returns></returns>
            bool IsWhitespace() const
            {
               for (auto ch : Text)
                  if (!iswspace(ch))  
                     return false;

               return true;
            }

            // ----------------------- MUTATORS ------------------------
         public:
            void  Offset(int cx) 
            {
               Rect.OffsetRect(cx, 0);
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            wstring Text;
            UINT    Format;
            Colour  Colour;
            CRect   Rect;
            bool    Skip;
            FontPtr Font;
         };

         /// <summary>Line rectangle</summary>   
         class LineRect : public CRect
         {
         public:
            LineRect(const CRect& page, int lineHeight) 
               : CRect(page.left, page.top, page.right, page.top+lineHeight), LineHeight(lineHeight)
            {}

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
         static void  DrawLine(CDC* dc, CRect line, const RichString& str);
         static int   DrawLines(CDC* dc, CRect& rect, const RichString& str, UINT flags);

      protected:
         static PhraseList  GetPhrases(const RichString& str);
         static PhraseList  GetWords(const RichParagraph& para);
         static int         MeasureLine(CDC* dc, PhraseIterator& start, const PhraseIterator& end, const CRect& line);
         static void        RenderLine(CDC* dc, const PhraseIterator& pos, const PhraseIterator& end);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace GUI::Utils;
