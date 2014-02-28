#pragma once
#include "Logic/RichString.h"
#include <strsafe.h>

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
      class PhraseList : public list<RichPhrase> 
      {
      public:
         PhraseList() 
         {}

         /// <summary>Append a phrase</summary>   
         PhraseList& operator+=(RichPhrase& p)
         {
            push_back(p);
            return *this;
         }
      };

      /// <summary>Phrase list iterator</summary>   
      typedef PhraseList::iterator  PhraseIterator;
         
      /// <summary>Block of text with contiguous formatting</summary>   
      class RichPhrase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty phrase</summary>
         RichPhrase() : Format(NULL), Colour(Colour::Default), Skip(false), Rect(0,0,0,0), Font(nullptr)
         {}

         /// <summary>Create phrase with formatting determined by first character</summary>
         /// <param name="ch">First character</param>
         RichPhrase(RichCharacter ch) : Format(ch.Format), Colour(ch.Colour), Skip(false), Rect(0,0,0,0), Font(nullptr)
         {
            Text.push_back(ch.Char);
         }

         /// <summary>Manually specify content and formatting of entire phrase</summary>
         /// <param name="txt">text.</param>
         /// <param name="format">formatting.</param>
         /// <param name="col">Colour</param>
         RichPhrase(const wstring& txt, UINT format, Colour col) : Text(txt), Format(format), Colour(col), Skip(false), Rect(0,0,0,0), Font(nullptr)
         {}

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

            // Custom font:
            if (!CustomFont.empty())
               StringCchCopy(lf.lfFaceName, LF_FACESIZE, CustomFont.c_str());

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

         /// <summary>Offsets the drawing rectangle .</summary>
         /// <param name="cx">Distance.</param>
         /// <param name="cy">Distance.</param>
         void  Offset(int cx, int cy)
         {
            Rect.OffsetRect(cx, cy);
         }

         /// <summary>Truncates the phrase to desired width.</summary>
         /// <param name="dc">The dc.</param>
         /// <param name="width">The width.</param>
         /// <returns>New size</returns>
         CSize Truncate(CDC* dc, int width)
         {
            CSize maxSize;
            int maxChars = 0;

            // Get max number of characters that we can fit
            if (!::GetTextExtentExPointW(dc->m_hDC, Text.c_str(), Text.length(), width, &maxChars, nullptr, &maxSize))
               throw Win32Exception(HERE, L"Unable to measure text extent");
            
            // Truncate
            if (maxChars > 0)
               Text.replace(Text.begin()+maxChars, Text.end(), L"...");
                  
            return GetSize(dc);
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
         wstring CustomFont;  // [optional] Name of font used for rendering
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
         static int         MeasureLine(CDC* dc, PhraseIterator& start, const PhraseIterator& end, const CRect& line, RenderFlags flags);
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
