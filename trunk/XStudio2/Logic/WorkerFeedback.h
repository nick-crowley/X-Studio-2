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
      enum class Operation : UINT { LoadGameData, LoadSaveDocument, FindReplace, Dummy };

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
         WorkerData(Operation op) : Operation(op), MainWnd(AfxGetMainWnd()), Aborted(false)
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
         
         /// <summary>Command thread to stop</summary>
         void  Abort()
         {
            Aborted = true;
         }

         /// <summary>Inform main window of progress</summary>
         void  SendFeedback(ProgressType t, UINT indent, const wstring& sz) const
         {
            if (Operation != Operation::Dummy)
               MainWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         /// <summary>Inform main window of progress and print message to console</summary>
         void  SendFeedback(Colour c, ProgressType t, UINT indent, const wstring& sz) const
         {
            if (Operation == Operation::Dummy)
               return;

            // Output to console 
            if (c == Colour::Cyan)
               Console << ENDL;
            Console << c << sz << ENDL;

            // Output to GUI
            MainWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         // -------------------- REPRESENTATION ---------------------

      private:
         Operation     Operation;
         CWnd*         MainWnd;
         volatile bool Aborted;
      };

      /// <summary>Thread function</summary>
      typedef DWORD (WINAPI *ThreadProc)(WorkerData*);

      // ----------------- EVENTS AND DELEGATES ------------------

      typedef Event<const WorkerProgress&>  FeedbackEvent;
      typedef FeedbackEvent::DelegatePtr    FeedbackHandler;

   }
}

using namespace Logic::Threads;
