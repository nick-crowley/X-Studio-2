#include "stdafx.h"
#include "StringParser.h"
#include "LanguagePage.h"
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

      /// <summary>Create a parser for a language string</summary>
      /// <param name="str">The string.</param>
      StringParser::StringParser(const LanguageString& str) : Text(str.Text), Page(str.Page)
      {
         //Console << "Parsing " << Colour::Yellow << str.Text << ENDL;

         // Skip parsing if no brackets are present
         if (Text.find_first_of(L"{(") != wstring::npos)
            Parse();
      }


      StringParser::~StringParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Called for each occurrence of {aaa,bbb} markers</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  StringParser::OnFullMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[2].str().c_str() ),
              page = _wtoi( match[1].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(page, id) ? StringLib.Resolve(page, id) : match[0].str();
      }

      /// <summary>Called for each occurrence of {aaa} markers</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  StringParser::OnDefaultMarker(wsmatch& match)
      {
         UINT id   = _wtoi( match[1].str().c_str() );

         // Insert string if present, otherwise keep original marker
         return StringLib.Contains(Page, id) ? StringLib.Resolve(Page, id) : match[0].str();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Parses the instance text.</summary>
      void  StringParser::Parse()
      {
         wsmatch match;
         wstring r;
         
         // Replace all {aaa,bbb} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, FullMarker); Position += r.length())
         {
            r = OnFullMarker(match);
            //Console << "  Replace: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;
            Text.replace(match[0].first, match[0].second, r);
         }

         // Replace all {aaa} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (Position = 0; regex_search(Text.cbegin()+Position, Text.cend(), match, DefaultMarker); Position += r.length())
         {
            r = OnDefaultMarker(match);
            //Console << "  Replace: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;
            Text.replace(match[0].first, match[0].second, r);
         }

         // Remove all (aaa) markers
         while (regex_search(Text.cbegin(), Text.cend(), match, RemoveComment))
         {
            //Console << "  Remove: " << Colour::Red << match[0].str() << ENDL;
            Text.erase(match[0].first, match[0].second);
         }
      }
   }
}
