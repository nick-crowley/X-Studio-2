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
         Colour CurrentColour = Colour::Default;
         bool   Escaped = false;

         // Iterate thru input
         for (auto ch = Input.begin(); ch != Input.end(); ++ch)
         {
            switch (*ch)
            {
            // Escape/ColourCode: 
            case '\\': 
               // ColourCode: Read and override existing stack colour, if any
               if (MatchColourCode(ch))
                  CurrentColour = ReadColourCode(ch);
               else
               {  // Backslash: Insert as char, escape next character
                  Escaped = true;
                  *Paragraph += RichChar('\\', CurrentColour, Formatting.Current);
               }
               continue;

            // Open/Close tag:
            case '[':
               // Escaped/Not-a-tag: Add as char
               if (Escaped || MatchTag(ch))
                  *Paragraph += RichChar('[', CurrentColour, Formatting.Current);
               else 
               {
                  // RichTag: Read entire tag. Adjust colour/formatting/paragraph
                  RichTag t = ReadTag(ch);
                  switch (t.Class)
                  {
                  // Paragraph:
                  case TagClass::Paragraph:
                     Output.Paragraphs.PushPop(t);
                     if (t.Opening)
                     {
                        Output += RichParagraph(t.Type);
                        Paragraph = &Output.Paragraphs.back();
                     }
                     break;

                  // Formatting: Add/Remove current formatting
                  case TagClass::Character:
                     Formatting.PushPop(t);
                     break;

                  // Colour: Add/Remove current colour. Reset any colourCode override
                  case TagClass::Colour:
                     Colours.PushPop(t);
                     CurrentColour = Colours.Current;
                     break;
                  }
               }
               break;

            // Char: Append to current paragraph
            default:
               *Paragraph += RichChar(*ch, CurrentColour, Formatting.Current);
               break;
            }

            // Revert to de-escaped
            Escaped = false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      bool  StringParser::MatchColourCode(CharIterator ch)
      {
         // Check for EOF?
         if (ch+3 >= Input.end())
            return false;

         // Match '\\033'
         if (ch[0] != '\\' || ch[1] != '0' || ch[2] != '3' || ch[3] != '3')
            return false;

         // Match colour character
         switch (ch[4])
         {
         case 'A':   // Silver
         case 'B':   // Blue
         case 'C':   // Cyan
         case 'G':   // Green
         case 'O':   // Orange
         case 'M':   // Magenta
         case 'R':   // Red
         case 'W':   // White
         case 'X':   // Default
         case 'Y':   // Yellow
         case 'Z':   // Black
            return true;
         }

         // Failed
         return false;
      }

      Colour  StringParser::ReadColourCode(CharIterator ch)
      {
         // Match colour character
         switch (*ch)
         {
         case 'A':   return Colour::Silver;
         case 'B':   return Colour::Blue;
         case 'C':   return Colour::Cyan;
         case 'G':   return Colour::Green;
         case 'O':   return Colour::Orange;
         case 'M':   return Colour::Purple;
         case 'R':   return Colour::Red;
         case 'W':   return Colour::White;
         case 'X':   return Colour::Default;
         case 'Y':   return Colour::Yellow;
         case 'Z':   return Colour::Black;
         }

         // Failed
         throw "Invalid colour code";
      }
   }
}
