// FindProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FindProgressDialog.h"
#include "MainWnd.h"
#include "afxdialogex.h"


/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create progress dialog for a background worker</summary>
      /// <param name="caption">caption.</param>
      /// <param name="desc">description text.</param>
      /// <param name="allowCancel">Allow cancellation</param>
      /// <param name="parent">parent window.</param>
      FindProgressDialog::FindProgressDialog(const wstring& caption, const wstring& desc, bool allowCancel, CWnd* parent/* = nullptr*/)
	      : CDialogEx(FindProgressDialog::IDD, parent),
           Caption(caption),
           Description(desc),
           AllowCancel(allowCancel),
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
      /// <param name="w">Worker to monitor.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentNullException">Worker is null</exception>
      INT_PTR  FindProgressDialog::DoModal(BackgroundWorker* w)
      {
         REQUIRED(w);

         Worker = w;
         return CDialogEx::DoModal();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Does the data exchange.</summary>
      /// <param name="pDX">The p dx.</param>
      void FindProgressDialog::DoDataExchange(CDataExchange* pDX)
      {
         CDialogEx::DoDataExchange(pDX);
         DDX_Control(pDX, IDCANCEL, Cancel);
         DDX_Control(pDX, IDC_PROGRESS_BAR, ProgressBar);
         DDX_Text(pDX, IDC_PROGRESS_STATIC, Description);
      }

      /// <summary>Setup polling timer + marquee bar</summary>
      /// <returns></returns>
      BOOL FindProgressDialog::OnInitDialog()
      {
         CDialogEx::OnInitDialog();

         // Set polling timer
         SetTimer(TIMER_ID, 250, nullptr);
         ProgressBar.SetMarquee(TRUE, 150);

         // Enable/Disable 'Cancel'
         Cancel.EnableWindow(AllowCancel ? TRUE : FALSE);

         // Centre window
         CenterWindow(nullptr);
         return TRUE;  // return TRUE unless you set the focus to a control
      }

      /// <summary>Aborts the worker thread</summary>
      void FindProgressDialog::OnCancel_Click()
      {
         if (!AllowCancel)
            return;

         // Feedback
         Console << Cons::UserAction << "Aborting operation..." << ENDL;

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


