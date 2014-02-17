#pragma once

#include "Logic/Common.h"
#include "Logic/SearchWorker.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class SearchOperation
      {
         // ------------------------ TYPES --------------------------
      private:
         typedef unique_ptr<SearchWorkerData>  SearchData;

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchOperation();
         //SearchOperation(const wstring& term, SearchTarget t) : Target(t), Search(term, t)
         //{
         
         //}
         virtual ~SearchOperation();

         DEFAULT_COPY(SearchOperation);	// Default copy semantics
         DEFAULT_MOVE(SearchOperation);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  IsStarted() const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Create(SearchTarget t, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx);

         void  FindAll();
         bool  FindNext();

         bool  Replace(const wstring& term);
         void  ReplaceAll(const wstring& term);
         void  Reset();

      protected:
         void  OnStart();
         void  OnFinish();

         // -------------------- REPRESENTATION ---------------------
      private:
         DocumentList  Documents;
         SearchData    Search;
         SearchTarget  Target;
      };

   }
}

using namespace GUI::Utils;
