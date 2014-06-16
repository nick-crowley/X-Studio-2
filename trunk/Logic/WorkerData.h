#pragma once
#include "Event.h"
#include "SyncEvent.h"

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
      enum class ProgressType : UINT 
      { 
         Operation, 
         Info, 
         Warning, 
         Error, 
         Succcess, 
         Failure 
      };

      /// <summary>Defines the output window used by various operations</summary>
      enum class Operation : UINT 
      { 
         LoadGameData, 
         LoadSaveDocument, 
         FileWatcher,
         FindAndReplace1, 
         FindAndReplace2, 
         ImportProject, 
         NoFeedback 
      };

      /// <summary>Get operation name.</summary>
      /// <param name="op">operation.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unrecognised operation</exception>
      wstring GetString(Operation op);

      // ----------------- EVENTS AND DELEGATES ------------------

      typedef Event<const WorkerProgress&>  FeedbackEvent;
      typedef FeedbackEvent::DelegatePtr    FeedbackHandler;

      // ------------------------ CLASSES ------------------------

      /// <summary>Base class for output window feedback items</summary>
      class LogicExport WorkerProgress
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create worker feedback.</summary>
         /// <param name="op">Operation type</param>
         /// <param name="t">Type of progress to report</param>
         /// <param name="indent">Amount of identation.</param>
         /// <param name="sz">Message</param>
         WorkerProgress(Operation op, ProgressType t, UINT indent, const wstring& sz) : Operation(op), Type(t), Text(sz), Indent(indent)
         {}

         // -------------------- REPRESENTATION ---------------------

         const Operation     Operation;      // Operation type
         const ProgressType  Type;           // Progress report type
         const wstring       Text;           // Report text
         const UINT          Indent;         // Amount of Indentation 
      };



      /// <summary>Base class for background worker thread data</summary>
      class LogicExport WorkerData
      {
         // --------------------- CONSTRUCTION ----------------------
      private:
         /// <summary>Creates 'No Feedback' sentinel data</summary>
         WorkerData();

      public:
         /// <summary>Creates worker data for an operation</summary>
         /// <param name="op">operation.</param>
         WorkerData(Operation op);

         virtual ~WorkerData();

         // ------------------------ STATIC -------------------------
      public:
         const static WorkerData   NoFeedback;

         // --------------------- PROPERTIES ------------------------
		public:
         PROPERTY_GET(ManualEvent,AbortEvent,GetAbortEvent);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets the event used to signal an abort.</summary>
         /// <returns></returns>
         /// <exception cref="Logic::Win32Exception">Unable to get event</exception>
         ManualEvent GetAbortEvent() const
         {
            return Aborted;
         }

         /// <summary>Gets the parent window that received notifications.</summary>
         /// <returns></returns>
         CWnd* GetParent() const
         {
            return ParentWnd;
         }

         /// <summary>Query whether thread has been commanded to stop</summary>
         bool  IsAborted() const
         {
            return Aborted.Signalled;
         }

         /// <summary>Inform main window of progress</summary>
         void  SendFeedback(ProgressType t, UINT indent, const wstring& sz) const;

         /// <summary>Inform main window of progress and print message to console</summary>
         void  SendFeedback(Cons c, ProgressType t, UINT indent, const wstring& sz) const;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Command worker to stop</summary>
         /// <exception cref="Logic::Win32Exception">Unable to abort worker</exception>
         virtual void  Abort();

         /// <summary>Resets to initial state.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to abort worker</exception>
         virtual void  Reset();

         // -------------------- REPRESENTATION ---------------------
      public:
         const Operation  Operation;    // Operation type

      protected:
         ManualEvent      Aborted;      // Used to signal operation should be aborted
         CWnd*            ParentWnd;    // Window that received feedback notifications
      };

      

   }
}

using namespace Logic::Threads;
