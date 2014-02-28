#include "stdafx.h"
#include "RichStringParser.h"
#include <algorithm>

namespace Logic
{
   namespace Language
   {
      /// <summary>Matches an opening tag (at beginning of string) and captures the name</summary>
      const wregex  RichStringParser::IsOpeningTag = wregex(L"^\\[([a-z]+)(?:\\s+[a-z]+\\s*=\\s*[\"'][\\w\\d\\._]+[\"'])*\\]");

      /// <summary>Matches any closing tag (at beginning of string) and captures the name</summary>
      const wregex  RichStringParser::IsClosingTag = wregex(L"^\\[/?([a-z]+)\\]");
      
      /// <summary>Matches any opening/closing tag (at beginning of string) without properties and captures the name</summary>
      const wregex  RichStringParser::IsBasicTag = wregex(L"^\\[/?([a-z]+)\\]");

      /// <summary>Matches multiple tag properties captures both name and value</summary>
      const wregex  RichStringParser::IsTagProperty = wregex(L"\\s+([a-z]+)\\s*=\\s*[\"']([\\w\\d\\._]+)[\"']");

      /// <summary>Matches opening and closing [author] tags (at beginning of string) and captures the text</summary>
      const wregex  RichStringParser::IsAuthorDefinition = wregex(L"^\\[author\\](.*?)\\[/author\\]");

      /// <summary>Matches opening and closing [select] tags (at start of string) and captures the text</summary>
      const wregex  RichStringParser::IsButtonDefinition = wregex(L"^\\[select\\](.*?)\\[/select\\]");

      /// <summary>Matches button text and closing [select] tags</summary>
      const wregex  RichStringParser::IsButtonText = wregex(L"^\\](.*?)\\[/select\\]");

      /// <summary>Matches opening and closing [title] tags (at beginning of string) and captures the text</summary>
      const wregex  RichStringParser::IsTitleDefinition = wregex(L"^\\[title\\](.*?)\\[/title\\]");
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a rich-text parser from an input string</summary>
      /// <param name="str">string containing rich-text tags</param>
      /// <param name="default">Default paragraph alignment</param>
      /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::Language::RichTextException">Error in formatting tags</exception>
      RichStringParser::RichStringParser(const wstring& str, Alignment default) 
         : Input(str), Output(default), Alignments(TagClass::Paragraph)
      {
         Parse();
      }


      RichStringParser::~RichStringParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Gets the associated paragraph alignment of a paragraph tag</summary>
      /// <param name="t">tag type</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Not a paragraph tag</exception>
      Alignment RichStringParser::GetAlignment(TagType t) 
      {
         switch (t)
         {
         case TagType::Left:     return Alignment::Left;
         case TagType::Right:    return Alignment::Right;
         case TagType::Centre:   return Alignment::Centre;
         case TagType::Justify:  return Alignment::Justify;
         }
         
         // Not paragraph tag
         throw ArgumentException(HERE, L"t", GuiString(L"Cannot get alignment for %s tag", ::GetString(t).c_str()));
      }

      /// <summary>Gets the class of a tag</summary>
      /// <param name="t">tag type</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Tag is unrecognised</exception>
      RichStringParser::TagClass  RichStringParser::GetClass(TagType t)
      {
         switch (t)
         {
         case TagType::Bold:
         case TagType::Underline:
         case TagType::Italic:
            return TagClass::Character;

         case TagType::Left:
         case TagType::Right:
         case TagType::Centre:
         case TagType::Justify:
            return TagClass::Paragraph;

         case TagType::Text:
         case TagType::Select:
         case TagType::Author:
         case TagType::Title:
         case TagType::Rank:
            return TagClass::Special;

         case TagType::Black:
         case TagType::Blue:
         case TagType::Cyan:
         case TagType::Green:
         case TagType::Magenta:
         case TagType::Orange:
         case TagType::Red:
         case TagType::Silver:
         case TagType::Yellow:
         case TagType::White:
            return TagClass::Colour;
         }

         throw ArgumentException(HERE, L"t", L"Default/Unrecognised tags have no class");
      }

      /// <summary>Convert tag class to string</summary>
      /// <param name="c">tag class.</param>
      /// <returns></returns>
      wstring  RichStringParser::GetString(TagClass c)
      {
         const wchar* str[] = { L"Character", L"Paragraph", L"Colour", L"Special" };

         // Validate
         if ((UINT)c > (UINT)TagClass::Special)
            throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised tag class %d", c));

         return str[(UINT)c];
      }
      
      /// <summary>Convert tag type to string</summary>
      /// <param name="t">tag type.</param>
      /// <returns></returns>
      wstring  GetString(TagType t)
      {
         const wchar* str[] = { L"Bold", L"Underline", L"Italic", 
                                L"Left", L"Right", L"Centre", L"Justify", 
                                L"Text", L"Select", L"Author", L"Title", L"Rank",
                                L"Black", L"Blue", L"Cyan", L"Green", L"Grey", L"Magenta", L"Orange", L"Red", L"Silver", L"Yellow", L"White", 
                                L"Default", L"Unrecognised"  };

         // Validate
         if ((UINT)t >= (UINT)TagType::Unrecognised)
            throw ArgumentException(HERE, L"t", GuiString(L"Unrecognised tag type %d", t));

         return str[(UINT)t];
      }

      /// <summary>Identifies a tag type from it's name</summary>
      /// <param name="name">tag name WITHOUT square brackets</param>
      /// <returns></returns>
      TagType  RichStringParser::IdentifyTag(const wstring& name)
      {
         switch (name.length())
         {
         case 1:
            if (name == L"b")
               return TagType::Bold;
            else if (name == L"i")
               return TagType::Italic;
            else if (name == L"u")
               return TagType::Underline;
            break;

         case 3:
            if (name == L"red")
               return TagType::Red;
            break;

         case 4:
            if (name == L"left")
               return TagType::Left;
            else if (name == L"blue")
               return TagType::Blue;
            else if (name == L"cyan")
               return TagType::Cyan;
            else if (name == L"grey")
               return TagType::Grey;
            else if (name == L"text")
               return TagType::Text;
            break;

         case 5:
            if (name == L"right")
               return TagType::Right;
            else if (name == L"title")
               return TagType::Title;
            else if (name == L"green")
               return TagType::Green;
            else if (name == L"black")
               return TagType::Black;
            else if (name == L"white")
               return TagType::White;
            break;

         case 6:
            if (name == L"center")
               return TagType::Centre;
            else if (name == L"select")
               return TagType::Select;
            else if (name == L"author")
               return TagType::Author;
            else if (name == L"orange")
               return TagType::Orange;
            else if (name == L"yellow")
               return TagType::Yellow;
            else if (name == L"silver")
               return TagType::Silver;
            break;

         case 7:
            if (name == L"magenta")
               return TagType::Magenta;
            else if (name == L"justify")
               return TagType::Justify;
            else if (name == L"ranking")
               return TagType::Rank;
            break;
         }

         // Unrecognised;
         return TagType::Unrecognised;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------


      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Parses input string</summary>
      /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::Language::RichTextException">Error in formatting tags</exception>
      void  RichStringParser::Parse()
      {
         RichParagraph* Paragraph = &FirstParagraph;     // Current paragraph (output)
         Colour         TextColour = Colour::Default;    // Current colour

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
               
               // Final char: Add verbatim
               else if (ch+1 == Input.end())
                  *Paragraph += new RichCharacter('\\', TextColour, Formatting.Current);

               // Escaped: Convert/De-escape character
               else switch (ch[1])
               {
               // NewLine/Tab: Convert to character representation
               case 'n':   *Paragraph += new RichCharacter('\n', TextColour, Formatting.Current);  ++ch;  break;
               case 't':   *Paragraph += new RichCharacter('\t', TextColour, Formatting.Current);  ++ch;  break;
               // Escaped bracket: Strip escape
               case '{':
               case '}':
               case '[':
               case ']':
               case '(':
               case ')':   *Paragraph += new RichCharacter(ch[1], TextColour, Formatting.Current);  ++ch;  break;
               // Unknown
               default:    *Paragraph += new RichCharacter('\\', TextColour, Formatting.Current);  ++ch;   break;
               }
               continue;

            // Open/Close tag:
            case '[':
               // Not-a-tag: Append as text
               if (!MatchTag(ch))
                  *Paragraph += new RichCharacter('[', TextColour, Formatting.Current);
               else 
               {
                  // RichTag: Read entire tag. Adjust colour/formatting/paragraph
                  RichTag tag = ReadTag(ch);
                  switch (tag.Class)
                  {
                  // Paragraph: Add/Remove alignment. Append new paragraph?
                  case TagClass::Paragraph:
                     Alignments.PushPop(tag);
                     // Open+Empty: Adjust existing alignment
                     if (tag.Opening && Paragraph->Content.empty())
                        Paragraph->Align = GetAlignment(tag.Type);

                     // Open: Append new 
                     else if (tag.Opening)
                     {  
                        Output += RichParagraph(GetAlignment(tag.Type));
                        Paragraph = &Output.Paragraphs.back();
                     }
                     // Close: Append empty default-aligned (unless EOF)
                     else if (!tag.Opening && ch+1 < Input.end())
                     {  
                        Output += RichParagraph(Default);
                        Paragraph = &Output.Paragraphs.back();
                     }
                     break;

                  // Formatting: Add/Remove current formatting
                  case TagClass::Character:
                     Formatting.PushPop(tag);
                     break;

                  // Colour: Add/Remove current colour. Reset any colourCode override
                  case TagClass::Colour:
                     Colours.PushPop(tag);
                     TextColour = Colours.Current;
                     break;

                  // Special: 
                  case TagClass::Special:
                     switch (tag.Type)
                     {
                     // Author/Title: Insert into output
                     case TagType::Author: Output.Author = tag.Text; break;
                     case TagType::Title:  Output.Title = tag.Text;  break;

                     // Button:
                     case TagType::Select: *Paragraph += CreateButton(tag);  break;
                     case TagType::Text:   SetColumnInfo(tag);               break;
                     }
                     break;
                  }
               }
               break;

            // Char: Append to current paragraph
            default:
               *Paragraph += new RichCharacter(*ch, TextColour, Formatting.Current);
               break;
            }
         }
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Creates a button from a select tag</summary>
      /// <param name="tag">select tag.</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidOperation">Not a 'select' tag</exception>
      /// <exception cref="Logic::Language::RichTextException">Invalid tag property</exception>
      RichButton*  RichStringParser::CreateButton(const RichTag& tag) const
      {
         // Ensure tag is 'select'
         if (tag.Type != TagType::Select)
            throw InvalidOperationException(HERE, GuiString(L"Cannot create a button from a '%s' tag", ::GetString(tag.Type).c_str()) );

         // Anonymous: Use text only
         if (tag.Properties.empty())
            return new RichButton(tag.Text);
         else
         {
            // Ensure property is 'value'
            if (tag.Properties.front().Name != L"value")
               throw RichTextException(HERE, GuiString(L"Unrecognised button property '%s'", tag.Properties.front().Name.c_str()) );

            // Return text + ID
            return new RichButton(tag.Text, tag.Properties.front().Value);
         }
      }
      

      /// <summary>Gets the first paragraph.</summary>
      /// <returns></returns>
      RichParagraph& RichStringParser::GetFirstParagraph() 
      {
         return Output.Paragraphs.back();
      }

      /// <summary>Matches a unix style colour code.</summary>
      /// <param name="pos">position of backslash</param>
      /// <returns></returns>
      bool  RichStringParser::MatchColourCode(CharIterator pos) const
      {
         // Check for EOF?
         if (distance(pos, Input.end()) <= 4)      //if (pos+4 >= Input.end())
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
      bool  RichStringParser::MatchTag(CharIterator pos) const
      {
         wsmatch match;

         // Match tag using RegEx
         if (regex_search(pos, Input.end(), match, IsOpeningTag) || regex_search(pos, Input.end(), match, IsClosingTag))
            // Ignore unrecognised tags
            return IdentifyTag(match[1].str()) != TagType::Unrecognised;

         // Not a tag
         return false;
      }


      /// <summary>Reads the unix style colour code and advances the iterator</summary>
      /// <param name="pos">position of backslash</param>
      /// <returns></returns>
      /// <exception cref="Logic::AlgorithmException">Previously matched colour code is unrecognised</exception>
      /// <remarks>Advances the iterator to the last character of the tag, so Parse() loop advances correctly to the next character</remarks>
      Colour  RichStringParser::ReadColourCode(CharIterator& pos)
      {
         Colour c;

         // Match colour character
         switch (pos[4])
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
            throw AlgorithmException(HERE, GuiString(L"Previously matched colour code '%c' is unrecognised", *pos));
         }

         // Consume + return colour
         pos += 4;
         return c;
      }

      /// <summary>Reads the entire tag and advances the iterator</summary>
      /// <param name="pos">position of opening bracket</param>
      /// <returns></returns>
      /// <exception cref="Logic::Language::AlgorithmException">Unable to read tag</exception>
      /// <exception cref="Logic::Language::RichTextException">Closing tag doesn't match currently open tag</exception>
      /// <remarks>Advances the iterator to the last character of the tag, so Parse() loop advances correctly to the next character</remarks>
      RichStringParser::RichTag  RichStringParser::ReadTag(CharIterator& pos)
      {
         wsmatch matches;

         // BASIC: Open/Close Tag without properties
         if (regex_search(pos, Input.cend(), matches, IsBasicTag))
         {
            // Identify open/close
            bool opening = (pos[1] != '/');
            wstring name = matches[1].str();
            
            // Identify type
            switch (TagType type = IdentifyTag(name))
            {
            // Title: Return title
            case TagType::Author:
            case TagType::Select:
            case TagType::Title:
               // Match [title](text)[/title]
               if (!regex_search(pos, Input.cend(), matches, type == TagType::Title  ? IsTitleDefinition
                                                           : type == TagType::Author ? IsAuthorDefinition
                                                                                     : IsButtonDefinition)) 
                  throw RichTextException(HERE, GuiString(L"Invalid [%s] tag", ::GetString(type).c_str()));

               // Advance iterator.  Return title text
               pos += matches[0].length()-1;
               return RichTag(type, matches[1].str());

            // Default: Advance iterator to ']' + return
            default:
               pos += matches[0].length()-1;
               return RichTag(type, opening);
            }
         }
         // COMPLEX: Open tag with properties
         else if (regex_search(pos, Input.cend(), matches, IsOpeningTag))  // Match tag name, identify start/end of properties
         {
            PropertyList props;

            // Match properties
            for (wsregex_iterator it(pos+matches[1].length(), pos+matches[0].length(), IsTagProperty), eof; it != eof; ++it)
               // Extract {name,value} from 2st/3rd match
               props.push_back( Property(it->str(1), it->str(2)) );
            
            // Advance Iterator + create tag
            pos += matches[0].length()-1;
            auto tag = RichTag(IdentifyTag(matches[1].str()), props);

            // Button: Extract text
            if (tag.Type == TagType::Select)
            {
               if (!regex_search(pos, Input.cend(), matches, IsButtonText))
                  throw RichTextException(HERE, GuiString(L"Invalid [select] tag"));

               // Extract text + Advance Iterator
               tag.Text = matches[1].str();
               pos += matches[0].length()-1;
            }

            // Return tag
            return tag;
         }

         // Error: No match
         throw AlgorithmException(HERE, L"Cannot read previously matched opening tag");
      }
      
      /// <summary>Extracts column information from a text tag</summary>
      /// <param name="tag">text tag</param>
      /// <exception cref="Logic::InvalidOperation">Not a 'text' tag</exception>
      /// <exception cref="Logic::Language::RichTextException">Invalid tag property</exception>
      void  RichStringParser::SetColumnInfo(const RichTag& tag)
      {
         // Ensure tag is 'text'
         if (tag.Type != TagType::Text)
            throw InvalidOperationException(HERE, GuiString(L"Cannot extract column info from a '%s' tag", ::GetString(tag.Type).c_str()) );

         // Extract properties direct into output
         for (const Property& p : tag.Properties)
         {
            // Column count
            if (p.Name == L"cols")
            {
               // Verify
               auto cols = _ttoi(p.Value.c_str());
               if (cols < 0 || cols > 3)
                  throw RichTextException(HERE, GuiString(L"Cannot arrange text in %d columns", cols));
               Output.Columns = static_cast<ColumnType>(cols);
            }
            // Column width
            else if (p.Name == L"colwidth")
               Output.Width = _ttoi(p.Value.c_str());
            // Column spacing
            else if (p.Name == L"colspacing")
               Output.Width = _ttoi(p.Value.c_str());
            else
               // Unrecognised
               throw RichTextException(HERE, GuiString(L"Invalid 'text' tag property '%s'", p.Name.c_str()));
         }
      }

   }
}
