#pragma once
#include "Event.h"


namespace Logic
{
   namespace Threads
   {
      class WorkerProgress;
      class WorkerData;

      /// <summary>Feedback message</summary>
      #define WM_FEEDBACK     (WM_USER+1)

      // ------------------------- TYPES -------------------------

      /// <summary>Thread function</summary>
      typedef DWORD (WINAPI *ThreadProc)(WorkerData*);

      // ------------------------- ENUMS -------------------------

      /// <summary>Defines the icon used by output window feedback</summary>
      enum class ProgressType : UINT { Operation, Info, Warning, Error, Succcess, Failure };

      /// <summary>Defines the output window used by various operations</summary>
      enum class Operation : UINT { LoadGameData, LoadSaveDocument, FindAndReplace1, FindAndReplace2, ImportProject, NoFeedback };

      // ----------------- EVENTS AND DELEGATES ------------------

      typedef Event<const WorkerProgress&>  FeedbackEvent;
      typedef FeedbackEvent::DelegatePtr    FeedbackHandler;

      // ------------------------ CLASSES ------------------------

      /// <summary>Base class LogicExport for output window feedback items</summary>
      class LogicExport WorkerProgress
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



      /// <summary>Base class LogicExport for background worker thread data</summary>
      class LogicExport WorkerData
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates worker data for an operation</summary>
         /// <param name="op">operation.</param>
         WorkerData(Operation op) : ParentWnd(AfxGetApp()->m_pMainWnd), Operation(op), Aborted(false)
         {
         }

      private:
         /// <summary>Creates 'No Feedback' sentinel data</summary>
         WorkerData() : ParentWnd(nullptr), Operation(Operation::NoFeedback), Aborted(false)
         {}

      public:
         virtual ~WorkerData()
         {}

         // ------------------------ STATIC -------------------------
      public:
         const static WorkerData   NoFeedback;

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      public:
         CWnd* GetParent() const
         {
            return ParentWnd;
         }

         /// <summary>Poll whether thread has been commanded to stop</summary>
         bool  IsAborted() const
         {
            return Aborted;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Command thread to stop</summary>
         virtual void  Abort()
         {
            Aborted = true;
         }

         /// <summary>Resets to initial state.</summary>
         virtual void  Reset()
         {
            ParentWnd = AfxGetApp()->m_pMainWnd;
            Aborted = false;
         }

         /// <summary>Inform main window of progress</summary>
         void  SendFeedback(ProgressType t, UINT indent, const wstring& sz) const
         {
            // Dummy: NOP
            if (Operation == Operation::NoFeedback || !ParentWnd)
               return;

            // Output to GUI
            ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         /// <summary>Inform main window of progress and print message to console</summary>
         void  SendFeedback(Cons c, ProgressType t, UINT indent, const wstring& sz) const
         {
            // Dummy: NOP
            if (Operation == Operation::NoFeedback || !ParentWnd)
               return;

            // Output to console 
            Console << c << sz << ENDL;

            // Output to GUI
            ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const Operation  Operation;

      protected:
         CWnd*            ParentWnd;

      private:
         volatile bool    Aborted;
      };

      

   }
}

using namespace Logic::Threads;
