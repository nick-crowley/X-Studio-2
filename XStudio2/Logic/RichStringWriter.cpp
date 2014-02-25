#include "stdafx.h"
#include "RichStringWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      RichStringWriter::RichStringWriter(TextDocumentPtr& doc) : Input(doc)
      {
         REQUIRED(doc);
      }


      RichStringWriter::~RichStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Writes this instance.</summary>
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
         
         // Character formatting change
         if (!prevFont->Bold && curFont->Bold)
            Output += L"[b]";
         else if (prevFont->Bold && !curFont->Bold)
            Output += L"[\\b]";

         if (!prevFont->Italic && curFont->Italic)
            Output += L"[i]";
         else if (prevFont->Italic && !curFont->Italic)
            Output += L"[\\i]";

         if (!prevFont->Underline && curFont->Underline)
            Output += L"[u]";
         else if (prevFont->Underline && !curFont->Underline)
            Output += L"[\\u]";

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
