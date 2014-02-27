#pragma once

#include "Common.h"
#include <regex>

namespace Logic
{
   namespace Language
   {
      class RichElement;
      class RichCharacter;

      // ------------------------ TYPES --------------------------

      /// <summary>Column spacing of a rich-text string</summary>
      enum class ColumnType { Default, Single, Double, Triple };

      /// <summary>Distinguishes characters from buttons in rich text</summary>
      enum class ElementType  { Character, Button };


      /// <summary>Shared pointer to a rich-text character/button</summary>
      typedef shared_ptr<RichElement>  RichElementPtr;

      /// <summary>List of rich-text characters/buttons</summary>
      typedef list<RichElementPtr>  ElementList;

      /// <summary>List of rich-text characters/buttons</summary>
      typedef list<const RichCharacter*>  RichCharList;

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
         virtual ~RichElement()
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

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(bool,Bold,IsBold);
         PROPERTY_GET(bool,Italic,IsItalic);
         PROPERTY_GET(bool,Underline,IsUnderline);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get whether character is bold</summary>
         bool  IsBold() const
         {
            return (Format & CFE_BOLD) != 0;
         }

         /// <summary>Get whether character is italicised</summary>
         bool  IsItalic() const
         {
            return (Format & CFE_ITALIC) != 0;
         }

         /// <summary>Get whether character is underlined</summary>
         bool  IsUnderline() const
         {
            return (Format & CFE_UNDERLINE) != 0;
         }

         // ----------------------- MUTATORS ------------------------

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

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(RichCharList,Characters,GetCharacters);

         // ---------------------- ACCESSORS ------------------------			

         /// <summary>Get whether empty</summary>
         bool empty() const
         {
            return Content.empty();
         }

         /// <summary>Gets content as list of characters</summary>
         /// <returns></returns>
         RichCharList GetCharacters() const
         {
            RichCharList list;

            // Extract characters, drop buttons
            for (auto& el : Content)
               if (el->Type == ElementType::Character)
                  list.push_back(dynamic_cast<RichCharacter*>(el.get()));

            return list;
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

         /// <summary>Create string from existing list of characters [Used for rendering button text]</summary>
         RichString(const ElementList& content) : RichString()
         {
            FirstParagraph.Content = content;
            FirstParagraph.Align = Alignment::Centre;
         }

         /// <summary>Create simple string without formatting</summary>
         RichString(const wstring& text) : RichString()
         {
            for (auto& ch : text)
               GetFirstParagraph() += new RichCharacter(ch, Colour::Default, 0);
         }

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(RichParagraph&,FirstParagraph,GetFirstParagraph);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get first paragraph</summary>
         const RichParagraph& GetFirstParagraph() const
         {
            return Paragraphs.front();
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Get first paragraph</summary>
         RichParagraph& GetFirstParagraph()
         {
            return Paragraphs.front();
         }

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



      class RichPhrase;

      /// <summary>Shared pointer to a font</summary>   
      typedef shared_ptr<CFont>  FontPtr;

      /// <summary>List of phrases</summary>   
      typedef list<RichPhrase>  PhraseList;

      /// <summary>Phrase list iterator</summary>   
      typedef PhraseList::iterator  PhraseIterator;
         
      /// <summary>Block of text with contiguous formatting</summary>   
      class RichPhrase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RichPhrase() : Format(NULL), Colour(Colour::Default), Skip(false), Font(nullptr)
         {}
         RichPhrase(RichCharacter ch) : Format(ch.Format), Colour(ch.Colour), Skip(false), Rect(0,0,0,0), Font(nullptr)
         {
            Text.push_back(ch.Char);
         }
         // ------------------------ STATIC -------------------------
      private:
         const static COLORREF  Black = RGB(0,0,0),     White  = RGB(255,255,255), Cyan   = RGB(42,168,200), Blue   = RGB(25,55,242), 
                                Green = RGB(13,191,25), Grey   = RGB(128,128,128), Orange = RGB(213,145,28), Purple = RGB(200,36,167), 
                                Red   = RGB(180,37,37), Silver = RGB(233,233,233), Yellow = RGB(180,180,0),  Default = RGB(200,200,200);

         /// <summary>Convert colour enumeration to RGB tooltip colour</summary>
         /// <param name="c">colour</param>
         /// <returns></returns>
         static COLORREF ToRGB(Colour c)
         {
            switch (c)
            {
            case Colour::Black:   return Black;
            case Colour::Default: return Default;
            case Colour::White:   return White;
            case Colour::Silver:  return Silver;

            case Colour::Grey:    return Grey;
            case Colour::Blue:    return Blue;
            case Colour::Cyan:    return Cyan;
            case Colour::Green:   return Green;
            case Colour::Orange:  return Orange;
            case Colour::Purple:  return Purple;
            case Colour::Red:     return Red;
            case Colour::Yellow:  return Yellow;
            }

            throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised colour enumeration: %d", c));
         }

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether empty.</summary>
         /// <returns></returns>
         bool  Empty() const
         {
            return Text.empty();
         }

         /// <summary>Gets the RGB colour</summary>
         /// <param name="invert">invert black/white for a dark background.</param>
         /// <returns></returns>
         COLORREF  GetColour(bool invert) const
         {
            Logic::Colour col = Colour;

            // Invert black/white
            if (invert)
               switch (Colour)
               {
               case Colour::Default:
               case Colour::White:   
               case Colour::Silver:  col = Colour::Black;  break;
               case Colour::Black:   col = Colour::White;  break;
               }
               
            // Set colour
            return ToRGB(col);
         }

         /// <summary>Creates a new font to represent this phrase</summary>
         /// <param name="from">Basis for new font.</param>
         /// <returns></returns>
         FontPtr GetFont(const LOGFONT& from) const
         {
            FontPtr f = FontPtr(new CFont());
            LOGFONT lf = from;

            // Convert formatting into properties
            lf.lfItalic = (Format & CFE_ITALIC ? TRUE : FALSE);
            lf.lfWeight = (Format & CFE_BOLD ? FW_BOLD : FW_NORMAL);
            lf.lfUnderline = (Format & CFE_UNDERLINE ? TRUE : FALSE);

            // Create font
            f->CreateFontIndirectW(&lf);
            return f;
         }

         /// <summary>Gets the text extent.</summary>
         /// <param name="pDC">The dc.</param>
         /// <returns></returns>
         CSize  GetSize(CDC* pDC) const
         {
            return pDC->GetTextExtent(Text.c_str(), Text.length());
         }

         /// <summary>Determines whether text is whitespace.</summary>
         /// <returns></returns>
         bool IsWhitespace() const
         {
            for (auto ch : Text)
               if (!iswspace(ch))  
                  return false;

            return true;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Clears this instance.</summary>
         void  Clear()
         {
            Text.clear();
            Colour = Colour::Default;
            Format = NULL;
         }

         /// <summary>Offsets the drawing rectangle horizontally.</summary>
         /// <param name="cx">Distance.</param>
         void  Offset(int cx)
         {
            Rect.OffsetRect(cx, 0);
         }

         RichPhrase& operator+=(wchar ch)
         {
            Text += ch;
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring Text;        // Text
         UINT    Format;      // Character formatting
         Colour  Colour;      // Colour
         CRect   Rect;        // Rendering rectangle
         bool    Skip;        // Whether to skip rendering of this token  [whitespace at start of line]
         FontPtr Font;        // Font used for rendering
      };

      
      /// <summary>Write rich-text objects to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichElement& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichCharacter& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichButton& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichParagraph& p);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichString& s);
      
   }
}

using namespace Logic::Language;
