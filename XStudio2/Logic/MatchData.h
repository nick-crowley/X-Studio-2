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
            MatchCase(matchCase), 
            MatchWord(matchWord),
            UseRegEx(regEx)
      {}

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
      }

      /// <summary>Sets the match.</summary>
      /// <param name="start">The start.</param>
      /// <param name="end">The end.</param>
      void  SetMatch(int start, int end)
      {
         Location = {start, end};
         IsMatched = true;
      }

      // -------------------- REPRESENTATION ---------------------
   public:
      bool        IsMatched;
      CHARRANGE   Location;
      IO::Path    FullPath;

      const wstring  SearchTerm,
                     ReplaceTerm;
      const bool     MatchCase,
                     MatchWord,
                     UseRegEx;
   };

   
}

using namespace Logic;