#include "stdafx.h"
#include "RichString.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Convert paragraph alignment string</summary>
      /// <param name="a">alignment</param>
      /// <returns></returns>
      wstring  GetString(Alignment a)
      {
         switch (a)
         {
         case Alignment::Left:    return L"Left";
         case Alignment::Centre:  return L"Centre";
         case Alignment::Right:   return L"Right";
         case Alignment::Justify: return L"Justify";
         }
         return L"Invalid";
      }
      

      /// <summary>Output rich-text element to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichElement& e)
      {
         if (e.Type == ElementType::Button)
            return c << (const RichButton&)e;

         else if (e.Type == ElementType::Character)
            return c << (const RichCharacter&)e;
         
         return c;
      }

      /// <summary>Output rich-text character to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichCharacter& e)
      {
         c << e.Colour;
         if (e.Format & CFE_BOLD)
            c << Cons::Bold;

         c << e.Char;

         if (e.Format & CFE_BOLD)
            c << Cons::Normal;

         return c;
      }

      /// <summary>Output rich-text button to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichButton& e)
      {
         c << "{" << Colour::Yellow << "RichButton" << Colour::White << " Id=" << e.ID << " Text=";

         // Text
         for (const auto& el : e.Text)
            c << *el;
         
         return c << "}";
      }

      /// <summary>Output rich-text paragraph to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichParagraph& p)
      {
         // Properties
         c << "{" << Colour::Yellow << "RichParagraph" << Colour::White << " Align=" << GetString(p.Align);

         // Elements
         c << ENDL << "Content=";
         for (const auto& el : p.Content)
            c << *el;
         
         return c << "}";
      }

      /// <summary>Output rich-text string to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichString& s)
      {
         // Properties
         c << "{" << Colour::Yellow << "RichString" << Colour::White;
         c << " Author=" << s.Author << " Title=" << s.Title << " Cols=" << (int)s.Columns << " Width=" << s.Width << " Spacing=" << s.Spacing;

         // Elements
         c << ENDL << "Paragraphs=";
         for (const auto& el : s.Paragraphs)
            c << el;
         
         return c << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   

   }
}
