#pragma once
#include "Logic/Macros.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   /// <summary>User interface</summary>
   #define WM_FEEDBACK     (WM_USER+1)

   // ------------------------ CLASSES ------------------------

   /// <summary>Thread function</summary>
   /// <remarks>DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter)</remarks>
   typedef LPTHREAD_START_ROUTINE  ThreadProc;    

   /// <summary></summary>
   class WorkerProgress
   {
   public:
      WorkerProgress(const wstring& sz) : Text(sz)
      {}

      wstring Text;
   };

   /// <summary></summary>
   class WorkerData
   {
   public:
      WorkerData() : MainWnd(AfxGetMainWnd()), Abort(false)
      {}
      virtual ~WorkerData()
      {}

      void  SendFeedback(const wstring& sz)
      {
         MainWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(sz));
      }

      CWnd*  MainWnd;
      bool   Abort;
   };

   // ----------------- EVENTS AND DELEGATES ------------------

   typedef Event<WorkerProgress*>      FeedbackEvent;
   typedef FeedbackEvent::DelegatePtr  FeedbackHandler;

   // ------------------------ CLASSES ------------------------

   /// <summary></summary>
   class WorkerThread
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      WorkerThread(ThreadProc pfn) : ThreadProc(pfn), Thread(NULL)
      {}
      virtual ~WorkerThread()
      {}
      
      // ------------------------ STATIC -------------------------
      
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
      bool  Start(WorkerData* param)
      {
         REQUIRED(param);

         // Ensure not started
         if (Thread != nullptr)
            throw InvalidOperationException(HERE, L"Thread already running");

         // Launch thread
         Thread = CreateThread(NULL, 0, ThreadProc, (void*)(Data=param), NULL, NULL);
         return Thread != NULL;
      }


      bool  Stop()
      {
         if (Thread == nullptr)
            throw InvalidOperationException(HERE, L"Thread not running");

         // Request thread stop
         Data->Abort = true;

         // Close handle
         CloseHandle(Thread);
         Thread = nullptr;
         return true;
      }

      // -------------------- REPRESENTATION ---------------------
      
   private:
      ThreadProc   ThreadProc;
      WorkerData*  Data;
      HANDLE       Thread;
   };
   


NAMESPACE_END(GUI)
