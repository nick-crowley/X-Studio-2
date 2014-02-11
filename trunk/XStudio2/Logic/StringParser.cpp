#include "stdafx.h"
#include "StringParser.h"

namespace Logic
{
   namespace Language
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      StringParser::StringParser(const wstring& str) : Input(str)
      {
         Parse();
      }


      StringParser::~StringParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Parses this instance.</summary>
      void  StringParser::Parse()
      {
         RichParagraph* Paragraph;
         bool  Escaped = false;
         Colour CurrentColour = Colour::Default;

         for (auto ch = Input.begin(); ch != Input.end(); ++ch)
         {
            switch (*ch)
            {
            // Escape/ColourCode: 
            case '\\': 
               // ColourCode: Read and override existing stack colour, if any
               if (MatchColourCode(ch))
                  CurrentColour = ReadColourCode(ch)
               else
               {  // Backslash: Insert as char, escape next character
                  Escaped = true;
                  Paragraphs.Current += RichChar('\\', CurrentColour, Formatting.Current);
               }
               continue;

            // Open/Close tag:
            case '[':
               // Escaped/Not-a-tag: Add as char
               if (Escaped || MatchTag(ch))
                  Paragraphs.Current += RichChar('[', CurrentColour, Formatting.Current);
               else 
               {
                  // RichTag: Read entire tag. Adjust colour/formatting/paragraph
                  RichTag t = ReadTag(ch);
                  switch (t.Class)
                  {
                  // Paragraph:
                  case TagClass::Paragraph:
                     Paragraphs.PushPop(t);
                     if (t.Opening)
                        Output += RichParagraph(t.Type);
                     break;

                  // Formatting: Add/Remove current formatting
                  case TagClass::Character:
                     Formatting.PushPop(t);
                     break;

                  // Colour: Add/Remove current colour. Reset any colourCode override
                  case TagClass::Colour:
                     Colours.PushPop(t);
                     Colour = Colours.Current;
                     break;
                  }
               }
               break;

            // Char: Append to current paragraph
            default:
               Paragraphs.Current += RichChar(*ch, CurrentColour, Formatting.Current);
               break;
            }

            // Revert to de-escaped
            Escaped = false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
