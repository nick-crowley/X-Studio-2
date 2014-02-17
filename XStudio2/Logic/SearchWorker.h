#pragma once

#include "Common.h"
#include "BackgroundWorker.h"
#include "SearchData.h"

namespace Logic
{
   namespace Threads
   {
      /// <summary></summary>
      enum class SearchTarget { Selection, Document, OpenDocuments, ProjectFiles, ScriptFolder };

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
         PROPERTY_GET_SET(bool,Initialized,GetInitialized,SetInitialized);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets the initialized flag</summary>
         /// <returns></returns>
         bool  GetInitialized() const
         {
            return ListBuilt;
         }

         /// <summary>Get whether there are any more files to search.</summary>
         /// <returns></returns>
         bool  HasCurrentFile() const
         {
            return !Files.empty();
         }

         /// <summary>Gets the full path of the current file being searched.</summary>
         /// <returns></returns>
         IO::Path  GetCurrentFile() const
         {
            return Files.front();
         }

         IO::Path  GetFolder() const
         {
            return Folder;
         }

         SearchTarget  GetTarget() const
         {
            return Target;
         }

         GameVersion  GetVersion() const
         {
            return Version;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Add path to the list of files to search.</summary>
         /// <param name="p">The p.</param>
         void  AddFile(IO::Path p)
         {
            Files.push_back(p);
         }
         
         /// <summary>Advances to the next file path.</summary>
         void  Advance()
         {
            Files.pop_front();
         }

         /// <summary>Initializes the match with the current file path</summary>
         void  InitMatch()
         {
            Match.Reset();
            Match.FullPath = CurrentFile;
         }

         /// <summary>Sets the initialized flag.</summary>
         /// <param name="b">The b.</param>
         void  SetInitialized(bool b)
         {
            ListBuilt = b;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         SearchData   Match;
         
      private:
         list<IO::Path>  Files;
         IO::Path        Folder;
         bool            ListBuilt;
         SearchTarget    Target;
         GameVersion     Version;
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
