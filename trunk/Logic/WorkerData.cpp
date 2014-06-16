#include "stdafx.h"
#include "WorkerData.h"

namespace Logic
{
   namespace Threads
   {
      // -------------------------------- STATIC DATA  --------------------------------

      /// <summary>No operation feedback sentinel</summary>
      const WorkerData   WorkerData::NoFeedback;

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates 'No Feedback' sentinel data</summary>
      WorkerData::WorkerData() : ParentWnd(nullptr), Operation(Operation::NoFeedback), Aborted(false)
      {
      }

      /// <summary>Creates worker data for an operation</summary>
      /// <param name="op">operation.</param>
      WorkerData::WorkerData(Logic::Threads::Operation op) : ParentWnd(AfxGetApp()->m_pMainWnd), Operation(op), Aborted(false)
      {
      }

      WorkerData::~WorkerData()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get operation name.</summary>
      /// <param name="op">operation.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unrecognised operation</exception>
      wstring GetString(Operation op)
      {
         switch (op)
         {
         case Operation::LoadGameData:      return L"LoadGameData";
         case Operation::LoadSaveDocument:  return L"LoadSaveDocument";
         case Operation::FileWatcher:       return L"FileWatcher";
         case Operation::FindAndReplace1:   return L"FindAndReplace2";
         case Operation::FindAndReplace2:   return L"FindAndReplace2";
         case Operation::ImportProject:     return L"ImportProject";
         case Operation::NoFeedback:        return L"NoFeedback";
         }
         throw ArgumentException(HERE, L"op", VString(L"Unrecognised operation: %d", op));
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Command worker to stop</summary>
      /// <exception cref="Logic::Win32Exception">Unable to abort worker</exception>
      void WorkerData::Abort()
      {
         Aborted.Signal(); // Signal abort event
      }

      /// <summary>Resets to initial state.</summary>
      /// <exception cref="Logic::Win32Exception">Unable to abort worker</exception>
      void WorkerData::Reset()
      {
         // Reset parent window + abort event
         ParentWnd = AfxGetApp()->m_pMainWnd;
         Aborted.Reset();
      }

      /// <summary>Inform main window of progress</summary>
      void WorkerData::SendFeedback(ProgressType t, UINT indent, const wstring& sz) const
      {
         // Dummy: NOP
         if (Operation == Operation::NoFeedback || !ParentWnd)
            return;

         // Output to GUI
         ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
      }

      /// <summary>Inform main window of progress and print message to console</summary>
      void WorkerData::SendFeedback(Cons c, ProgressType t, UINT indent, const wstring& sz) const
      {
         // Dummy: NOP
         if (Operation == Operation::NoFeedback || !ParentWnd)
            return;

         // Output to console 
         Console << c << sz << ENDL;

         // Output to GUI
         ParentWnd->PostMessageW(WM_FEEDBACK, NULL, (LPARAM)new WorkerProgress(Operation, t, indent, sz));
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}





      