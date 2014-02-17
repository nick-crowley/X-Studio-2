#pragma once

#include "Common.h"

namespace Logic
{
   
      

      /// <summary></summary>
      class SearchData
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         SearchData();
         SearchData(const wstring& txt);
         virtual ~SearchData();

         DEFAULT_COPY(SearchData);	// Default copy semantics
         DEFAULT_MOVE(SearchData);	// Default move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const CHARRANGE  NO_MATCH;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool       IsComplete() const;
         CHARRANGE  GetLastMatch() const;
         wstring    GetTerm() const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  ResetLastMatch();
         void  SetComplete();
         void  SetLastMatch(int pos = -1);

         // -------------------- REPRESENTATION ---------------------
      private:
         bool        Complete;
         CHARRANGE   LastMatch;
         wstring     Term;
      };

   
}

using namespace Logic;