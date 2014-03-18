#pragma once
#include "afxwin.h"
//#include "../Logic/MatchData.h"
//#include "../Logic/SearchWorker.h"
#include "../Logic/BackgroundWorker.h"

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
      /// <summary>Find text progress dialog</summary>
      class ProgressDialog : public CDialogEx
      {
         // ------------------------ TYPES --------------------------
      public:
         enum { IDD = IDD_PROGRESS };

      protected:
         const UINT  TIMER_ID = 42;
         
         // --------------------- CONSTRUCTION ----------------------
      public:
	      ProgressDialog(const wstring& caption, const wstring& desc, bool allowCancel, CWnd* parent = nullptr);
	      virtual ~ProgressDialog();

         // ------------------------ STATIC -------------------------

         DECLARE_DYNAMIC(ProgressDialog)
         DECLARE_MESSAGE_MAP()

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      protected:

         // ----------------------- MUTATORS ------------------------
      public:
         INT_PTR DoModal(BackgroundWorker* w);

      protected:
	      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
         BOOL OnInitDialog() override;
         
         afx_msg void OnCancel_Click();
         afx_msg void OnTimer(UINT_PTR  id);
         
         // -------------------- REPRESENTATION ---------------------
      protected:
         CProgressCtrl  ProgressBar;
         CButton        Cancel;

         BackgroundWorker*  Worker;

         wstring        Caption,
                        Description;
         bool           AllowCancel;
      };

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)
