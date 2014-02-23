#include "stdafx.h"
#include "RichTextRenderer.h"
#include "Logic/RtfStringWriter.h"

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

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Draw rich text in a single line</summary>
      /// <param name="dc">dc.</param>
      /// <param name="line">line rect</param>
      /// <param name="str">string.</param>
      /// <exception cref="Logic::Win32Exception">Drawing error</exception>
      void  RichTextRenderer::DrawLine(CDC* dc, CRect line, const RichString& str)
      {
         LOGFONT fontData;
         CFont* oldFont;

         // Get original font properties
         oldFont = dc->GetCurrentFont();
         if (!oldFont->GetLogFont(&fontData))
            throw Win32Exception(HERE, L"Unable to get logical font");
         
         // Draw phrases
         for (auto& p : GetPhrases(str))
         {
            // Select approprate font
            auto font = p.GetFont(fontData);
            dc->SelectObject(font.get());
            
            // Set colour
            dc->SetTextColor(p.GetColour(true));

            // Draw as much text as possible
            dc->DrawText(p.Text.c_str(), p.Text.length(), line, DT_LEFT|DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
            line.left += dc->GetTextExtent(p.Text.c_str(), p.Text.length()).cx;

            // Restore
            dc->SelectObject(oldFont);

            // EOF: Abort if we run out of space
            if (line.left > line.right)
               break;
         }
      }

      /// <summary>Draw multiple lines of rich text</summary>
      /// <param name="dc">dc.</param>
      /// <param name="rect">drawing rectangle</param>
      /// <param name="str">string.</param>
      /// <param name="flags">DrawText flags.</param>
      /// <exception cref="Logic::Win32Exception">Drawing error</exception>
      void  RichTextRenderer::DrawLines(CDC* dc, CRect& rect, const RichString& str, UINT flags)
      {
         LOGFONT  fontData;
         CFont*   oldFont;
         LineRect line(rect, dc->GetTextExtent(L"ABC").cy);
         bool     Draw = (flags & DT_CALCRECT) == 0;

         // Get original font properties
         oldFont = dc->GetCurrentFont();
         if (!oldFont->GetLogFont(&fontData))
            throw Win32Exception(HERE, L"Unable to get logical font");
         
         // Draw paragraphs
         CRect remaining(line);
         for (auto& para : str.Paragraphs)
         {
            // Draw words
            for (auto& w : GetWords(para))
            {
               // Newline:
               if (w.Text == L"\n") {
                  remaining = line.Advance();
                  continue;
               }

               // Select font/colour
               auto font = w.GetFont(fontData);
               dc->SelectObject(font.get());
               if (Draw)
                  dc->SetTextColor(w.GetColour(true));

               // Start a new line if word won't fit
               auto word = dc->GetTextExtent(w.Text.c_str(), w.Text.length());
               if (word.cx > remaining.Width())
                  remaining = line.Advance();

               // Skip leading whitespace
               if (remaining.left == line.left && w.IsWhitespace)
                  continue;

               // Draw word
               if (Draw)
                  dc->DrawText(w.Text.c_str(), w.Text.length(), remaining, DT_LEFT);
               remaining.left += word.cx;

               // Restore font
               dc->SelectObject(oldFont);
            }

            // Start each paragraph on a separate line
            remaining = line.Advance();
         }

         // Set drawing extent
         rect.bottom = line.bottom;
      }

      
      /// <summary>Convert string into a list of phrases with contiguous formatting</summary>
      /// <param name="str">string.</param>
      /// <returns>List of phrases</returns>
      RichTextRenderer::PhraseList  RichTextRenderer::GetPhrases(const RichString& str)
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
      RichTextRenderer::PhraseList  RichTextRenderer::GetWords(const RichParagraph& para)
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

            // Create new word on line/word break
            else if (ch->Char == '\n' || iswspace(ch->Char) != iswspace(current.Text.front()))
               phrases.push_back(RichPhrase(*ch));
            else
               // Otherwise append to last phrase
               current.Text.push_back(ch->Char);
         });

         // return words
         return phrases;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
