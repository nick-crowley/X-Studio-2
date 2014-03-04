#include "stdafx.h"
#include "RichString.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

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
         c << "{" << Cons::Yellow << "RichButton" << Cons::White << " Id=" << e.ID << " Text=";

         // Text
         for (auto& chr : e.Text)
            c << chr;
         
         return c << "}";
      }

      /// <summary>Output rich-text paragraph to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichParagraph& p)
      {
         // Properties
         c << "{" << Cons::Yellow << "RichParagraph" << Cons::White << " Align=" << GetString(p.Align);

         // Elements
         c << " Content=";
         for (const auto& el : p.Content)
            c << *el;
         
         return c << "}";
      }

      /// <summary>Output rich-text string to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const RichString& s)
      {
         // Properties
         c << "{" << Cons::Yellow << "RichString" << Cons::White;
         c << " Author=" << s.Author << " Title=" << s.Title << " Cols=" << (int)s.Columns << " Width=" << s.Width << " Spacing=" << s.Spacing;

         // Elements
         c << ENDL << "Paragraphs=";
         for (const auto& para : s.Paragraphs)
            c << para << ENDL;
         
         return c << "}";
      }

      /// <summary>Gets column type string.</summary>
      /// <param name="t">The type.</param>
      /// <returns></returns>
      const wchar*  GetString(ColumnType t)
      {
         switch (t)
         {
         case ColumnType::Default:  return L"Default";
         case ColumnType::Single:   return L"Single";
         case ColumnType::Double:   return L"Double";
         case ColumnType::Triple:   return L"Triple";
         }
         return L"Unknown";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   

   }
}
