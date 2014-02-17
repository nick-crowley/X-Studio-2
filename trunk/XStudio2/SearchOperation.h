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
         SearchOperation();
         //SearchOperation(const wstring& term, SearchTarget t) : Target(t), FileSearchData(term, t)
         //{
         //   // Feedback
         //   GuiString msg(L"Searching for '%s' in %s", term.c_str(), GetString(t).c_str() );
         //   Console << Cons::Heading << Cons::Bold << msg << ENDL;
         //   FileSearchData.SendFeedback(ProgressType::Operation, 0, msg);
         //}
         virtual ~SearchOperation();

         DEFAULT_COPY(SearchOperation);	// Default copy semantics
         DEFAULT_MOVE(SearchOperation);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  IsStarted() const
         {
         }

         // ----------------------- MUTATORS ------------------------
      public:
         
         void  Create(const wstring& term, SearchTarget t)
         {
         }

         void  FindAll()
         {
         }

         bool  FindNext()
         {
         }

         bool  Replace(const wstring& term)
         {
         }

         void  ReplaceAll(const wstring& term)
         {
         }

         void  Reset()
         {
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         DocumentList      Documents;
         SearchWorkerData  FileSearchData;
         SearchWorker      FileSearch;
         SearchTarget      Target;
      private:
      };

   }
}

using namespace GUI::Utils;
