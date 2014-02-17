#pragma once

#include "Common.h"
#include "BackgroundWorker.h"
#include "SearchData.h"

namespace Logic
{
   namespace Threads
   {
      /// <summary></summary>
      enum class SearchTarget { Selection, Document, OpenDocuments, ProjectDocuments, ScriptFolder };

      /// <summary>Get search target name</summary>
      wstring  GetString(SearchTarget t);


      /// <summary>Data for find and replace worker thread</summary>
      class SearchWorkerData : public WorkerData
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchWorkerData(const wstring& txt, SearchTarget targ) : WorkerData(Operation::FindReplace)
         {}
         SearchWorkerData(const wstring& txt, SearchTarget targ, IO::Path folder, GameVersion ver) : WorkerData(Operation::FindReplace)
         {}
         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(IO::Path,CurrentFile,GetCurrentFile);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get whether there are any more files to search.</summary>
         /// <returns></returns>
         bool  HasFiles() const
         {
            return !Files.empty();
         }

         /// <summary>Gets the full path of the current file being searched.</summary>
         /// <returns></returns>
         IO::Path  GetCurrentFile() const
         {
            return Files.front();
         }

         /// <summary>Gets the search target.</summary>
         /// <returns></returns>
         SearchTarget  GetTarget() const
         {
            return Target;
         }

         /// <summary>Get whether search is complete.</summary>
         /// <returns></returns>
         bool  IsComplete() const
         {
            return Search.IsComplete();
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Adds a file to the list of files to search.</summary>
         /// <param name="p">The p.</param>
         void  AddFile(Path p)
         {
            Files.push_back(p);
         }
         
         /// <summary>Gets the text search data.</summary>
         /// <returns></returns>
         SearchData&  GetSearchData()
         {
            return Search;
         }

         /// <summary>Advances to the next file.</summary>
         void  NextFile()
         {
            Files.pop_front();
         }

         /// <summary>Resets the last match in prepration for searching a new document.</summary>
         void  ResetLastMatch()
         {
            Search.ResetLastMatch();
         }

         /// <summary>Marks search as complete.</summary>
         void  SetComplete()
         {
            Search.SetComplete();
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         SearchData      Search;
         list<IO::Path>  Files;
         SearchTarget    Target;
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
