#pragma once

#include "Common.h"
#include <regex>

namespace Logic
{
   namespace Language
   {
      class RichElement;

      // ------------------------ TYPES --------------------------

      /// <summary>Column spacing of a rich-text string</summary>
      enum class ColumnType { Default, Single, Double, Triple };

      /// <summary>Distinguishes characters from buttons in rich text</summary>
      enum class ElementType  { Character, Button };


      /// <summary>Shared pointer to a rich-text character/button</summary>
      typedef shared_ptr<RichElement>  RichElementPtr;

      /// <summary>List of rich-text characters/buttons</summary>
      typedef list<RichElementPtr>  ElementList;

      // ------------------------ CLASSES ------------------------

      /// <summary>Occurs when an error is detected in a rich-text string</summary>
      class RichTextException : public ExceptionBase
      {
      public:
         /// <summary>Create a RichTextException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="msg">Message</param>
         RichTextException(wstring  src, wstring  msg) 
            : ExceptionBase(src, wstring(L"Invalid formatting: ") + msg)
         {}
      };

      /// <summary>Base class for a Button/Character within a rich-text string</summary>
      class RichElement
      {
         // --------------------- CONSTRUCTION ----------------------
      protected:
         RichElement(ElementType t) : Type(t)
         {}

      public:
         ElementType Type;
      };

      /// <summary>Character within a rich-text paragraph</summary>
      class RichCharacter : public RichElement
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RichCharacter(wchar ch, Colour c, UINT format) : RichElement(ElementType::Character), Char(ch), Colour(c), Format(format)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         wchar  Char;
         UINT   Format;
         Colour Colour;
      };

      
      /// <summary>Button within a rich-text paragraph</summary>
      class RichButton : public RichElement
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RichButton(const wstring& id, const ElementList& txt) : RichElement(ElementType::Button), ID(id), Text(txt)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring     ID;
         ElementList Text;
      };

      /// <summary>Paragraph of text within a rich-text string</summary>
      class RichParagraph
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty left-aligned paragraph</summary>
         RichParagraph() : Align(Alignment::Left)
         {}
         /// <summary>Create empty paragraph</summary>
         /// <param name="al">alignment</param>
         RichParagraph(Alignment al) : Align(al)
         {}

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append character</summary>
         RichParagraph& operator+=(RichCharacter* ch)
         {
            Content.push_back(RichElementPtr(ch));
            return *this;
         }

         /// <summary>Append button</summary>
         RichParagraph& operator+=(RichButton* btn)
         {
            Content.push_back(RichElementPtr(btn));
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         ElementList Content;
         Alignment   Align;
      };

      /// <summary>Result of parsing a Language string with rich-text formatting</summary>
      class RichString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty string with single left-aligned paragraph</summary>
         RichString() : Columns(ColumnType::Default), Spacing(0), Width(0)
         {
            operator+=( RichParagraph(Alignment::Left) );
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append paragraph</summary>
         RichString& operator+=(const RichParagraph& p)
         {
            Paragraphs.push_back(p);
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         list<RichParagraph> Paragraphs;
         ColumnType          Columns;
         UINT                Width,
                             Spacing;
         wstring             Author,
                             Title;
      };

      /// <summary>Get paragraph alignment string</summary>
      wstring  GetString(Alignment a);

      /// <summary>Write rich-text objects to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichElement& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichCharacter& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichButton& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichParagraph& p);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichString& s);
      
      /// <summary>Rich-text parser</summary>
      class StringParser
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary>Constant character iterator</summary>
         typedef wstring::const_iterator CharIterator;

         /// <summary>Rich-text tag {name,value} property pair</summary>
         typedef pair<wstring,wstring> Property;

         /// <summary>List of {name,value} property pairs for a rich-text tag</summary>
         typedef list<Property>  PropertyList;

         /// <summary>Defines available rich edit tags</summary>
         enum class TagType { Bold, Underline, Italic, Left, Right, Centre, Justify, Text, Select, Author, Title, Rank,
                              Black, Blue, Cyan, Green, Magenta, Orange, Red, Silver, Yellow, White,
                              Unrecognised };

         /// <summary>Groups tags by category</summary>
         enum class TagClass { Character, Paragraph, Colour, Special };

         /// <summary>Any valid tag in a rich-text string</summary>
         class RichTag
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            RichTag(TagType t, bool open) : Type(t), Class(GetClass(t)), Opening(open), Closing(!open), SelfClosing(false)
            {}

            RichTag(TagType t, const wstring& txt) : Type(t), Class(TagClass::Special), Text(txt), Opening(false), Closing(false), SelfClosing(false)
            {}

            RichTag(TagType t, const PropertyList& props) : Type(t), Properties(props), Class(GetClass(t)), Opening(true), Closing(false), SelfClosing(false)
            {}

            // -------------------- REPRESENTATION ---------------------
         public:
            bool         Opening,         // Opening tag
                         Closing,         // Closing tag
                         SelfClosing;     // Self-closing tag
            
            wstring      Text;         // Title/Author/Button text
            TagType      Type;         // Tag type
            TagClass     Class;        // Class of tag
            PropertyList Properties;   // {Name,Value} property pairs
         };

         /// <summary>Stack of rich formatting tags</summary>
         class TagStack : protected deque<TagType>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create tag stack</summary>
            /// <param name="cl">The class of tags to hold</param>
            TagStack(TagClass cl) : Class(cl)
            {}

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Push or pop a tag from the stack</summary>
            /// <exception cref="Logic::ArgumentException">Tag is of the incorrect class</exception>
            /// <exception cref="Logic::Language::RichTextException">Closing tag doesn't match currently open tag</exception>
            void  PushPop(const RichTag& tag)
            {
               // Ensure tag is of correct class
               if (tag.Class != Class)
                  throw ArgumentException(HERE, L"tag", GuiString(L"Cannot add %s tag to a %s stack", GetString(tag.Type).c_str(), GetString(tag.Class).c_str()) );

               // Open: Push stack
               if (tag.Opening)
                  push_back(tag.Type);

               // Close: Pop stack
               else if (!empty() && back() == tag.Type)
                  pop_back();

               else
                  // Mismatch: Error
                  throw RichTextException(HERE, GuiString(L"Unexpected closing %s tag", GetString(tag.Type).c_str()) );
            }

            // -------------------- REPRESENTATION ---------------------
         private:
            TagClass Class;
         };

         /// <summary>Specialised character tag stack</summary>
         class CharTagStack : public TagStack
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CharTagStack() : TagStack(TagClass::Character)
            {}

            // --------------------- PROPERTIES ------------------------
         public:
            /// <summary>Generate character formatting flags from tags in stack</summary>
            PROPERTY_GET(UINT,Current,GetCurrent);

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Generate character formatting flags from tags in stack</summary>
            UINT  GetCurrent() const
            {
               UINT format = NULL;
               
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

         /// <summary>Specialised colour tag stack</summary>
         class ColourTagStack : public TagStack
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ColourTagStack() : TagStack(TagClass::Colour)
            {}
            
            // --------------------- PROPERTIES ------------------------
         public:
            /// <summary>Get colour on top of the stack, or default colour if empty</summary>
            PROPERTY_GET(Colour,Current,GetCurrent);

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Get colour on top of the stack, or default colour if empty</summary>
            /// <exception cref="Logic::AlgorithmException">Current colour tag is unrecognised</exception>
            Colour GetCurrent() const
            {
               // Default if empty
               if (empty())
                  return Colour::Default;

               // Convert tag to colour
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

               // Error
               throw AlgorithmException(HERE, L"Unrecognised colour tag on top of stack");
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

         static Alignment GetAlignment(TagType t);
         static TagClass  GetClass(TagType t);
         static wstring   GetString(TagType t);
         static wstring   GetString(TagClass t);
         static TagType   IdentifyTag(const wstring& name);

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(RichParagraph&,FirstParagraph,GetFirstParagraph);

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  MatchColourCode(CharIterator pos) const;
         bool  MatchTag(CharIterator pos) const;

      private:
         RichButton*  CreateButton(const RichTag& tag) const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Parse();

      private:
         RichParagraph& GetFirstParagraph();
         Colour  ReadColourCode(CharIterator& pos);
         RichTag ReadTag(CharIterator& pos);
         void    SetColumnInfo(const RichTag& tag);
         
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