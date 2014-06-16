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
            FileWatcherData() : WorkerData(Operation::FileWatcher)
            {}

            // --------------------- PROPERTIES ------------------------
         
            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Gets the full path of the target file.</summary>
            /// <returns></returns>
            Path GetFullPath() const
            {
               return FullPath;
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Notifies owner the file has changed</summary>
            void NotifyOwner()
            {
               Owner->PostMessageW(WM_FILE_CHANGE);
            }
            
            /// <summary>Resets to initial state.</summary>
            /// <param name="owner">Window that will receive file change notifications</param>
            /// <param name="file">Full path of file to watch</param>
            /// <exception cref="Logic::ArgumentNullException">Owner window is nullptr</exception>
            void Reset(CWnd* owner, const Path& file)
            {
               REQUIRED(owner);

               // Reset
               Owner = owner;
               FullPath = file;

               // Reset base
               __super::Reset();
            }
            
            // -------------------- REPRESENTATION ---------------------
         protected:
            CWnd* Owner;      // Window to notify upon changes
            Path  FullPath;   // Full path of file to watch
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         FileWatcherWorker();
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
         /// <param name="owner">Window that will receive file change notifications</param>
         /// <param name="file">Full path of file to watch</param>
         /// <exception cref="Logic::ArgumentNullException">Owner window is nullptr</exception>
         /// <exception cref="Logic::FileNotFoundException">File does not exist</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(CWnd* owner, const Path& file);

         // -------------------- REPRESENTATION ---------------------
      protected:
         FileWatcherData  Data;
      };

   }
}

using namespace Logic::Threads;
