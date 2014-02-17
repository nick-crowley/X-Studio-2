#pragma once
#include "stdafx.h"
#include "WorkerFeedback.h"


namespace Logic
{
   namespace Threads
   {

      /// <summary>Background worker pattern</summary>
      class BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------
      protected:
         BackgroundWorker(ThreadProc pfn) : Proc(pfn), Thread(NULL)
         {}
      public:
         virtual ~BackgroundWorker()
         {}
      
         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------
	  
         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Determines whether thread is running.</summary>
         /// <returns></returns>
         bool  IsRunning() const
         {
            DWORD code(0);

            // Query thread state
            if (!GetExitCodeThread(Thread, &code))
               throw Win32Exception(HERE, L"Unable to query thread state");

            return code == STILL_ACTIVE;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Sets the 'abort' flag and closes the thread handle</summary>
         /// <returns>true if successful, false otherwise</returns>
         /// <exception cref="Logic::InvalidOperationException">Thread is not running</exception>
         bool  Stop()
         {
            if (Thread == nullptr)
               throw InvalidOperationException(HERE, L"Thread not running");

            // Request thread stop
            Data->Abort();

            // Close handle
            CloseHandle(Thread);
            Thread = nullptr;
            return true;
         }

      protected:
         /// <summary>Starts the thread.</summary>
         /// <param name="param">operation data.</param>
         /// <returns>true if successful, false otherwise</returns>
         /// <exception cref="Logic::ArgumentNullException">param is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         bool  Start(WorkerData* param)
         {
            REQUIRED(param);

            // Ensure not started
            if (Thread != nullptr)
               throw InvalidOperationException(HERE, L"Thread already running");

            // Launch thread
            Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Proc, (void*)(Data=param), NULL, NULL);
            return Thread != NULL;
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
