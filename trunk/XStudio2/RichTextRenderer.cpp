#include "stdafx.h"
#include "RichTextRenderer.h"


namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      RichTextRenderer::RichTextRenderer()
      {
      }


      RichTextRenderer::~RichTextRenderer()
      {
      }

      // ---------------------------- PUBLIC STATIC METHODS ---------------------------

      /// <summary>Draw rich text in a single line</summary>
      /// <param name="dc">dc.</param>
      /// <param name="line">line rect</param>
      /// <param name="str">string.</param>
      /// <param name="flags">How to modify colours for light/dark backgrounds.</param>
      /// <exception cref="Logic::Win32Exception">Drawing error</exception>
      void  RichTextRenderer::DrawLine(CDC* dc, CRect line, const RichString& str, RenderFlags flags)
      {
         // Separate into phrases
         auto words = GetPhrases(str);

         // Measure phrases
         auto first = words.begin(), last = first;
         int line_remaining = MeasureLine(dc, last, words.end(), line);

         // Alignment: Offset all word rectangles
         for (auto w = first; w != last; ++w)
         {
            switch (str.FirstParagraph.Align)
            {
            case Alignment::Right:   w->Offset(line_remaining);    break;
            case Alignment::Centre:
            case Alignment::Justify: w->Offset(line_remaining/2);  break;
            }
         }

         // Render words
         RenderLine(dc, first, last, flags);
      }

      /// <summary>Draw multiple lines of rich text</summary>
      /// <param name="dc">dc.</param>
      /// <param name="rect">drawing rectangle</param>
      /// <param name="str">string.</param>
      /// <param name="flags">drawing flags.</param>
      /// <exception cref="Logic::Win32Exception">Drawing error</exception>
      /// <returns>Width of widest line</returns>
      int  RichTextRenderer::DrawLines(CDC* dc, CRect& rect, const RichString& str, RenderFlags flags)
      {
         LOGFONT  fontData;
         CFont*   oldFont;
         LineRect line(rect, dc->GetTextExtent(L"ABC").cy);
         long     rightMargin = rect.left;

         // Get original font properties
         oldFont = dc->GetCurrentFont();
         if (!oldFont->GetLogFont(&fontData))
            throw Win32Exception(HERE, L"Unable to get logical font");
         
         // Draw paragraphs
         for (auto para = str.Paragraphs.begin(); para != str.Paragraphs.end(); )
         {
            auto words(GetWords(*para));

            // Draw words
            for (auto w = words.begin(); w != words.end(); )
            {
               CRect remaining(line);

               // Measure words on line
               auto first = w;
               int line_remaining = MeasureLine(dc, w, words.end(), line);

               // Alignment: Offset all word rectangles
               for (auto word = first; word != w; ++word)
               {
                  switch (para->Align)
                  {
                  case Alignment::Right:   word->Offset(line_remaining);    break;
                  case Alignment::Centre:
                  case Alignment::Justify: word->Offset(line_remaining/2);  break;
                  }

                  // Set rightmost margin
                  rightMargin = max(rightMargin, word->Rect.right);
               }

               // Render words
               if (flags != RenderFlags::Calculate)
                  RenderLine(dc, first, w, flags);

               // NewLine
               if (w != words.end())
                  line.Advance();
            }

            // Start each paragraph on a separate line
            if (++para != str.Paragraphs.end())
               line.Advance();
         }

         // Set drawing extent
         rect.bottom = (!str.FirstParagraph.empty() ? line.bottom : line.top);

         // Return width of widest line of text
         return rightMargin - rect.left;
      }


      // -------------------------- PROTECTED STATIC METHODS -------------------------------

      /// <summary>Calculates the amount of words that will fit in the line rectangle, and sets their word rectangles</summary>
      /// <param name="dc">The dc.</param>
      /// <param name="start">position of first word, this will be advanced to beyond the last word.</param>
      /// <param name="end">end of words.</param>
      /// <param name="line">line rectangle.</param>
      /// <returns>Line length remaining</returns>
      int  RichTextRenderer::MeasureLine(CDC* dc, PhraseIterator& start, const PhraseIterator& end, const CRect& line)
      {
         LOGFONT fontData;
         CRect   remaining(line);
         PhraseIterator w;

         // Get original font properties
         CFont* oldFont = dc->GetCurrentFont();
         oldFont->GetLogFont(&fontData);

         // Determine total length of words on this line
         bool firstWord = true;
         for (w = start; w != end; ++w)
         {
            // Newline: Consume but don't measure
            if (w->Text == L"\n") {
               w->Skip = true;
               ++w;
               break;
            }

            // Create word font
            w->Font = w->GetFont(fontData);
            dc->SelectObject(w->Font.get());
            // Measure word
            auto word = w->GetSize(dc);
            dc->SelectObject(oldFont);

            // EndOfLine: stop      // SpecialCase: Punctuation - never start new line with punctuation
            if (word.cx > remaining.Width()) // && !iswpunct(w->Text.front()))    
               break;

            // Skip whitespace at start of line
            if (firstWord && w->IsWhitespace())
               w->Skip = true;
            else
            {
               // Set word rectangle
               w->Rect.SetRect(remaining.left, remaining.top, remaining.left + word.cx, remaining.bottom);

               // Include in measurement
               remaining.left += word.cx;
               firstWord = false;
            }
         }

         // Check for words too long to fit on a line
         if (w == start)
            throw AlgorithmException(HERE, GuiString(L"Unable to fit '%s' on a single line", start->Text.c_str()));

         // Consume words + Return remaining line length
         start = w;
         return remaining.Width();
      }
      

      /// <summary>Renders previously measured words.</summary>
      /// <param name="dc">The dc.</param>
      /// <param name="pos">position of first word.</param>
      /// <param name="end">position after the last word.</param>
      void  RichTextRenderer::RenderLine(CDC* dc, const PhraseIterator& pos, const PhraseIterator& end, RenderFlags flags)
      {
         LOGFONT fontData;

         // Get original font properties
         CFont* oldFont = dc->GetCurrentFont();
         oldFont->GetLogFont(&fontData);

         // Draw words
         for (PhraseIterator w = pos; w != end; ++w)
         {
            // Newline: Consume but don't draw
            if (w->Text == L"\n") {
               ++w;
               break;
            }
            // Skip leading whitespace
            else if (w->Skip)
               continue;

            // Select font/colour
            dc->SelectObject(w->Font.get());
            dc->SetTextColor(w->GetColour(flags));
            dc->SelectObject(oldFont);

            // DEBUG:
            //Console << Cons::Yellow << GuiString(L"RenderLine: {'%s', %d}", w->Text.c_str(), w->Rect.Width()) << ENDL;

            // Draw word
            dc->SelectObject(w->Font.get());
            dc->DrawText(w->Text.c_str(), w->Text.length(), w->Rect, DT_LEFT);
            dc->SelectObject(oldFont);
         }
      }
      

      
      /// <summary>Convert string into a list of phrases with contiguous formatting</summary>
      /// <param name="str">string.</param>
      /// <returns>List of phrases</returns>
      PhraseList  RichTextRenderer::GetPhrases(const RichString& str)
      {
         RichCharList chars;

         // Flatten string into list of characters
         for (auto& para : str.Paragraphs)
            for (auto ch : para.Characters)
               if (ch->Char != '\n')      // Drop line breaks
                  chars.push_back(ch);

         // Abort if empty string
         if (chars.empty())
            return PhraseList();

         PhraseList phrases;
         phrases.push_back(RichPhrase(*chars.front()));

         // Transform into blocks of contiguous characters
         for_each(++chars.begin(), chars.end(), [&](const RichCharacter* ch) 
         {
            // Create new phrase if colour/formatting changes
            if (ch->Colour != phrases.back().Colour
             || ch->Format != phrases.back().Format)
                phrases.push_back(RichPhrase(*ch));
            else
               // Otherwise append to last phrase
               phrases.back().Text.push_back(ch->Char);
         });

         return phrases;
      }

      /// <summary>Convert string into a list of words/whitespace with contiguous formatting</summary>
      /// <param name="str">string.</param>
      /// <returns>List of words/whitespace with contiguous formatting</returns>
      PhraseList  RichTextRenderer::GetWords(const RichParagraph& para)
      {
         RichCharList chars;

         // Flatten paragraph into list of characters
         for (auto ch : para.Characters)
            chars.push_back(ch);

         // Abort if empty string
         if (chars.empty())
            return PhraseList();

         // Get first char
         PhraseList phrases;
         phrases.push_back(RichPhrase(*chars.front()));

         // Transform into blocks of contiguous characters
         for_each(++chars.begin(), chars.end(), [&](const RichCharacter* ch) 
         {
            RichPhrase& current = phrases.back();

            // Create new word on colour/formatting change
            if (ch->Colour != current.Colour
             || ch->Format != current.Format)
                phrases.push_back(RichPhrase(*ch));

            // Create new word on punctuation, line-break and word-break
            else if (/*iswpunct(ch->Char) ||*/ ch->Char == '\n' || iswspace(ch->Char) != iswspace(current.Text.front()))
               phrases.push_back(RichPhrase(*ch));
            else
               // Otherwise append to last phrase
               current.Text.push_back(ch->Char);
         });

         // return words
         return phrases;
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
