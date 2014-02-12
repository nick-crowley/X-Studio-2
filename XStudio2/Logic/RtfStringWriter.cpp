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
         list<COLORREF> col;
         col.push_back(Black);
         col.push_back(White);
         col.push_back(Red);
         col.push_back(Green);
         col.push_back(Blue);
         col.push_back(Yellow);
         col.push_back(Purple);
         col.push_back(Cyan);
         col.push_back(Grey);

         // Init writer
         RtfWriter::Open(L"Arial", 10, col);

         // Set properties
         //RtfWriter::SetForeColour(White);
         RtfWriter::SetBackColour(Black);
      }

      RtfStringWriter::~RtfStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Convert colour enumeration to RGB</summary>
      /// <param name="c">colour</param>
      /// <returns></returns>
      COLORREF RtfStringWriter::ToRGB(Colour c)
      {
         switch (c)
         {
         case Colour::Black:   return Black;
         case Colour::White:   return White;
         case Colour::Silver:  
         case Colour::Grey:    return Grey;
         case Colour::Blue:    return Blue;
         case Colour::Cyan:    return Cyan;
         case Colour::Green:   return Green;
         case Colour::Orange:  return Orange;
         case Colour::Purple:  return Purple;
         case Colour::Red:     return Red;
         case Colour::Yellow:  return Yellow;
         }

         throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised colour %d", c));
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
