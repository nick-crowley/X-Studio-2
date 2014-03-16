#include "stdafx.h"
#include "DescriptionParser.h"
#include "StringResolver.h"      // RegEx exception
#include "DescriptionLibrary.h"
#include "StringLibrary.h"
#include <strsafe.h>
#include <regex>

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

      /// <summary>Matches a parameter insertion marker</summary>
      const wregex DescriptionParser::MartchParameterMarker(L"\\$(\\d)[xyzaoº¹²³ª]*");

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Parses the source text of a script object description</summary>
      /// <param name="txt">Input text.</param>
      /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
      /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
      DescriptionParser::DescriptionParser(const wstring& txt)
      {
         // Allocate formatting buffer
         FormatBuffer = CharArrayPtr(new wchar[BUFFER_LENGTH]);
         FormatBuffer.get()[0] = L'\0';

         // Parse
         Text = Parse(txt);
      }

      /// <summary>Parses the source text of a command description</summary>
      /// <param name="txt">Input text.</param>
      /// <param name="cmd">Command syntax.</param>
      /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
      /// <exception cref="Logic::IndexOutOfRangeException">Parameter does not exist</exception>
      /// <exception cref="Logic::PageNotFoundException">Parameter types Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">Parameter type string does not exist</exception>
      /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
      DescriptionParser::DescriptionParser(const wstring& txt, CommandSyntaxRef cmd) : DescriptionParser(txt)
      {
         // Populate parameters macros, then re-parse
         Text = Parse(Populate(Text, cmd));
      }

      DescriptionParser::~DescriptionParser()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Called for each occurrence of {aaa} markers</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      /// <exception cref="Logic::IndexOutOfRangeException">Parameter does not exist</exception>
      /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
      wstring  DescriptionParser::onParameterMarker(wsmatch& match, CommandSyntaxRef cmd)
      {
         UINT index = _wtoi( match[1].str().c_str() );

         // Verify index
         if (index > cmd.Parameters.size())
            throw IndexOutOfRangeException(HERE, index, cmd.Parameters.size());

         // Lookup type name
         wstring type = StringLib.Find(KnownPage::PARAMETER_TYPES, (UINT)cmd.Parameters[index].Type).Text;

         // Format appropriately
         return VString(L"{PARAMETER:%s}", type.c_str());
      }

      
      /// <summary>Populates the parameters markers within the description source text.</summary>
      /// <param name="src">source text.</param>
      /// <param name="cmd">command syntax.</param>
      /// <returns></returns>
      wstring  DescriptionParser::Populate(wstring src, CommandSyntaxRef cmd)
      {
         wsmatch match;
         wstring r;
         
         // Replace all {aaa,bbb} markers.  Manually track position for in-place replacement + avoid infinite loop
         for (int Position = 0; regex_search(src.cbegin()+Position, src.cend(), match, MartchParameterMarker); )
         {
            r = onParameterMarker(match, cmd);

#ifdef PRINT_CONSOLE            
            Console << "  Replace: " << Cons::Yellow << match[0].str() << Cons::White << " with " << Cons::Green << r << ENDL;
#endif
            // Advance position + perform replacement
            Position = (match[0].first - src.cbegin()) + r.length();
            src.replace(match[0].first, match[0].second, r);
         }

         return src;
      }


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
                  throw FileFormatException(HERE, VString(L"The macro '%s' requires %d parameters : '%s'", macro->Name.c_str(), macro->ParamCount, match[0].str().c_str()));

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

   }
}

