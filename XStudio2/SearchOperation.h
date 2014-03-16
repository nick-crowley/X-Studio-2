#pragma once

#include "..Utils/Utils.h"
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
         SearchOperation(Operation output, SearchTarget target, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx);
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
         bool  Find(SearchCommand cmd);

         // -------------------- REPRESENTATION ---------------------
      private:
         DocumentList      Documents;
         SearchWorkerData  Search;
         SearchTarget      Target;
      };

   }
}

using namespace GUI::Utils;
