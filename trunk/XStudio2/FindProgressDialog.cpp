// FindProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FindProgressDialog.h"
#include "MainWnd.h"
#include "afxdialogex.h"


/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)

      // -------------------------------- CONSTRUCTION --------------------------------

      FindProgressDialog::FindProgressDialog(CWnd* pParent /*=NULL*/)
	      : CDialogEx(FindProgressDialog::IDD, pParent),
           Worker(nullptr)
      {

      }

      FindProgressDialog::~FindProgressDialog()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      IMPLEMENT_DYNAMIC(FindProgressDialog, CDialogEx)
      
      BEGIN_MESSAGE_MAP(FindProgressDialog, CDialogEx)
         ON_BN_CLICKED(IDCANCEL, &FindProgressDialog::OnCancel_Click)
         ON_WM_TIMER()
      END_MESSAGE_MAP()

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Displays modal dialog</summary>
      /// <param name="work">Worker to monitor.</param>
      /// <returns></returns>
      INT_PTR  FindProgressDialog::DoModal(SearchWorker* work)
      {
         Worker = work;
         return CDialogEx::DoModal();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      void FindProgressDialog::DoDataExchange(CDataExchange* pDX)
      {
         CDialogEx::DoDataExchange(pDX);
         DDX_Control(pDX, IDC_PROGRESS, ProgressBar);
      }

      /// <summary>One-time initialization</summary>
      /// <returns></returns>
      BOOL FindProgressDialog::OnInitDialog()
      {
         CDialogEx::OnInitDialog();

         // Set polling timer
         SetTimer(42, 250, nullptr);
         ProgressBar.SetMarquee(TRUE, 150);
         
         return TRUE;  // return TRUE unless you set the focus to a control
      }

      /// <summary>Finds the next match, if any</summary>
      void FindProgressDialog::OnCancel_Click()
      {
         // Stop worker
         Worker->Stop();

         // Close dialog
         KillTimer(42);
         EndDialog(IDOK);
      }

      
      void FindProgressDialog::OnTimer(UINT_PTR  id)
      {

         if (!Worker->IsRunning())
         {
            KillTimer(42);
            EndDialog(IDOK);
         }
      }
      

      // ------------------------------- PRIVATE METHODS ------------------------------



/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)


