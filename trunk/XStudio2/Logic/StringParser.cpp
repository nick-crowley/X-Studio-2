#include "stdafx.h"
#include "StringParser.h"
#include <algorithm>

namespace Logic
{
   namespace Language
   {
      /// <summary>Matches an opening tag (at beginning of string) and captures the name</summary>
      const wregex  StringParser::IsOpeningTag = wregex(L"^\\[([a-z]+)(?:\\s+[a-z]+\\s*=\\s*'\\w+')*\\]");

      /// <summary>Matches any closing tag (at beginning of string) and captures the name</summary>
      const wregex  StringParser::IsClosingTag = wregex(L"^\\[/?([a-z]+)\\]");
      
      /// <summary>Matches any opening/closing tag (at beginning of string) without properties and captures the name</summary>
      const wregex  StringParser::IsBasicTag = wregex(L"^\\[/?([a-z]+)\\]");

      /// <summary>Matches multiple tag properties captures both name and value</summary>
      const wregex  StringParser::IsTagProperty = wregex(L"\\s+([a-z]+)\\s*=\\s*'(\\w+)'");

      /// <summary>Matches opening and closing [author] tags (at beginning of string) and captures the text</summary>
      const wregex  StringParser::IsAuthorDefition = wregex(L"^[author](.*)[/author]");

      /// <summary>Matches opening and closing [title] tags (at beginning of string) and captures the text</summary>
      const wregex  StringParser::IsTitleDefition = wregex(L"^[title](.*)[/title]");
   
      // -------------------------------- CONSTRUCTION --------------------------------

      StringParser::StringParser(const wstring& str) : Input(str), Alignments(TagClass::Paragraph)
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
         Colour TextColour = Colour::Default;
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
                  TextColour = ReadColourCode(ch);
               else
               {  // Backslash: Append as text, escape next character
                  Escaped = true;
                  *Paragraph += new RichChar('\\', TextColour, Formatting.Current);
               }
               continue;

            // Open/Close tag:
            case '[':
               // Escaped/Not-a-tag: Append as text
               if (Escaped || !MatchTag(ch))
                  *Paragraph += new RichChar('[', TextColour, Formatting.Current);
               else 
               {
                  // RichTag: Read entire tag. Adjust colour/formatting/paragraph
                  RichTag t = ReadTag(ch);
                  switch (t.Class)
                  {
                  // Paragraph: Add/Remove alignment. Append new paragraph?
                  case TagClass::Paragraph:
                     Alignments.PushPop(t);
                     if (t.Opening)
                     {
                        Output += RichParagraph(GetAlignment(t.Type));
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
                     TextColour = Colours.Current;
                     break;
                  }
               }
               break;

            // Char: Append to current paragraph
            default:
               *Paragraph += new RichChar(*ch, TextColour, Formatting.Current);
               break;
            }

            // Revert to de-escaped
            Escaped = false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Matches a unix style colour code.</summary>
      /// <param name="pos">position of backslash</param>
      /// <returns></returns>
      bool  StringParser::MatchColourCode(CharIterator pos) const
      {
         // Check for EOF?
         if (pos+3 >= Input.end())
            return false;

         // Match '\\033'
         if (pos[0] != '\\' || pos[1] != '0' || pos[2] != '3' || pos[3] != '3')
            return false;

         // Match colour character
         switch (pos[4])
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

      /// <summary>Matches any opening or closing tag</summary>
      /// <param name="pos">position of opening bracket</param>
      /// <returns></returns>
      bool  StringParser::MatchTag(CharIterator pos) const
      {
         wsmatch match;

         // Match tag using RegEx
         return regex_search(pos, Input.end(), match, IsOpeningTag) || regex_search(pos, Input.end(), match, IsClosingTag);
      }


      /// <summary>Reads the unix style colour code and advances the iterator</summary>
      /// <param name="pos">position of backslash</param>
      /// <returns></returns>
      Colour  StringParser::ReadColourCode(CharIterator& pos)
      {
         Colour c;

         // Match colour character
         switch (*pos)
         {
         case 'A':   c = Colour::Silver;  break;
         case 'B':   c = Colour::Blue;    break;
         case 'C':   c = Colour::Cyan;    break;
         case 'G':   c = Colour::Green;   break;
         case 'O':   c = Colour::Orange;  break;
         case 'M':   c = Colour::Purple;  break;
         case 'R':   c = Colour::Red;     break;
         case 'W':   c = Colour::White;   break;
         case 'X':   c = Colour::Default; break;
         case 'Y':   c = Colour::Yellow;  break;
         case 'Z':   c = Colour::Black;   break;
         default:
            throw "Invalid colour code";
         }

         // Consume + return colour
         pos += 4;
         return c;
      }

      /// <summary>Reads the entire tag and advances the iterator</summary>
      /// <param name="pos">position of opening bracket</param>
      /// <returns></returns>
      StringParser::RichTag  StringParser::ReadTag(CharIterator& pos)
      {
         wsmatch matches;

         // Find end bracket
         auto end = find_if(pos, Input.cend(), [](const wchar& ch) {return ch == ']';} );
         
         // BASIC: Open/Close Tag without properties
         if (regex_match(pos, end, matches, IsBasicTag))
         {
            // Identify name/open/close
            bool opening = (pos[1] != '\\');
            wstring name(opening ? pos+1 : pos+2, end-1);
            
            // Identify type
            switch (TagType type = IdentifyTag(name))
            {
            // Author/Title: Extract text directly into Output
            case TagType::Author:
               // Match [author](text)[/author]
               if (!regex_search(pos, Input.cend(), matches, IsAuthorTag))
                  throw "Invalid author";

               // Extract capture. Advance iterator
               pos += matches[0].length();
               return RichTag(type, matches[1].str());

            case TagType::Title:
               // Match [title](text)[/title]
               if (!regex_search(pos, Input.cend(), matches, IsTitleTag))
                  throw "Invalid title";

               // Extract capture. Advance iterator
               pos += matches[0].length();
               return RichTag(type, matches[1].str());

            // Default: Advance iterator to ']' + return
            default:
               pos = end;
               return RichTag(type, opening);
            }
         }
         else
         {
            // COMPLEX: Open tag with properties
         }
      }

      Alignment StringParser::GetAlignment(TagType t) const
      {
         // TODO
         return Alignment::Left;
      }

      StringParser::TagType  StringParser::IdentifyTag(const wstring& name) const
      {
         // TODO
         return TagType::Author;
      }
   }
}
