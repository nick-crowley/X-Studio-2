#include "stdafx.h"
#include "StringParser.h"
#include "LanguageString.h"
#include "StringLibrary.h"

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

      const wregex StringParser::FullMarker(L"\\{(\\d+),(\\d+)\\}"), 
                   StringParser::DefaultMarker(L"\\{(\\d+)\\}"),
                   StringParser::RemoveComment(expr);

      // -------------------------------- CONSTRUCTION --------------------------------

      StringParser::StringParser(const LanguageString& str, UINT page) : Text(str.Text), Page(page)
      {
         Parse();
      }


      StringParser::~StringParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      wstring  StringParser::OnFullMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[1].str().c_str() ),
              page = _wtoi( match[2].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(page, id) ? StringLib.Find(page, id).Text : match[0].str();
      }

      wstring  StringParser::OnDefaultMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[1].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(Page, id) ? StringLib.Find(Page, id).Text : match[0].str();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------

      void  StringParser::Parse()
      {
         wsmatch match;
         wstring r;
         
         // Replace all {aaa,bbb} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, FullMarker); Position += r.length())
         {
            r = OnFullMarker(match);
            Text.replace(match[0].first, match[0].second, r);
         }

         // Replace all {aaa} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, DefaultMarker); Position += r.length())
         {
            r = OnDefaultMarker(match);
            Text.replace(match[0].first, match[0].second, r);
         }

         // Remove all (aaa) markers
         while (regex_search(Text.cbegin(), Text.cend(), match, DefaultMarker))
            Text.erase(match[0].first, match[0].second);
      }
   }
}
