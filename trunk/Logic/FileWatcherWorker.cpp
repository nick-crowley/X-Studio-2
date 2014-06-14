#include "stdafx.h"
#include "FileWatcherWorker.h"
#include "../Logic/FileStream.h"
#include "../Logic/LegacyProjectFileReader.h"
#include "../Logic/ProjectFileWriter.h"
#include <strsafe.h>

namespace Logic
{
   namespace Threads
   {
      /// <summary>Watches a file for changes by external programs</summary>
      class FileWatcher
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Short name for Win32 folder change notification</summary>
         typedef FILE_NOTIFY_INFORMATION NOTIFICATION;

         /// <summary>Provides a handle to a folder</summary>
         class FolderHandle
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Opens a shared handle to a folder</summary>
            /// <param name="p">Full Folder path.</param>
            /// <exception cref="Logic::DirectoryNotFoundException">Folder not found</exception>
            /// <exception cref="Logic::IOException">I/O error occurred</exception>
            FolderHandle(const Path& p) : Handle(nullptr)
            {
               // Attempt to open folder
               Handle = CreateFile(p.c_str(), GENERIC_READ, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, 
                                    nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

               // Verify folder exists
               if (Handle == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PATH_NOT_FOUND)
                  throw DirectoryNotFoundException(HERE, p);

               // Verify handle
               if (Handle == INVALID_HANDLE_VALUE)
                  throw IOException(HERE, L"Unable to open folder handle: " + SysErrorString());
            }

            /// <summary>Closes the handle</summary>
            virtual ~FolderHandle()
            {
               CloseHandle(Handle);
            }
		      
            NO_COPY(FolderHandle);	// Default copy semantics
            NO_MOVE(FolderHandle);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
			
            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Get file HANDLE.</summary>
            /// <returns>Handle.</returns>
            operator HANDLE() const
            {
               return Handle;
            }

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         protected:
            HANDLE  Handle;
         };

      public:
         /// <summary>Type of change to a file</summary>
         enum class ChangeType : DWORD 
         { 
            Added=FILE_ACTION_ADDED, 
            Removed=FILE_ACTION_REMOVED, 
            Modified=FILE_ACTION_MODIFIED, 
            RenamedOld=FILE_ACTION_RENAMED_OLD_NAME, 
            RenamedNew=FILE_ACTION_RENAMED_NEW_NAME
         };

         /// <summary>Represents a change to a file</summary>
         class FileChange
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create from a Win32 file change record</summary>
            /// <param name="folder">Folder containing the file.</param>
            /// <param name="f">File change record.</param>
            FileChange(const Path& folder, FILE_NOTIFY_INFORMATION* f)
            {
               REQUIRED(f);
               
               // Action
               Action = static_cast<ChangeType>(f->Action);

               // Generate path
               Path p;
               StringCchCopyN((wchar*)p, MAX_PATH, f->FileName, f->FileNameLength);
               FullPath = folder+p;
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            ChangeType Action;         // Type of change
            Path       FullPath;       // Full path of file
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create file watcher for a file</summary>
         /// <param name="f">Full path of file</param>
         FileWatcher(const Path& f) : FullPath(f)
         {}
         virtual ~FileWatcher()
         {}

         NO_COPY(FileWatcher);	// Uncopyable
         NO_MOVE(FileWatcher);	// Unmoveable

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      
         // ---------------------- ACCESSORS ------------------------			
      
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Suspends calling thread and waits for notification of changes to target file</summary>
         /// <returns>List of changes</returns>
         /// <exception cref="Logic::Win32Exception">Unable to listen for changes</exception>
         list<FileChange> Watch()
         {
            FolderHandle handle(FullPath.Folder);     // Folder handle
            ByteArrayPtr Buffer(new byte[4096]);      // FILE_NOTIFY_INFORMATION record buffer
            DWORD length = 0;

            // Prepare
            ZeroMemory(Buffer.get(), 4096);

            // Register+Wait for changes
            if (!ReadDirectoryChangesW(handle, Buffer.get(), 4096, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &length, nullptr, nullptr))
               throw Win32Exception(HERE, L"Unable to listen for file changes: " + SysErrorString());

            // Extract results
            return GetChanges(Buffer.get(), length);
         }

      protected:
         /// <summary>Gets a list of FileChanges extracted from a buffer of FILE_NOTIFY_INFORMATION records</summary>
         /// <param name="buf">Buffer containing FILE_NOTIFY_INFORMATION records.</param>
         /// <param name="len">Length of buffer in bytes</param>
         /// <returns>List of FileChanges</returns>
         list<FileChange> GetChanges(BYTE* buf, DWORD len)
         {
            list<FileChange> c;

            // Extract each record
            for (NOTIFICATION* n = (NOTIFICATION*)buf; n->NextEntryOffset < len; n = (NOTIFICATION*)((BYTE*)n + n->NextEntryOffset))
            {
               c.push_back(FileChange(FullPath.Folder, n));
            
               // Abort once complete
               if (!n->NextEntryOffset)
                  break;
            }

            return c;
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         Path  FullPath;
      };

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a new file watcher worker.</summary>
      /// <param name="owner">Window that will receive file change notifications</param>
      /// <exception cref="Logic::ArgumentNullException">Owner window is nullptr</exception>
      FileWatcherWorker::FileWatcherWorker(CWnd* owner)
         : BackgroundWorker((ThreadProc)ThreadMain),
           Data(owner)
      {
      }


      FileWatcherWorker::~FileWatcherWorker()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Registers and processes file-change notifications</summary>
      /// <param name="data">worker data.</param>
      /// <returns>TRUE if successful, FALSE if failed</returns>
      DWORD WINAPI FileWatcherWorker::ThreadMain(FileWatcherData* data)
      {
         try
         {
            HRESULT  hr;

            // Init COM
            if (FAILED(hr=CoInitialize(NULL)))
               throw ComException(HERE, hr);

            // Feedback
            Console << Cons::UserAction << "Watching for file changes: " << data->TargetPath << ENDL;

            // Await notification
            while (TRUE)
            {
               FileWatcher fw(data->TargetPath);

               // Watch for changes + iterate thru results
               for (auto& c : fw.Watch())
               {
                  // Modified: Notify owner window
                  if (c.Action == FileWatcher::ChangeType::Modified && c.FullPath == data->TargetPath)
                  {
                     Console << Cons::UserAction << "External changes detected in: " << data->TargetPath << ENDL;
                     data->NotifyOwner();
                  }
               }
            }
         
            // Cleanup
            CoUninitialize();
            return TRUE;
         }
         catch (ExceptionBase& e) 
         {
            Console.Log(HERE, e);

            // Cleanup
            CoUninitialize();
            return FALSE;
         }
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Starts watching for changes to a file</summary>
      /// <param name="p">Full file path</param>
      /// <exception cref="Logic::FileNotFoundException">File does not exist</exception>
      /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
      /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
      void  FileWatcherWorker::Start(const Path& p)
      {
         // Ensure file exists + thread not running
         if (!p.Exists())
            throw FileNotFoundException(HERE, p);
         if (IsRunning())
            throw InvalidOperationException(HERE, L"Thread already running");

         // Start thread
         Data.Reset(p);
         __super::Start(&Data);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

