#pragma once

#include "Common.h"

namespace Logic
{
   
   /// <summary></summary>
   class MatchData
   {
      // ------------------------ TYPES --------------------------
   private:

      // --------------------- CONSTRUCTION ----------------------
   public:
      MatchData(const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
         : SearchTerm(search), 
            ReplaceTerm(replace), 
            IsMatched(false), 
            Location({0,0}), 
            LineNumber(0),
            MatchCase(matchCase), 
            MatchWord(matchWord),
            UseRegEx(regEx)
      {
      }

      //NO_COPY(MatchData);	// Default copy semantics
      //NO_MOVE(MatchData);	// Default move semantics

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------

      // ---------------------- ACCESSORS ------------------------	

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Clears the match.</summary>
      void  Clear()
      {
         FullPath = L"";
         IsMatched = false;
         Location = {0,0};
         Text.empty();
         LineNumber = 0;
      }

      /// <summary>Sets the match.</summary>
      /// <param name="start">start index.</param>
      /// <param name="length">length.</param>
      /// <param name="txt">text.</param>
      /// <param name="txt">1-based line number.</param>
      void  SetMatch(int start, int length, const wstring& txt, int line)
      {
         Location = {start, start+length};
         IsMatched = true;
         LineNumber = line;
         Text = txt;
      }

      // -------------------- REPRESENTATION ---------------------
   public:
      bool        IsMatched;
      CHARRANGE   Location;
      IO::Path    FullPath;
      int         LineNumber;
      wstring     Text;

      const wstring  SearchTerm,
                     ReplaceTerm;
      const bool     MatchCase,
                     MatchWord,
                     UseRegEx;
   };

   
}

using namespace Logic;