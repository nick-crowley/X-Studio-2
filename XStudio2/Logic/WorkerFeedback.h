#pragma once
#include "stdafx.h"
#include "Event.h"


namespace Logic
{
   namespace Threads
   {

      /// <summary>Feedback message</summary>
      #define WM_FEEDBACK     (WM_USER+1)

      // ------------------------ CLASSES ------------------------

      /// <summary></summary>
      enum class ProgressType : UINT { Operation, Info, Warning, Error, Succcess, Failure };

      /// <summary></summary>
      enum class Operation : UINT { LoadGameData, LoadSaveDocument, FindAndReplace, Dummy };

      /// <summary></summary>
      class WorkerProgress
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         WorkerProgress(Operation op, ProgressType t, UINT indent, const wstring& sz) : Operation(op), Type(t), Text(sz), Indent(indent)
         {}

         // -------------------- REPRESENTATION ---------------------

         const Operation     Operation;
         const ProgressType  Type;
         const wstring       Text;
         const UINT          Indent;
      };

      /// <summary></summary>
      class WorkerData
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates worker data for an operation</summary>
         /// <param name="op">operation.</param>
         WorkerData(Operation op) : ParentWnd(AfxGetMainWnd()), Operation(op), Aborted(false)
         {}
         virtual ~WorkerData()
         {}

         // ------------------------ STATIC -------------------------
      public:
         const static WorkerData   NoFeedback;

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Poll whether thread has been commanded to stop</summary>
         bool  IsAborted() const
         {
            return Aborted;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Command thread to stop</summary>
         void  Abort()
         {
            Aborted = true;
         }

         /// <summary>Inform main window of progress</summary>
         void  SendFeedback(ProgressType t, UINT indent, const wstring& sz) const
         {
            // Dummy: NOP
            if (Operation == Operation::Dummy || !ParentWnd)
               return;

            // Output to GUI
            ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         /// <summary>Inform main window of progress and print message to console</summary>
         void  SendFeedback(Cons c, ProgressType t, UINT indent, const wstring& sz) const
         {
            // Dummy: NOP
            if (Operation == Operation::Dummy || !ParentWnd)
               return;

            // Output to console 
            Console << c << sz << ENDL;

            // Output to GUI
            ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         /// <summary>Sets the feedback window manually.</summary>
         /// <param name="wnd">The window.</param>
         /// <exception cref="Logic::ArgumentNullException">window is null</exception>
         void  SetFeedbackWnd(CWnd* wnd)
         {
            REQUIRED(wnd);
            ParentWnd = wnd;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const Operation  Operation;

      private:
         volatile bool    Aborted;
         CWnd*            ParentWnd;
      };

      /// <summary>Thread function</summary>
      typedef DWORD (WINAPI *ThreadProc)(WorkerData*);

      // ----------------- EVENTS AND DELEGATES ------------------

      typedef Event<const WorkerProgress&>  FeedbackEvent;
      typedef FeedbackEvent::DelegatePtr    FeedbackHandler;

   }
}

using namespace Logic::Threads;
