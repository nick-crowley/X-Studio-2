#pragma once

#include "Common.h"
#include "BackgroundWorker.h"
#include "MatchData.h"

namespace Logic
{
   namespace Threads
   {
      /// <summary></summary>
      enum class SearchTarget { Selection, Document, OpenDocuments, ProjectFiles, ScriptFolder };

      /// <summary>Get search target name</summary>
      wstring  GetString(SearchTarget t);

      class SearchWorker;

      /// <summary>Data for find and replace worker thread</summary>
      class SearchWorkerData : public WorkerData
      {
         friend class SearchWorker;
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchWorkerData(SearchTarget targ, const MatchData& search, IO::Path folder, GameVersion ver) 
            : WorkerData(Operation::FindReplace), 
              Folder(folder), 
              Version(ver), 
              Target(targ), 
              Match(search), 
              Initialized(false)
         {}
         virtual ~SearchWorkerData()
         {}

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         IO::Path        Folder;
         MatchData       Match;
         SearchTarget    Target;
         GameVersion     Version;
         
      private:
         bool            Initialized;
         list<IO::Path>  Files;
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
      protected:
         static void  BuildFileList(SearchWorkerData* data);
         static DWORD WINAPI ThreadMain(SearchWorkerData* data);

         // --------------------- PROPERTIES ------------------------
      
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
