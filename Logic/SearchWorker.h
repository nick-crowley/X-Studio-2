#pragma once

#include "BackgroundWorker.h"
#include "MatchData.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace Threads
   {
      /// <summary>Forward declaration</summary>
      class SearchWorker;

      /// <summary>Type of search being performed</summary>
      enum class SearchCommand { Find, Replace, FindAll, ReplaceAll };

      /// <summary>Get search target name</summary>
      LogicExport wstring  GetString(SearchTarget t);

      

      /// <summary>Data for find and replace worker thread</summary>
      class LogicExport SearchWorkerData : public WorkerData
      {
         friend class SearchWorker;
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Initializes a new instance of the <see cref="SearchWorkerData"/> class.</summary>
         /// <param name="op">operation.</param>
         /// <param name="targ">search target.</param>
         /// <param name="proj">project file  (only required for project searches).</param>
         /// <param name="search">search term.</param>
         /// <exception cref="Logic::ArgumentNullException">Missing project file when target is project files</exception>
         /// <exception cref="Logic::InvalidOperationException">Target not project or script folder</exception>
         SearchWorkerData(Threads::Operation op, SearchTarget targ, ProjectFile* proj, const MatchData& search)
            : WorkerData(op), 
              Project(proj),
              Target(targ), 
              Match(search), 
              Initialized(false),
              Command(SearchCommand::Find)
         {
            switch (Target)
            {
            // Document Based: Invalid
            case SearchTarget::Selection:
            case SearchTarget::Document:
            case SearchTarget::OpenDocuments:
               throw InvalidOperationException(HERE, L"Unsupported target type");

            case SearchTarget::ProjectFiles:
               REQUIRED(proj);
            }
         }

         virtual ~SearchWorkerData()
         {}

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Feedbacks the error.</summary>
         /// <param name="msg">The MSG.</param>
         void  FeedbackError(wstring msg)
         {
            SendFeedback(ProgressType::Error, 1, msg);
         }

         /// <summary>Feedbacks finish notification.</summary>
         void  FeedbackFinish(bool cancelled)
         {
            if (!cancelled)
               SendFeedback(Cons::UserAction, ProgressType::Succcess, 0, L"No more matches");
            else
               SendFeedback(Cons::UserAction, ProgressType::Failure, 0, L"Operation cancelled");
         }

         /// <summary>Feedbacks the match.</summary>
         void  FeedbackMatch()
         {
            SendFeedback(ProgressType::Info, 1, VString(L"%s (%d) : %s", Match.FullPath.FileName.c_str(), 
                                                                           Match.LineNumber, 
                                                                           Match.LineText.c_str()));
         }

         /// <summary>Feedbacks start notification.</summary>
         void  FeedbackStart()
         {
            // Feedback
            auto txt = (Match.UseRegEx ? L"Searching for '%s' in %s using regular expressions" : L"Searching for '%s' in %s");
            SendFeedback(ProgressType::Operation, 0, VString(txt, Match.SearchTerm.c_str(), GetString(Target).c_str()) );
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         MatchData      Match;
         SearchTarget   Target;
         SearchCommand  Command;
         
      private:
         bool         Initialized;
         ProjectFile* Project;
         list<Path>   Files;
      };

      /// <summary>Worker thread for performing Find&Replace on script files that are not currently open as documents</summary>
      class LogicExport SearchWorker : public BackgroundWorker
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
         /// <summary>Starts the thread.</summary>
         /// <param name="param">operation data.</param>
         /// <exception cref="Logic::ArgumentNullException">param is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(SearchCommand cmd, SearchWorkerData* param)
         {
            REQUIRED(param);

            // Set command + Start.
            param->Command = cmd;
            BackgroundWorker::Start(param);
         }

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::Threads;
