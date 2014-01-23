#pragma once
#include "stdafx.h"
#include "WorkerFeedback.h"


namespace Logic
{
   namespace Threads
   {

      /// <summary></summary>
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
      
         // ----------------------- MUTATORS ------------------------
      public:
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
