#include "stdafx.h"
#include "SearchData.h"


namespace Logic
{
      const CHARRANGE  SearchData::NO_MATCH = {-1,-1};
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Default ctor - creates a search already marked as complete</summary>
      SearchData::SearchData() : Complete(true)
      {
      }

      /// <summary>Create data for a text search</summary>
      /// <param name="txt">Search term</param>
      SearchData::SearchData(const wstring& txt)
         : Term(txt), LastMatch({0,0}), Complete(false)
      {
      }

      SearchData::~SearchData()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Determines whether this search is complete.</summary>
      /// <returns></returns>
      bool  SearchData::IsComplete() const
      {
         return Complete;
      }

      /// <summary>Gets the last match.</summary>
      /// <returns></returns>
      CHARRANGE  SearchData::GetLastMatch() const
      {
         return LastMatch;
      }


      /// <summary>Gets the search term.</summary>
      /// <returns></returns>
      wstring  SearchData::GetTerm() const
      {
         return Term;
      }

      /// <summary>Resets the last match to zero, in preparation for searching another document/file.</summary>
      void  SearchData::ResetLastMatch()
      {
         LastMatch = {0,0};
      }

      /// <summary>Marks the search as complete.</summary>
      void  SearchData::SetComplete()
      {
         Complete = true;
      }

      /// <summary>Sets the position of the last match.</summary>
      /// <param name="pos">Characater position if found, otherwise -1</param>
      void  SearchData::SetMatch(int pos)
      {
         if (pos != -1)
            LastMatch = {pos, pos+Term.length()};
         else
            LastMatch = NO_MATCH;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      
   
}

