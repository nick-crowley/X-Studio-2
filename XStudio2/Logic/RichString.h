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
      
   }
}

using namespace Logic::Language;