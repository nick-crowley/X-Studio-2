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
      void  RichTextRenderer::DrawLine(CDC* dc, CRect line, const RichString& str)
      {
         RichCharList chars;

         // Flatten string into list of characters
         for (auto& para : str.Paragraphs)
            for (auto ch : para.Characters)
               if (ch->Char != '\n')
                  chars.push_back(ch);

         // Abort if empty string
         if (chars.empty())
            return;

         list<RichPhrase> phrases;
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

         LOGFONT fontData;
         CFont* oldFont;

         // Get original font properties
         oldFont = dc->GetCurrentFont();
         if (!oldFont->GetLogFont(&fontData))
            throw Win32Exception(HERE, L"Unable to get logical font");
         
         // Draw phrases
         for (auto& block : phrases)
         {
            // Select approprate font
            auto font = shared_ptr<CFont>(block.GetFont(fontData));
            dc->SelectObject(font.get());
            
            // Invert black/white
            Colour col = block.Colour;
            switch (block.Colour)
            {
            case Colour::Default:
            case Colour::White:   
            case Colour::Silver:  col = Colour::Black;  break;
            case Colour::Black:   col = Colour::White;  break;
            }
            // Set colour
            dc->SetTextColor(RtfStringWriter::ToRGB(col));

            // Draw as much text as possible
            dc->DrawText(block.Text.c_str(), block.Text.length(), line, DT_LEFT|DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
            line.left += dc->GetTextExtent(block.Text.c_str(), block.Text.length()).cx;

            // Restore
            dc->SelectObject(oldFont);

            // EOF: Abort if we run out of space
            if (line.left > line.right)
               break;
         }
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
