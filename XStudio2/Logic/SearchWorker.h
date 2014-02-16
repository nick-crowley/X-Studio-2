#pragma once

#include "Common.h"
#include "BackgroundWorker.h"
#include "SearchData.h"

namespace Logic
{
   namespace Threads
   {
      /// <summary>Data for find and replace worker thread</summary>
      class SearchWorkerData : public WorkerData
      {
      public:
         SearchWorkerData() : WorkerData(Operation::FindReplace)
         {}

         SearchData  Search;
      };

      /// <summary>Find and replace worker thread</summary>
      class SearchWorker : public BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchWorker();
         virtual ~SearchWorker();

         DEFAULT_COPY(SearchWorker);	// Default copy semantics
         DEFAULT_MOVE(SearchWorker);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      protected:
         static DWORD WINAPI ThreadMain(SearchWorkerData* data);

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         bool  Start(SearchWorkerData* param)
         {
            return BackgroundWorker::Start(param);
         }

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::Threads;
