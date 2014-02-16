// FindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FindDialog.h"
#include "afxdialogex.h"


/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Window)

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
         ON_BN_CLICKED(IDC_FIND, &FindDialog::OnFind_Click)
         ON_BN_CLICKED(IDC_REPLACE, &FindDialog::OnReplace_Click)
         ON_BN_CLICKED(IDC_FIND_ALL, &FindDialog::OnFindAll_Click)
         ON_BN_CLICKED(IDC_REPLACE_ALL, &FindDialog::OnReplaceAll_Click)
         ON_BN_CLICKED(IDC_OPTIONS, &FindDialog::OnOptions_Click)
         ON_CBN_EDITCHANGE(IDC_FIND_COMBO, &FindDialog::OnFind_TextChanged)
         ON_CBN_EDITCHANGE(IDC_REPLACE_COMBO, &FindDialog::OnReplace_TextChanged)
         ON_CBN_EDITCHANGE(IDC_TARGET_COMBO, &FindDialog::OnTarget_TextChanged)
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
      
      BOOL FindDialog::OnInitDialog()
      {
         CDialogEx::OnInitDialog();

         // Populate targets
         TargetCombo.AddString(L"Selection");
         TargetCombo.AddString(L"Current Document");
         TargetCombo.AddString(L"All Open Documents");
         TargetCombo.AddString(L"All Project Files");
         TargetCombo.AddString(L"Scripts Folder");

         return TRUE;  // return TRUE unless you set the focus to a control
      }

      void FindDialog::OnFind_Click()
      {
         // TODO: Add your control notification handler code here
      }

      
      void FindDialog::OnFind_TextChanged()
      {
         // TODO: Add your control notification handler code here
      }
      
      void FindDialog::OnFindAll_Click()
      {
         // TODO: Add your control notification handler code here
      }
      
      void FindDialog::OnOptions_Click()
      {
         // TODO: Add your control notification handler code here
      }

      void FindDialog::OnReplace_Click()
      {
         // TODO: Add your control notification handler code here
      }

      void FindDialog::OnReplace_TextChanged()
      {
         // TODO: Add your control notification handler code here
      }
      

      void FindDialog::OnReplaceAll_Click()
      {
         // TODO: Add your control notification handler code here
      }
      
      void FindDialog::OnTarget_TextChanged()
      {
         // TODO: Add your control notification handler code here
      }
      

      // ------------------------------- PRIVATE METHODS ------------------------------



/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Window)


