// FindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FindDialog.h"
#include "afxdialogex.h"


/// <summary>User interface windows</summary>
//NAMESPACE_BEGIN2(GUI,Window)

      // -------------------------------- CONSTRUCTION --------------------------------

      FindDialog::FindDialog(CWnd* pParent /*=NULL*/)
	      : CDialogEx(FindDialog::IDD, pParent)
         , UseRegEx(FALSE)
         , MatchCase(FALSE)
         , MatchWholeWord(FALSE)
      {

      }

      FindDialog::~FindDialog()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      IMPLEMENT_DYNAMIC(FindDialog, CDialogEx)
      
      BEGIN_MESSAGE_MAP(FindDialog, CDialogEx)
         ON_BN_CLICKED(IDC_FIND, &FindDialog::OnBnClickedFind)
         ON_BN_CLICKED(IDC_REPLACE, &FindDialog::OnBnClickedReplace)
         ON_BN_CLICKED(IDC_FIND_ALL, &FindDialog::OnBnClickedFindAll)
         ON_BN_CLICKED(IDC_REPLACE_ALL, &FindDialog::OnBnClickedReplaceAll)
         ON_BN_CLICKED(IDC_OPTIONS, &FindDialog::OnBnClickedOptions)
      END_MESSAGE_MAP()

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      void FindDialog::DoDataExchange(CDataExchange* pDX)
      {
         CDialogEx::DoDataExchange(pDX);
         DDX_Check(pDX, IDC_REGEX_CHECK, UseRegEx);
         DDX_Check(pDX, IDC_CASE_CHECK, MatchCase);
         DDX_Check(pDX, IDC_WORD_CHECK, MatchWholeWord);
         DDX_Control(pDX, IDC_FIND, FindButton);
         DDX_Control(pDX, IDC_REPLACE, ReplaceButton);
         DDX_Control(pDX, IDC_FIND_ALL, FindAllButton);
         DDX_Control(pDX, IDC_REPLACE_ALL, ReplaceAllButton);
         DDX_Control(pDX, IDC_FIND_COMBO, FindCombo);
         DDX_Control(pDX, IDC_REPLACE_COMBO, ReplaceCombo);
         DDX_Control(pDX, IDC_TARGET_COMBO, TargetCombo);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------



/// <summary>User interface windows</summary>
//NAMESPACE_END2(GUI,Window)


      void FindDialog::OnBnClickedFind()
      {
         // TODO: Add your control notification handler code here
      }


      void FindDialog::OnBnClickedReplace()
      {
         // TODO: Add your control notification handler code here
      }


      void FindDialog::OnBnClickedFindAll()
      {
         // TODO: Add your control notification handler code here
      }


      void FindDialog::OnBnClickedReplaceAll()
      {
         // TODO: Add your control notification handler code here
      }


      void FindDialog::OnBnClickedOptions()
      {
         // TODO: Add your control notification handler code here
      }
