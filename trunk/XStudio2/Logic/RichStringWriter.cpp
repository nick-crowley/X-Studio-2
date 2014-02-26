#include "stdafx.h"
#include "RichStringWriter.h"
#include "RtfStringWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a rich text source code writer for a TOM text document</summary>
      /// <param name="doc">The document.</param>
      /// <param name="tags">Type of colour tags to use</param>
      /// <exception cref="Logic::ArgumentNullException">Document is null</exception>
      RichStringWriter::RichStringWriter(TextDocumentPtr& doc, ColourTag tags) : Input(doc), ColourTags(tags)
      {
         REQUIRED(doc);
      }


      RichStringWriter::~RichStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Generates richText source code for the input document</summary>
      /// <returns></returns>
      wstring  RichStringWriter::Write()
      {
         try
         {
            TextRangePtr chr = Input->Range(0, 1),
                         prev = Input->Range(0, 1);

            // Write initial state
            WriteState(chr, true);

            // Examine each character
            for (int i = 1, end = Input->Range(0, 0)->EndOf(TOM::tomStory, 1); i < end; ++i)
            {
               // Advance character
               chr->Move(TOM::tomCharacter, 1);

               // Write character
               WriteChar(chr, prev);

               // Advance previous
               prev->Move(TOM::tomCharacter, 1);
            }

            // Write Closing state
            WriteState(chr, false);
            return Output;
         }
         catch (_com_error& e) {
            throw ComException(HERE, e);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Gets the colour of a character.</summary>
      /// <param name="f">font data.</param>
      /// <returns></returns>
      Colour  RichStringWriter::GetColour(TextFontPtr& f)
      {
         return RtfStringWriter::FromRGB(f->ForeColor);
      }

      /// <summary>Writes the character.</summary>
      /// <param name="ch">The ch.</param>
      void  RichStringWriter::WriteChar(wchar ch)
      {
         switch (ch)
         {
         case '\r':  break;
         case '\v':  Output += '\n';  break;
         default:    Output += ch;    break;
         }
      }


      void  RichStringWriter::OnFormattingLost(TagList lost)
      {
         TagList unchanged;
         
         // Close all open tags until all lost formatting has been closed
         while (!lost.empty())
         {
            // Close tag
            auto tag = Stack.Pop();
            WriteTag(t, false);

            // Remove/preserve tag
            if (tag == lost.front())
               lost.pop_front();
            else
               unchanged.push_back(t);
         }

         // Re-open tags whoose formatting was unchanged
         for (TagType t : unchanged)
            WriteTag(t, true);
      }

      void  RichStringWriter::OnFormattingGained(const TagList& gained)
      {
         // Open tags
         for (TagType t : gained)
            WriteTag(t, true);
      }

      void  RichStringWriter::OnParagraphOpened(CharState s)
      {
         // TODO: Open all tags in 's'.  Add them to stack.
      }

      void  RichStringWriter::OnParagraphClosed(CharState s)
      {
         // Close all tags
         for (TagType t : Stack)
            switch (t)
            {
            case TagType::Bold:       Output += L"[/b]";  break;
            case TagType::Italic:     Output += L"[/i]";  break;
            case TagType::Underline:  Output += L"[/u]";  break;
            }
         
         // TODO: Close para

         Stack.clear();
      }


      /// <summary>Writes a character.</summary>
      /// <param name="chr">character.</param>
      /// <param name="prev">previous character.</param>
      void  RichStringWriter::WriteChar(TextRangePtr chr, TextRangePtr prev)
      {
         REQUIRED(chr);
         REQUIRED(prev);

         // Get fonts
         TextFontPtr  curFont = chr->Font, 
                      prevFont = prev->Font;

#error This algorithm will cause mis-matched tags. Use a two stage process: Rtf->RichString, RichString->SourceText

         // Colour change
         //if (curFont->ForeColor != prevFont->ForeColor)
         
         // Existing character formatting
         if (prevFont->Bold && !curFont->Bold)
            Output += L"[\\b]";

         if (prevFont->Italic && !curFont->Italic)
            Output += L"[\\i]";

         if (prevFont->Underline && !curFont->Underline)
            Output += L"[\\u]";

         // New Character formatting 
         if (!prevFont->Bold && curFont->Bold)
            Output += L"[b]";

         if (!prevFont->Italic && curFont->Italic)
            Output += L"[i]";

         if (!prevFont->Underline && curFont->Underline)
            Output += L"[u]";

         // Paragraph alignment change
         if (chr->Para->Alignment != prev->Para->Alignment)
         {
            // Close previous
            switch (prev->Para->Alignment)
            {
            case TOM::tomAlignLeft:     Output += L"[/left]";     break;
            case TOM::tomAlignCenter:   Output += L"[/center]";   break;
            case TOM::tomAlignRight:    Output += L"[/right]";    break;
            case TOM::tomAlignJustify:  Output += L"[/justify]";  break;
            default:
               throw AlgorithmException(HERE, GuiString(L"Unrecognised paragraph alignment: %d", prev->Para->Alignment));
            }

            // Open new
            switch (chr->Para->Alignment)
            {
            case TOM::tomAlignLeft:     Output += L"[left]";     break;
            case TOM::tomAlignCenter:   Output += L"[center]";   break;
            case TOM::tomAlignRight:    Output += L"[right]";    break;
            case TOM::tomAlignJustify:  Output += L"[justify]";  break;
            default:
               throw AlgorithmException(HERE, GuiString(L"Unrecognised paragraph alignment: %d", chr->Para->Alignment));
            }
         }

         // Write character
         WriteChar((wchar)chr->Char);
      }

      /// <summary>Writes the first or last character.</summary>
      /// <param name="chr">The character.</param>
      /// <param name="open">whether opening or closing output.</param>
      void  RichStringWriter::WriteState(TextRangePtr chr, bool open)
      {
         REQUIRED(chr);

         //
         TextFontPtr font = chr->Font;
         TextParaPtr para = chr->Para;

         // Colour
         /*if (GetColour(font) != Colour::Default)
            WriteColour(GetColour(font), open);*/
         
         // Character formatting
         if (font->Bold)
            Output += (open ? L"[b]" : L"[/b]");
         if (font->Italic)
            Output += (open ? L"[i]" : L"[/i]");
         if (font->Underline)
            Output += (open ? L"[u]" : L"[/u]");

         // Paragraph formatting
         switch (para->Alignment)
         {
         case TOM::tomAlignLeft:     Output += (open ? L"[left]"    : L"[/left]");     break;
         case TOM::tomAlignCenter:   Output += (open ? L"[center]"  : L"[/center]");   break;
         case TOM::tomAlignRight:    Output += (open ? L"[right]"   : L"[/right]");    break;
         case TOM::tomAlignJustify:  Output += (open ? L"[justify]" : L"[/justify]");  break;
         default:
            throw AlgorithmException(HERE, GuiString(L"Unrecognised paragraph alignment: %d", para->Alignment));
         }

         // Open: Write char
         if (open)
            WriteChar((wchar)chr->Char);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
