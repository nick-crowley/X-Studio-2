#pragma once
#include "../Logic/SearchWorker.h"
#include "ProjectDocument.h"

namespace GUI
{
   namespace Utils
   {
      /// <summary>Performs Find&Replace on scripts currently open as documents</summary>
      class SearchOperation
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchOperation(Operation output, SearchTarget target, ProjectDocument* proj, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx);
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
