#pragma once
#include "WorkerFeedback.h"


namespace Logic
{
   namespace Threads
   {

      /// <summary>Background worker pattern</summary>
      class LogicExport BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------
      protected:
         BackgroundWorker(ThreadProc pfn) : Proc(pfn), Thread(NULL)
         {}
      public:
         virtual ~BackgroundWorker()
         {
            Close();
         }
      
         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------
	  
         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Determines whether thread is running.</summary>
         /// <returns></returns>
         bool  IsRunning() const
         {
            DWORD code(0);

            // Check if closed
            if (Thread == nullptr)
               return false;

            // Query thread state
            if (!GetExitCodeThread(Thread, &code))
               throw Win32Exception(HERE, L"Unable to query thread state");

            // return true if active
            return code == STILL_ACTIVE;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Closes the thread handle.</summary>
         /// <param name="deleteData">Delete thread data.</param>
         void  Close(bool deleteData = false)
         {
            // Close thread
            CloseHandle(Thread);
            Thread = nullptr;

            // Delete data
            if (deleteData && Data)
            {
               delete Data;
               Data = nullptr;
            }
         }

         /// <summary>Sets the 'abort' flag and closes the thread handle</summary>
         /// <exception cref="Logic::InvalidOperationException">Thread is not running</exception>
         void  Stop()
         {
            // Ensure running
            if (!IsRunning())
               throw InvalidOperationException(HERE, L"Thread not running");

            // Request thread stop
            Data->Abort();
         }

      protected:
         /// <summary>Starts the thread.</summary>
         /// <param name="param">operation data.</param>
         /// <exception cref="Logic::ArgumentNullException">param is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(WorkerData* param)
         {
            REQUIRED(param);

            // Ensure not started
            if (Thread != nullptr)
               throw InvalidOperationException(HERE, L"Thread already running");

            // Launch thread
            if (Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Proc, (void*)(Data=param), NULL, NULL))
               return;

            // Failed:
            throw Win32Exception(HERE, L"Unable to start thread");
         }

         // -------------------- REPRESENTATION ---------------------
      
      private:
         ThreadProc   Proc;
         WorkerData*  Data;
         HANDLE       Thread;
      };
   

   }
}

using namespace Logic::Threads;
