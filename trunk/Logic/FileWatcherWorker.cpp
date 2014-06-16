#include "stdafx.h"
#include "FileWatcherWorker.h"
#include "ComThreadHelper.h"
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
                                    nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED|FILE_FLAG_BACKUP_SEMANTICS, nullptr);

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
      public:
         /// <summary>Empty File IO completion routine.</summary>
         /// <param name="dwErrorCode">status code.</param>
         /// <param name="dwBytes">bytes transferred.</param>
         /// <param name="pOverlapped">overlapped i/o.</param>
         static void CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwBytes, LPOVERLAPPED pOverlapped)
         {
         }

         // --------------------- PROPERTIES ------------------------
      
         // ---------------------- ACCESSORS ------------------------			
      
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Suspends calling thread and waits for notification of changes to target file</summary>
         /// <param name="abort">Handle to the event used to abort operation</param>
         /// <returns>List of changes</returns>
         /// <exception cref="Logic::Win32Exception">Unable to listen for changes</exception>
         list<FileChange> Watch(ManualEvent& abort)
         {
            FolderHandle handle(FullPath.Folder);     // Folder handle
            ByteArrayPtr Buffer(new byte[4096]);      // FILE_NOTIFY_INFORMATION record buffer
            OVERLAPPED   async;

            // Prepare
            ZeroMemory(Buffer.get(), 4096);
            ZeroMemory(&async, sizeof(OVERLAPPED));

            // Feedback
            Console << Cons::UserAction << "Watching for file changes: " << FullPath << "...";
            
            try
            {
               // Init async notification of changes
               if (!ReadDirectoryChangesW(handle, Buffer.get(), 4096, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, nullptr, &async, FileIOCompletionRoutine))
                  throw Win32Exception(HERE, L"Unable to create file-change listener");
               Console << Cons::Success << ENDL;

               // Wait until aborted/alerted 
               switch (WaitForSingleObjectEx(abort, INFINITE, TRUE))
               {
               // Aborted: Return nothing
               case WAIT_OBJECT_0:  
                  return list<FileChange>();    //DEBUG: Console << "File watch operation aborted" << ENDL;
               
               // Notification: Decode/return notifications
               case WAIT_IO_COMPLETION:
                  return GetChanges(Buffer.get(), async.InternalHigh);   //DEBUG: Console << "IO Completion routine queued" << ENDL;

               // Error:
               default:
                  throw Win32Exception(HERE, L"Unable to wait on file-change listener");
               }
            }
            catch (...)
            {
               Console << Cons::Failure << ENDL;
               throw;
            }
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
      FileWatcherWorker::FileWatcherWorker() : BackgroundWorker((ThreadProc)ThreadMain)
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
            REQUIRED(data);

            ComThreadHelper COM; // Init COM
            
            // Await notification
            Path path = data->GetFullPath();
            do 
            {
               FileWatcher fw(path);
               
               // Watch for changes
               for (auto& c : fw.Watch(data->AbortEvent))
               {
                  // Notify owner window if file was modified
                  if (c.Action == FileWatcher::ChangeType::Modified && c.FullPath == path)
                  {
                     Console << Cons::UserAction << "External changes detected in: " << path << ENDL;
                     data->NotifyOwner();
                  }
               }
               // Repeat until aborted
            } while (!data->AbortEvent.Signalled);

            // Success
            return TRUE;
         }
         catch (ExceptionBase& e) 
         {
            Console.Log(HERE, e);
            return FALSE;
         }
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Starts watching for changes to a file</summary>
      /// <param name="owner">Window that will receive file change notifications</param>
      /// <param name="file">Full path of file to watch</param>
      /// <exception cref="Logic::ArgumentNullException">Owner window is nullptr</exception>
      /// <exception cref="Logic::FileNotFoundException">File does not exist</exception>
      /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
      /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
      void  FileWatcherWorker::Start(CWnd* owner, const Path& file)
      {
         // Ensure file exists + thread not running
         if (!file.Exists())
            throw FileNotFoundException(HERE, file);
         if (IsRunning())
            throw InvalidOperationException(HERE, L"Thread already running");

         // Setup/Start thread
         Data.Reset(owner, file);
         __super::Start(&Data);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

