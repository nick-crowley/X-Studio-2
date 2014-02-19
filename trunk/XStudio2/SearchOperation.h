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

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchOperation(Operation op, SearchTarget t, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx);
         virtual ~SearchOperation();

         NO_COPY(SearchOperation);	// No copy semantics
         NO_MOVE(SearchOperation);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      
         // ----------------------- MUTATORS ------------------------
      public:
         void  FindAll();
         bool  FindNext();

         bool  Replace(const wstring& term);
         void  ReplaceAll(const wstring& term);
      
      protected:
         bool  Find();

         // -------------------- REPRESENTATION ---------------------
      private:
         DocumentList      Documents;
         SearchWorkerData  Search;
         SearchTarget      Target;
      };

   }
}

using namespace GUI::Utils;
