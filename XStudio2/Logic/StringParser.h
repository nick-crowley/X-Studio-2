#pragma once

#include "Common.h"
#include <regex>

namespace Logic
{
   namespace Language
   {
      class RichElement;

      /// <summary>Paragraph alignment</summary>
      enum class Alignment { Left, Centre, Right, Justify };

      /// <summary>Column spacing of a rich-text string</summary>
      enum class ColumnSpacing { Default, Single, Double, Triple };

      /// <summary>Distinguishes characters from buttons in rich text</summary>
      enum class ElementType  { Character, Button };


      /// <summary>Shared pointer to a rich-text character/button</summary>
      typedef shared_ptr<RichElement>  RichElementPtr;

      /// <summary>List of rich-text characters/buttons</summary>
      typedef list<RichElementPtr>  ElementList;

      /// <summary>Button/Character within a rich-text string</summary>
      class RichElement
      {
      protected:
         RichElement(ElementType t) : Type(t)
         {}

      public:
         ElementType Type;
      };

      /// <summary>Character within a rich-text paragraph</summary>
      class RichChar : public RichElement
      {
      public:
         RichChar(wchar ch, Colour c, UINT format) : RichElement(ElementType::Character), Char(ch), Colour(c), Format(format)
         {}

         wchar  Char;
         UINT   Format;
         Colour Colour;
      };
      
      /// <summary>Button within a rich-text paragraph</summary>
      class RichButton : public RichElement
      {
      public:
         RichButton(const wstring& id, const ElementList& txt) : RichElement(ElementType::Button), ID(id), Text(txt)
         {}

         wstring     ID;
         ElementList Text;
      };

      /// <summary>Paragraph of text within a rich-text string</summary>
      class RichParagraph
      {
      public:
         RichParagraph() : Align(Alignment::Left)
         {}
         RichParagraph(Alignment al) : Align(al)
         {}

         /// <summary>Append character</summary>
         RichParagraph& operator+=(RichChar* ch)
         {
            Content.push_back(RichElementPtr(ch));
            return *this;
         }

         ElementList Content;
         Alignment   Align;
      };

      /// <summary>Result of parsing a Language string with rich-text formatting</summary>
      class RichString
      {
      public:
         /// <summary>Append paragraph</summary>
         RichString& operator+=(const RichParagraph& p)
         {
            Paragraphs.push_back(p);
            return *this;
         }

         list<RichParagraph> Paragraphs;
         ColumnSpacing       Spacing;
         UINT                ColumnWidth;
         wstring             Author,
                             Title;
      };
      
      /// <summary>Rich-text parser</summary>
      class StringParser
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary>Constant character iterator</summary>
         typedef wstring::const_iterator CharIterator;

         /// <summary>Defines available rich edit tags</summary>
         enum class TagType { Bold, Underline, Italic, Left, Right, Centre, Justify, Text, Select, Author, Title, Rank,
                              Black, Blue, Cyan, Green, Magenta, Orange, Red, Silver, Yellow, White };

         /// <summary>Groups tags by category</summary>
         enum class TagClass { Character, Paragraph, Colour, Special };

         /// <summary>Any valid tag in a rich-text string</summary>
         class RichTag
         {
         public:
            RichTag(TagType t, bool open) : Type(t), Class(GetClass(t)), Opening(open), Closing(!open), SelfClosing(false)
            {}

            RichTag(TagType t, const wstring& txt) : Type(t), Class(TagClass::Special), Text(txt), Opening(false), Closing(false), SelfClosing(false)
            {}

            bool     Opening,
                     Closing,
                     SelfClosing;
            
            wstring  Text;
            TagType  Type;
            TagClass Class;

         private:
            TagClass GetClass(TagType t)
            {
               // TODO
               return TagClass::Character;
            }
         };

         /// <summary>Stack of rich formatting tags</summary>
         class TagStack : protected deque<TagType>
         {
         public:
            /// <summary>Create tag stack</summary>
            /// <param name="cl">The class of tags to hold</param>
            TagStack(TagClass cl) : Class(cl)
            {}

            /// <summary>Push or pop a tag from the stack</summary>
            void  PushPop(const RichTag& tag)
            {
               if (tag.Class != Class)
                  throw "Cannot add %s tag to a %s stack";

               // Open: Push stack
               if (tag.Opening)
                  push_back(tag.Type);

               // Close: Pop stack
               else if (!empty() && back() == tag.Type)
                  pop_back();

               else
                  // Mismatch: Error
                  throw "Mismatched tag";
            }

         private:
            TagClass Class;
         };

         /// <summary>Specialised character tag stack</summary>
         class CharTagStack : public TagStack
         {
         public:
            CharTagStack() : TagStack(TagClass::Character)
            {}

            PROPERTY_GET(UINT,Current,GetCurrent);

            /// <summary>Get formatting flags for all tags in stack</summary>
            UINT  GetCurrent()
            {
               UINT format;
               
               // Convert current tags into formating flags
               for (auto& t : *this)
                  switch (t)
                  {
                  case TagType::Bold:       format |= CFE_BOLD;       break;
                  case TagType::Italic:     format |= CFE_ITALIC;     break;
                  case TagType::Underline:  format |= CFE_UNDERLINE;  break;
                  }

               return format;
            }
         };

         /// <summary>Specialised character tag stack</summary>
         class ColourTagStack : public TagStack
         {
         public:
            ColourTagStack() : TagStack(TagClass::Colour)
            {}
            
            PROPERTY_GET(Colour,Current,GetCurrent);

            /// <summary>Get formatting flags for all tags in stack</summary>
            Colour GetCurrent()
            {
               if (empty())
                  return Colour::Default;

               switch (back())
               {
               case TagType::Black:	   return Colour::Black;
               case TagType::Blue:	   return Colour::Blue;     
               case TagType::Cyan:	   return Colour::Cyan;
               case TagType::Green:	   return Colour::Green;
               case TagType::Magenta:	return Colour::Purple;
               case TagType::Orange:	return Colour::Orange;
               case TagType::Red:	   return Colour::Red;
               case TagType::Silver:	return Colour::Silver;
               case TagType::Yellow:	return Colour::Yellow;
               case TagType::White:	   return Colour::White;
               }

               throw "Unrecognised colour tag";
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         StringParser(const wstring& str);
         virtual ~StringParser();

         DEFAULT_COPY(StringParser);	// Default copy semantics
         DEFAULT_MOVE(StringParser);	// Default move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const wregex IsOpeningTag,
                             IsClosingTag,
                             IsBasicTag,
                             IsTagProperty,
                             IsAuthorDefition,
                             IsTitleDefition;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  MatchColourCode(CharIterator pos) const;
         bool  MatchTag(CharIterator pos) const;

      private:
         Alignment GetAlignment(TagType t) const;
         TagType   IdentifyTag(const wstring& name) const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Parse();

      private:
         Colour  ReadColourCode(CharIterator& pos);
         RichTag ReadTag(CharIterator& pos);

         // -------------------- REPRESENTATION ---------------------
      public:
         RichString      Output;

      private:
         wstring         Input;
         ColourTagStack  Colours;
         CharTagStack    Formatting;
         TagStack        Alignments;
      };

   }
}

using namespace Logic::Language;