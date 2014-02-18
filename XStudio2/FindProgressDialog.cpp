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
         SetTimer(TIMER_ID, 250, nullptr);
         ProgressBar.SetMarquee(TRUE, 150);

         // Centre
         CenterWindow(nullptr);
         
         return TRUE;  // return TRUE unless you set the focus to a control
      }

      /// <summary>Aborts the worker thread</summary>
      void FindProgressDialog::OnCancel_Click()
      {
         // Feedback
         Console << Cons::UserAction << "Aborting search..." << ENDL;

         // Abort worker
         Worker->Stop();
      }

      
      /// <summary>Closes when the worker thread ends</summary>
      /// <param name="id">The identifier.</param>
      void FindProgressDialog::OnTimer(UINT_PTR  id)
      {
         // Close dialog when thread ends
         if (!Worker->IsRunning())
         {
            KillTimer(TIMER_ID);
            EndDialog(IDOK);
         }
      }
      

      // ------------------------------- PRIVATE METHODS ------------------------------



/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)


