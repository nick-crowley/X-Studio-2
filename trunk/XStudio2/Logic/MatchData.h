#pragma once

#include "Common.h"
#include <regex>
#include "StringResolver.h"      // RegEx Exception

namespace Logic
{
   /// <summary>Compare character ranges</summary>
   bool operator==(const CHARRANGE& a, const CHARRANGE& b)
   {
      return a.cpMax == b.cpMax && a.cpMin == b.cpMin;
   }

   /// <summary>Compare character ranges</summary>
   bool operator!=(const CHARRANGE& a, const CHARRANGE& b)
   {
      return a.cpMax != b.cpMax || a.cpMin != b.cpMin;
   }

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
           _LineNumber(0),
           SearchRange({0,0}),
           MatchCase(regEx ? matchCase : false), 
           MatchWord(regEx ? matchWord : false),
           UseRegEx(regEx),
           RegEx(GetSafeRegEx(regEx ? search : L""))
      {
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
   public:
      PROPERTY_GET(long,Start,GetStart);
      PROPERTY_GET(long,End,GetEnd);
      PROPERTY_GET(IO::Path,FullPath,GetFullPath);
      PROPERTY_GET(UINT,Length,GetLength);
      PROPERTY_GET(UINT,LineNumber,GetLineNumber);
      PROPERTY_GET(wstring,LineText,GetLineText);
      PROPERTY_GET(bool,Matched,GetMatched);
      PROPERTY_GET(long,RangeStart,GetRangeStart);
      PROPERTY_GET(long,RangeEnd,GetRangeEnd);

      // ---------------------- ACCESSORS ------------------------	
   public:
      /// <summary>Compares a range exactly with the location of the match.</summary>
      /// <param name="cr">range.</param>
      /// <returns></returns>
      bool  Compare(CHARRANGE cr) const
      {
         return Location == cr;
      }

      /// <summary>Get the start index of the match</summary>
      /// <returns></returns>
      long  GetStart() const
      {
         return Location.cpMin;
      }

      /// <summary>Get the finish index of the match</summary>
      /// <returns></returns>
      long  GetEnd() const
      {
         return Location.cpMax;
      }

      /// <summary>Get the path or document title</summary>
      /// <returns></returns>
      IO::Path  GetFullPath() const
      {
         return _FullPath;
      }

      /// <summary>Get the length of the match</summary>
      /// <returns></returns>
      UINT  GetLength() const
      {
         return Location.cpMax - Location.cpMin;
      }
      
      /// <summary>Get the 1-based line number</summary>
      /// <returns></returns>
      UINT  GetLineNumber() const
      {
         return _LineNumber;
      }

      /// <summary>Get the line text</summary>
      /// <returns></returns>
      wstring  GetLineText() const
      {
         return _LineText;
      }
      
      /// <summary>Get whether match was successful</summary>
      /// <returns></returns>
      bool  GetMatched() const
      {
         return IsMatched;
      }

      /// <summary>Get the start index of the match</summary>
      /// <returns></returns>
      long  GetRangeStart() const
      {
         return SearchRange.cpMin;
      }

      /// <summary>Get the finish index of the match</summary>
      /// <returns></returns>
      long  GetRangeEnd() const
      {
         return SearchRange.cpMax;
      }

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Clears the match.</summary>
      void  Clear()
      {
         _FullPath   = L"";
         IsMatched   = false;
         Location    = {0,0};
         _LineNumber = 0;
         _LineText.empty();
      }

      /// <summary>Clears only the location of the match.</summary>
      void  ClearLocation()
      {
         Location = {0,0};
      }

      
      /// <summary>Sets the match.</summary>
      /// <param name="start">start index.</param>
      /// <param name="length">length.</param>
      /// <param name="line">1-based line number.</param>
      /// <param name="lineText">Line text.</param>
      void  SetMatch(int start, int length, int line, wstring lineText)
      {
         Location    = {start, start+length};
         IsMatched   = true;
         _LineNumber = line;
         _LineText   = lineText;
      }

      /// <summary>Sets the file-path or document title</summary>
      /// <param name="p">path/title.</param>
      void  SetPath(IO::Path p)
      {
         _FullPath = p;
      }

      /// <summary>Sets the range of the text selection being searched</summary>
      /// <param name="cr">range.</param>
      void  SetRange(CHARRANGE cr)
      {
         // Ensure 'TextSelection'
         if (Target != SearchTarget::Selection)
            throw InvalidOperationException(HERE, L"Search target must be text-selection to set a range");

         // Set range. Set initial match to start of range.
         SearchRange = cr;
         Location = {cr.cpMin, cr.cpMin};
      }

      /// <summary>Updates the line text after a replacement.</summary>
      /// <param name="lineText">Line text.</param>
      void  UpdateLineText(wstring lineText)
      {
         _LineText = lineText;
      }

      /// <summary>Updates the range of the text selection, in response to making text replacements</summary>
      /// <param name="start">start.</param>
      /// <param name="end">end.</param>
      void  UpdateRange(long start, long end)
      {
         // Ensure 'TextSelection'
         if (Target != SearchTarget::Selection)
            throw InvalidOperationException(HERE, L"Search target must be text-selection to set a range");

         // Set range
         SearchRange = {start, end};
      }

      // -------------------- REPRESENTATION ---------------------
   public:
      const SearchTarget Target;        // Search target
      const wstring      SearchTerm,    // Search term/expression
                         ReplaceTerm;   // Replacement term/expression
      const bool         MatchCase,     // Match case 
                         MatchWord,     // Match whole word
                         UseRegEx;      // Search and replacement terms are regular expressions
      const wregex       RegEx;         // Search RegEx, if 'UseRegEx'


   private:
      bool        IsMatched;         // Whether match was successful
      CHARRANGE   Location,          // Location of match
                  SearchRange;       // Range of Text selection to search
      int         _LineNumber;       // 1-based line number of match
      wstring     _LineText;         // Text of line containing match
      IO::Path    _FullPath;         // File: Fullpath   Document: Title
   };

   
}

using namespace Logic;