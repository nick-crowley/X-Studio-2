#pragma once
#include "Logic/RichString.h"

namespace GUI
{
   namespace Utils
   {
      class RichPhrase;

      /// <summary>Drawing flags</summary>   
      enum class RenderFlags 
      { 
         RichText,   // Use richText colours
         Tooltip,    // Use tooltip colours
         Inverted,   // Use richText colours but invert black/white for a light background
         Selected,   // Render for selected background - white text with preserved formatting 
         Calculate   // Calculation only
      };

      /// <summary>Shared pointer to a font</summary>   
      typedef shared_ptr<CFont>  FontPtr;

      /// <summary>List of phrases</summary>   
      typedef list<RichPhrase>  PhraseList;

      /// <summary>Phrase list iterator</summary>   
      typedef PhraseList::iterator  PhraseIterator;
         
      /// <summary>Block of text with contiguous formatting</summary>   
      class RichPhrase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RichPhrase() : Format(NULL), Colour(Colour::Default), Skip(false), Font(nullptr)
         {}
         RichPhrase(RichCharacter ch) : Format(ch.Format), Colour(ch.Colour), Skip(false), Rect(0,0,0,0), Font(nullptr)
         {
            Text.push_back(ch.Char);
         }
         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether empty.</summary>
         /// <returns></returns>
         bool  Empty() const
         {
            return Text.empty();
         }

         /// <summary>Gets the display colour, inverting if necessary</summary>
         /// <param name="invert">invert black/white for a light background.</param>
         /// <returns>RGB value</returns>
         /// <exception cref="Logic::ArgumentException">Invalid render flag</exception>
         COLORREF  GetColour(RenderFlags flags) const
         {
            switch (flags)
            {
            // Tooltip: set Default to Black.  Use Tooltip RGBs
            case RenderFlags::Tooltip:
               if (Colour == Colour::Default)
                  return (COLORREF)ForTooltip(Colour::Black);

               return (COLORREF)ForTooltip(Colour);

            // Selected: Always white
            case RenderFlags::Selected:
               return (COLORREF)ForRichText(Colour::White);

            // Inverted: Invert black/white. Use RichText RGBs
            case RenderFlags::Inverted:
               switch (Colour)
               {
               case Colour::Default:
               case Colour::White:   
               case Colour::Silver:  return (COLORREF)ForRichText(Colour::Black);
               case Colour::Black:   return (COLORREF)ForRichText(Colour::White);
               }
               return (COLORREF)ForRichText(Colour);

            // RichText: Use RichText RGBs without conversion
            case RenderFlags::RichText:
               return (COLORREF)ForRichText(Colour);
            }
            
            throw ArgumentException(HERE, L"flags", GuiString(L"Invalid render flag %d", flags));
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
         /// <summary>Clears this instance.</summary>
         void  Clear()
         {
            Text.clear();
            Colour = Colour::Default;
            Format = NULL;
         }

         /// <summary>Offsets the drawing rectangle horizontally.</summary>
         /// <param name="cx">Distance.</param>
         void  Offset(int cx)
         {
            Rect.OffsetRect(cx, 0);
         }

         /// <summary>Appends a character to the phrase.</summary>
         RichPhrase& operator+=(wchar ch)
         {
            Text += ch;
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring Text;        // Text
         UINT    Format;      // Character formatting
         Colour  Colour;      // Colour
         CRect   Rect;        // Rendering rectangle
         bool    Skip;        // Whether to skip rendering of this token  [whitespace at start of line]
         FontPtr Font;        // Font used for rendering
      };

      /// <summary></summary>
      class RichTextRenderer
      {
         // ------------------------ TYPES --------------------------
      public:
         

      private:
         /// <summary>Collection of recently used fonts, used for optimizing drawing</summary>   
         class FontMap : public map<UINT, FontPtr>
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
      private:
         RichTextRenderer();
         virtual ~RichTextRenderer();

         NO_COPY(RichTextRenderer);	// No copy semantics
         NO_MOVE(RichTextRenderer);	// No move semantics

         // ------------------------ STATIC -------------------------
      public:
         static void  DrawLine(CDC* dc, CRect line, const RichString& str, RenderFlags flags);
         static int   DrawLines(CDC* dc, CRect& rect, const RichString& str, RenderFlags flags);

      protected:
         static PhraseList  GetPhrases(const RichString& str);
         static PhraseList  GetWords(const RichParagraph& para);
         static int         MeasureLine(CDC* dc, PhraseIterator& start, const PhraseIterator& end, const CRect& line);
         static void        RenderLine(CDC* dc, const PhraseIterator& pos, const PhraseIterator& end, RenderFlags flags);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace GUI::Utils;
