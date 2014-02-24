#include "stdafx.h"
#include "DescriptionParser.h"
#include <strsafe.h>
#include "StringResolver.h"      // RegEx exception
#include "DescriptionLibrary.h"

/// <summary>Turn console debugging output on/off</summary>
//#define PRINT_CONSOLE

namespace Logic
{
   namespace Language
   {
      /// <summary>Matches a capitalized keyword or a macro with no parameters, in both instances the name is captured</summary>
      const wregex  DescriptionParser::MatchKeyword(L"\\b([A-Z_0-9]+)(?![:\\}])\\b" L"|" L"\\{([A-Z_0-9]+)\\}");

      /// <summary>Matches a macro with one or more parameters. captures the name alone and parameters as a block</summary>
      const wregex  DescriptionParser::MatchMacro(L"\\{([A-Z_0-9]+):([^\\}]+)\\}");

      /// <summary>Matches each parameter in a comma delimited string</summary>
      const wregex  DescriptionParser::MatchParameters(L"([^,]+)((?=,)[^,]+)*");

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a parser and parses the input text</summary>
      /// <param name="txt">Input text.</param>
      /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
      /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
      DescriptionParser::DescriptionParser(const wstring& txt)
      {
         // Allocate formatting buffer
         FormatBuffer = CharArrayPtr(new wchar[BUFFER_LENGTH]);
         FormatBuffer.get()[0] = L'\0';

         // Parse
         Text = Parse(txt, 0);
         //Text = PostProcess(Text);
      }


      DescriptionParser::~DescriptionParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Called for each occurrence of parameterized macros</summary>
      /// <param name="match">The match.</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Replacement text</returns>
      /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
      /// <exception cref="std::regex_error">RegEx error</exception>
      wstring  DescriptionParser::onMatchMacro(const wsmatch& match, int depth) const 
      {
         const DescriptionMacro* macro;
         wstring name      = match[1].str(),
                 arguments = match[2].str();

         // Lookup macro 
         if (DescriptionLib.Macros.TryFind(name, macro))
         {
            // SpecialCase: If macro has one parameter then format manually (this handles HEADING, which can have commas in it's argument) 
            if (macro->ParamCount == 1)
               StringCchPrintf(FormatBuffer.get(), BUFFER_LENGTH, macro->Text.c_str(), arguments.c_str());
            else
            {
               // MACRO: Extract arguments and format them into the replacement text
               vector<wstring> arg = { L"", L"", L"", L"", L"", L"" };
               UINT i = 0;

               // Separate parameters into array
               for (wsregex_iterator m(arguments.begin(), arguments.end(), MatchParameters), end; m != end && i <= 5; ++m)
                  arg[i++] = (*m)[0].str();

               // Verify argument count
               if (macro->ParamCount != i)
                  throw FileFormatException(HERE, GuiString(L"The macro '%s' requires %d parameters : '%s'", macro->Name.c_str(), macro->ParamCount, match[0].str().c_str()));

               // Format macro with up to six parameters...
               StringCchPrintf(FormatBuffer.get(), BUFFER_LENGTH, 
                               macro->Text.c_str(), 
                               arg[0].c_str(), arg[1].c_str(), arg[2].c_str(), arg[3].c_str(), arg[4].c_str(), arg[5].c_str());
            }

#ifdef PRINT_CONSOLE
            Console << Cons::Cyan << Indent(depth) << "Matched Macro: " << Cons::Yellow << match[0].str() 
                    << Cons::White << " with " << Cons::Yellow << FormatBuffer.get() << ENDL;
#endif
            // Recursively parse
            return Parse(FormatBuffer.get(), depth+1);
         }

#ifdef PRINT_CONSOLE
         Console << Cons::Red << Indent(depth) << "Ignored Macro: " << Cons::White << match[0].str() << ENDL;
#endif

         // Failed: Return verbatim
         return match[0].str();
      }

      /// <summary>Called for each occurrence of parameterless macros</summary>
      /// <param name="match">The match.</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Replacement text</returns>
      wstring  DescriptionParser::onMatchKeyword(const wsmatch& match, int depth) const 
      {
         const DescriptionMacro* m;
         wstring name = (match[1].matched ? match[1].str() : match[2].str()); // Retrieve keyword/macro
         
         // Lookup macro + recursively parse
         if (DescriptionLib.Macros.TryFind(name, m))
         {
#ifdef PRINT_CONSOLE
            Console << Cons::Cyan << Indent(depth) << "Matched Keyword: " << Cons::Yellow << match[0].str()
                    << Cons::White << " with " << Cons::Yellow << m->Text << ENDL;
#endif

            return m->Recursive ? Parse(m->Text, depth+1) : m->Text;
         }

#ifdef PRINT_CONSOLE
         Console << Cons::Red << Indent(depth) << "Ignored Keyword: " << Cons::White << match[0].str() << ENDL;
#endif

         // Failed: Return match
         return match[0].str();
      }


      /// <summary>Parses all the keywords/macros in a string and replaces them recursively</summary>
      /// <param name="text">Source text to parse</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Fully parsed text</returns>
      /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
      /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
      wstring  DescriptionParser::Parse(wstring text, int depth) const 
      {
         UINT Position;
         wsmatch match;
         wstring r;
         
         try
         {
#ifdef PRINT_CONSOLE
            Console << Cons::Cyan << Indent(depth) << "Parsing: " << Cons::White << text << ENDL;
#endif

            // Find/Replace all macros:  {AAAA:bbb}, {AAAA:bbb,ccc}, {AAAA:bbb,ccc,ddd} ...
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchMacro); ) //  Manually track position for in-place replacement + avoid infinite loop
            {
               // Recursively generate replacement text
               r = onMatchMacro(match, depth);

#ifdef PRINT_CONSOLE
               Console << Indent(depth) << "Replacing text: " << Cons::Yellow << match[0].str() << Cons::White << " with " << Cons::Green << r << ENDL;
#endif

               // Advance position to beyond inserted text, and insert text
               Position = (match[0].first - text.cbegin()) + r.length();
               text.replace(match[0].first, match[0].second, r);
            }

            // Find/Replace all keywords:  {AAAAA}, AAAAA
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchKeyword); )  
            {
               // Recursively generate replacement text
               r = onMatchKeyword(match, depth);

#ifdef PRINT_CONSOLE
               Console << Indent(depth) << "Replacing text: " << Cons::Yellow << match[0].str() << Cons::White << " with " << Cons::Green << r << ENDL;
#endif

               // Advance position to beyond inserted text, and insert text
               Position = (match[0].first - text.cbegin()) + r.length();
               text.replace(match[0].first, match[0].second, r);
            }

            return text;
         }
         catch (regex_error& e) {
            throw RegularExpressionException(HERE, e);
         }
      }

      /// <summary>Posts the process.</summary>
      /// <param name="text">The text.</param>
      /// <returns></returns>
      wstring  DescriptionParser::PostProcess(wstring text) const
      {
         list<wchar> chars(text.begin(), text.end());

         for (auto ch = chars.begin(); ch != chars.end(); ++ch)
         {
            if (*ch == '\\')
            {
               auto prev = ch++;
               switch (*ch)
               {
               case '(':
               case ')':
               case '{':
               case '}':
               case '[':
               case ']':
               case '\\':
                  chars.erase(prev);
                  break;
               }
            }
         }

         return wstring(chars.begin(), chars.end());
      }
   }
}

