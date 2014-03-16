#pragma once


#include <regex>

namespace Logic
{
   namespace Language
   {
      class RichElement;
      class RichCharacter;

      // ------------------------ TYPES --------------------------

      /// <summary>Column layout in a rich-text string</summary>
      enum class ColumnType { Default, Single, Double, Triple };

      /// <summary>Get column type string</summary>
      const wchar*  GetString(ColumnType t);

      /// <summary>Distinguishes characters from buttons in rich text</summary>
      enum class ElementType  { Character, Button };


      /// <summary>Shared pointer to a rich-text character/button</summary>
      typedef shared_ptr<RichElement>  RichElementPtr;

      /// <summary>List of rich-text characters/buttons</summary>
      typedef list<RichElementPtr>  ElementList;

      /// <summary>List of rich-text characters</summary>
      class LogicExport RichCharList : public list<const RichCharacter*>
      {
      public:
         RichCharList()
         {}

         /// <summary>Append another list of rich-text characters</summary>
         RichCharList& operator+=(const RichCharList& r)
         {
            insert(end(), r.begin(), r.end());
            return *this;
         }

         /// <summary>Append another list of rich-text characters</summary>
         RichCharList& operator+=(const RichCharacter& chr)
         {
            push_back(&chr);
            return *this;
         }
      };

      // ------------------------ CLASSES ------------------------

      /// <summary>Occurs when an error is detected in a rich-text string</summary>
      class LogicExport RichTextException : public ExceptionBase
      {
      public:
         /// <summary>Create a RichTextException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="msg">Message</param>
         RichTextException(wstring  src, wstring  msg) 
            : ExceptionBase(src, wstring(L"Invalid formatting: ") + msg)
         {}
      };

      /// <summary>Base class LogicExport for a Button/Character within a rich-text string</summary>
      class LogicExport RichElement
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
      class LogicExport RichCharacter : public RichElement
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
      class LogicExport RichButton : public RichElement
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RichButton(const wstring& txt) : RichElement(ElementType::Button), Text(txt)
         {}
         RichButton(const wstring& txt, const wstring& id) : RichElement(ElementType::Button), ID(id), Text(txt)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  ID,
                  Text;    // Button text - may be richText source
      };

      /// <summary>Paragraph of text within a rich-text string</summary>
      class LogicExport RichParagraph
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty paragraph</summary>
         /// <param name="al">alignment</param>
         RichParagraph(Alignment align = Alignment::Left) : Align(align)
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
      class LogicExport RichString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty string with single left-aligned paragraph</summary>
         RichString(Alignment default = Alignment::Left) : Columns(ColumnType::Default), Spacing(0), Width(0)
         {
            operator+=( RichParagraph(default) );
         }

         /// <summary>Create simple string without formatting</summary>
         explicit RichString(const wstring& text) : RichString(Alignment::Left)
         {
            for (auto& ch : text)
               GetFirstParagraph() += new RichCharacter(ch, Colour::Default, 0);
         }

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(RichCharList,Characters,GetCharacters);
         PROPERTY_GET(RichParagraph&,FirstParagraph,GetFirstParagraph);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets content as list of characters</summary>
         /// <returns></returns>
         RichCharList GetCharacters() const
         {
            RichCharList list;

            // Extract characters, drop buttons
            for (auto& para : Paragraphs)
               list += para.Characters;

            return list;
         }

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



      
      /// <summary>Write rich-text objects to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichElement& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichCharacter& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichButton& e);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichParagraph& p);
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichString& s);
      
   }
}

using namespace Logic::Language;
