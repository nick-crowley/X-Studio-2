#pragma once

#include "Common.h"
#include <regex>
#include "StringResolver.h"      // RegEx Exception

namespace Logic
{
   
   /// <summary>Area being searched</summary>
   enum class SearchTarget { Selection, Document, OpenDocuments, ProjectFiles, ScriptFolder };

   /// <summary></summary>
   class MatchData
   {
      // ------------------------ TYPES --------------------------
   private:

      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create empty match data</summary>
      /// <param name="target">search target.</param>
      /// <param name="search">search term or expression.</param>
      /// <param name="replace">replacement term or expression</param>
      /// <param name="matchCase">match case flag.</param>
      /// <param name="matchWord">match whole word flag.</param>
      /// <param name="regEx">Whether terms are strings or regEx</param>
      /// <exception cref="Logic::Language::RegularExpressionException">Error in expression</exception>
      MatchData(SearchTarget target, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
         : Target(target),
           SearchTerm(search), 
           ReplaceTerm(replace), 
           IsMatched(false), 
           Location({0,0}), 
           LineNumber(0),
           SearchRange({0,0}),
           MatchCase(regEx ? matchCase : false), 
           MatchWord(regEx ? matchWord : false),
           UseRegEx(regEx),
           RegEx(GetSafeRegEx(regEx ? search : L""))
      {
         /*if (MatchWord && UseRegEx)
            throw NotSupportedException(HERE, L"Can't use match whole word only and regular expressions together. Use \\b.");*/
      }

      NO_MOVE(MatchData);	      // Immutable
      NO_COPY_ASSIGN(MatchData);	// Immutable

      // ------------------------ STATIC -------------------------
   private:
      /// <summary>Creates a regular expression.</summary>
      /// <param name="txt">expression</param>
      /// <returns></returns>
      /// <exception cref="Logic::Language::RegularExpressionException">Error in expression</exception>
      wregex  GetSafeRegEx(const wstring& txt)
      {
         try {
            return txt.length() ? wregex(txt) : wregex();
         }
         catch (regex_error& e) {
            throw RegularExpressionException(HERE, e);
         }
      }

      // --------------------- PROPERTIES ------------------------

      // ---------------------- ACCESSORS ------------------------	
   public:
      /// <summary>Get the length of the match</summary>
      /// <returns></returns>
      UINT  Length() const
      {
         return Location.cpMax - Location.cpMin;
      }

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Clears the match.</summary>
      void  Clear()
      {
         FullPath = L"";
         IsMatched = false;
         Location = {0,0};
         LineText.empty();
         LineNumber = 0;
      }

      /// <summary>Sets the match.</summary>
      /// <param name="start">start index.</param>
      /// <param name="length">length.</param>
      /// <param name="line">1-based line number.</param>
      void  SetMatch(int start, int length, int line, wstring lineText)
      {
         Location = {start, start+length};
         IsMatched = true;
         LineNumber = line;
         LineText = lineText;
      }

      // -------------------- REPRESENTATION ---------------------
   public:
      bool        IsMatched;        // Whether match was successful
      CHARRANGE   Location,         // Location of match
                  SearchRange;      // Range of Text selection to search
      IO::Path    FullPath;         // File: Fullpath   Document: Title
      int         LineNumber;       // 1-based line number of match
      wstring     LineText;         // Text of line containing match

      const SearchTarget Target;        // Search target
      const wstring      SearchTerm,    // Search term/expression
                         ReplaceTerm;   // Replacement term/expression
      const bool         MatchCase,     // Match case 
                         MatchWord,     // Match whole word
                         UseRegEx;      // Search and replacement terms are regular expressions
      const wregex       RegEx;         // Search RegEx, if 'UseRegEx'
   };

   
}

using namespace Logic;