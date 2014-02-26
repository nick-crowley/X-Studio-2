#include "stdafx.h"
#include "RtfStringWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create an Rtf string writer</summary>
      /// <param name="out">Output stream</param>
      RtfStringWriter::RtfStringWriter(string& out) : RtfWriter(out)
      {
         // Define colours
         list<COLORREF> col = 
         {
            (COLORREF)LanguageColour::Default,
            (COLORREF)LanguageColour::Black,
            (COLORREF)LanguageColour::Cyan,
            (COLORREF)LanguageColour::Blue,
            (COLORREF)LanguageColour::Green,
            (COLORREF)LanguageColour::Grey,
            (COLORREF)LanguageColour::Orange,
            (COLORREF)LanguageColour::Purple,
            (COLORREF)LanguageColour::Red,
            (COLORREF)LanguageColour::Silver,
            (COLORREF)LanguageColour::Yellow,
            (COLORREF)LanguageColour::White,
         };

         // Init writer
         RtfWriter::Open(L"Arial", 10, col);

         // Set properties
         //RtfWriter::SetForeColour(White);
         RtfWriter::SetBackColour((COLORREF)LanguageColour::Black);
      }

      RtfStringWriter::~RtfStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Convert colour enumeration to RGB</summary>
      /// <param name="c">colour</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Invalid colour</exception>
      COLORREF RtfStringWriter::ToRGB(Colour c)
      {
         switch (c)
         {
         case Colour::Black:   return (COLORREF)LanguageColour::Black;
         case Colour::White:   return (COLORREF)LanguageColour::White;
         case Colour::Default: return (COLORREF)LanguageColour::Default;

         case Colour::Grey:    return (COLORREF)LanguageColour::Grey;
         case Colour::Blue:    return (COLORREF)LanguageColour::Blue;
         case Colour::Cyan:    return (COLORREF)LanguageColour::Cyan;
         case Colour::Green:   return (COLORREF)LanguageColour::Green;
         case Colour::Orange:  return (COLORREF)LanguageColour::Orange;
         case Colour::Purple:  return (COLORREF)LanguageColour::Purple;
         case Colour::Red:     return (COLORREF)LanguageColour::Red;
         case Colour::Silver:  return (COLORREF)LanguageColour::Silver;
         case Colour::Yellow:  return (COLORREF)LanguageColour::Yellow;
         }

         throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised colour enumeration: %d", c));
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer</summary>
      void  RtfStringWriter::Close()
      {
         RtfWriter::Close();
      }

      /// <summary>Writes a rich-text string to the output stream</summary>
      /// <param name="str">The string.</param>
      void  RtfStringWriter::Write(const RichString& str)
      {
         bool firstPara = true;

         // Write paragraphs
         for (auto& para : str.Paragraphs)
         {
            // Create paragraph
            if (!firstPara)
               RtfWriter::StartParagraph();

            // Align paragraph
            RtfWriter::SetAlignment(para.Align);

            // Write content
            for (auto& el : para.Content)
            {
               const RichCharacter* chr;
               switch (el->Type)
               {
               case ElementType::Button:
                  break;

               case ElementType::Character:
                  chr = dynamic_cast<const RichCharacter*>(el.get());

                  // Set properties
                  RtfWriter::SetBold(chr->Bold);
                  RtfWriter::SetItalic(chr->Italic);
                  RtfWriter::SetUnderline(chr->Underline);
                  RtfWriter::SetForeColour(ToRGB(chr->Colour));

                  // Write char
                  RtfWriter::Write(chr->Char);
                  break;
               }
            }

            // Create future paragraphs
            firstPara = false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
