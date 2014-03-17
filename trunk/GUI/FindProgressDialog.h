#pragma once
#include "afxwin.h"
//#include "../Logic/MatchData.h"
#include "../Logic/SearchWorker.h"

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
      /// <summary>Find text progress dialog</summary>
      class FindProgressDialog : public CDialogEx
      {
         // ------------------------ TYPES --------------------------
      public:
         enum { IDD = IDD_FINDPROGRESS };

      private:
         
         // --------------------- CONSTRUCTION ----------------------
      public:
	      FindProgressDialog(CWnd* pParent = NULL);   // standard constructor
	      virtual ~FindProgressDialog();

         // ------------------------ STATIC -------------------------

         DECLARE_DYNAMIC(FindProgressDialog)
         DECLARE_MESSAGE_MAP()

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      protected:

         // ----------------------- MUTATORS ------------------------
      public:
         INT_PTR DoModal(SearchWorker* work);

      protected:
	      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
         BOOL OnInitDialog() override;
         
         afx_msg void OnCancel_Click();
         afx_msg void OnTimer(UINT_PTR  id);
         
         // -------------------- REPRESENTATION ---------------------
      protected:
         const UINT  TIMER_ID = 42;

         CProgressCtrl  ProgressBar;
         SearchWorker*  Worker;
      };

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)
