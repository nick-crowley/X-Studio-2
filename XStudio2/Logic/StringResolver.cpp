#include "stdafx.h"
#include "StringResolver.h"
#include "LanguagePage.h"
#include "StringLibrary.h"
#include <initializer_list>

// Define reg.ex characters that are difficult to understand using C-style escape codes
#define open_bracket   L"\\("
#define close_bracket  L"\\)"
#define backslash      L"\\\\"

namespace Logic
{
   namespace Language
   {
      const wchar* expr = L"(?!" backslash L")" open_bracket 
                          L"[^" open_bracket close_bracket backslash L"]+" 
                          L"(?!" backslash close_bracket L")" close_bracket;

      const wregex StringResolver::FullMarker(L"\\{(\\d+),(\\d+)\\}"), 
                   StringResolver::DefaultMarker(L"\\{(\\d+)\\}"),
                   StringResolver::RemoveComment(expr);

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a parser for a language string</summary>
      /// <param name="str">The string.</param>
      StringResolver::StringResolver(const LanguageString& str) : Text(str.Text), Page(str.Page)
      {
         //Console << "Parsing " << Cons::Yellow << str.Text << ENDL;

         // Skip parsing if no brackets are present
         if (Text.find_first_of(L"{()}") != wstring::npos)
            Parse();
      }


      StringResolver::~StringResolver()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Called for each occurrence of {aaa,bbb} markers</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  StringResolver::OnFullMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[2].str().c_str() ),
              page = _wtoi( match[1].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(page, id) ? StringLib.Resolve(page, id) : match[0].str();
      }

      /// <summary>Called for each occurrence of {aaa} markers</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  StringResolver::OnDefaultMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[1].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(Page, id) ? StringLib.Resolve(Page, id) : match[0].str();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Parses the instance text.</summary>
      void  StringResolver::Parse()
      {
         wsmatch match;
         wstring r;
         
         // Replace all {aaa,bbb} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, FullMarker); Position += r.length())
         {
            r = OnFullMarker(match);
            //Console << "  Replace: " << Cons::Yellow << match[0].str() << Cons::White << " with " << Cons::Green << r << ENDL;
            Text.replace(match[0].first, match[0].second, r);
         }

         // Replace all {aaa} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, DefaultMarker); Position += r.length())
         {
            r = OnDefaultMarker(match);
            //Console << "  Replace: " << Cons::Yellow << match[0].str() << Cons::White << " with " << Cons::Green << r << ENDL;
            Text.replace(match[0].first, match[0].second, r);
         }

         // Remove all (aaa) markers
         while (regex_search(Text.cbegin(), Text.cend(), match, RemoveComment))
         {
            //Console << "  Remove: " << Cons::Red << match[0].str() << ENDL;
            Text.erase(match[0].first, match[0].second);
         }

         // De-escape brackets
         for (auto chr : {L"\\(", L"\\)", L"\\{", L"\\}"})
            for (int pos = Text.find(chr); pos != wstring::npos; pos = Text.find(chr))
               Text.erase(pos, 1);
      }
   }
}
