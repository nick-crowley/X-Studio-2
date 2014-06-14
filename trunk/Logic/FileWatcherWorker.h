#pragma once
#include "BackgroundWorker.h"
#include <strsafe.h>

namespace Logic
{
   namespace Threads
   {
      /// <summary>Notifies a window of a file change</summary>
      #define WM_FILE_CHANGE   (WM_USER+2)

      /// <summary>Registers and processes file-change notifications</summary>
      class LogicExport FileWatcherWorker : public BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Data for file-change notification worker thread</summary>
         class LogicExport FileWatcherData : public WorkerData
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create data for a file watcher worker</summary>
            /// <param name="owner">Window that will receive file change notifications</param>
            /// <exception cref="Logic::ArgumentNullException">Owner window is nullptr</exception>
            FileWatcherData(CWnd* owner) : WorkerData(Operation::FileWatcher), Owner(owner)
            {
               REQUIRED(owner);
            }

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Notifies owner the file has changed</summary>
            void NotifyOwner()
            {
               Owner->PostMessageW(WM_FILE_CHANGE);
            }
            
            /// <summary>Resets to initial state.</summary>
            /// <param name="f">legacy project path to upgrade</param>
            void Reset(const Path& p)
            {
               // Reset path + aborted (+ parentWnd)
               TargetPath = p;
               __super::Reset();
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            Path  TargetPath;    // Full path of file to watch

         protected:
            CWnd*  Owner;        // Window to notify upon changes
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         FileWatcherWorker(CWnd* owner);
         virtual ~FileWatcherWorker();

         NO_COPY(FileWatcherWorker);	// No copy semantics
         NO_MOVE(FileWatcherWorker);	// No move semantics

         // ------------------------ STATIC -------------------------
      protected:
         static DWORD WINAPI ThreadMain(FileWatcherData* data);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Starts watching for changes to a file</summary>
         /// <param name="p">Full file path</param>
         /// <exception cref="Logic::FileNotFoundException">File does not exist</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(const Path& p);

         // -------------------- REPRESENTATION ---------------------
      protected:
         FileWatcherData  Data;
      };

   }
}

using namespace Logic::Threads;
