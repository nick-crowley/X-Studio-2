// FindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FindDialog.h"
#include "MainWnd.h"
#include "afxdialogex.h"
#include "FindProgressDialog.h"

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates the dialog</summary>
      /// <param name="parent">Parent window</param>
      FindDialog::FindDialog(CWnd* pParent /*=NULL*/)
	      : CDialogEx(FindDialog::IDD, pParent)
         , UseRegEx(FALSE)
         , MatchCase(FALSE)
         , MatchWholeWord(FALSE)
         , Expanded(true)
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
         ON_CONTROL_RANGE(CBN_EDITCHANGE,IDC_FIND_COMBO,IDC_TARGET_COMBO,&FindDialog::OnOptions_Changed)
         ON_CONTROL_RANGE(BN_CLICKED,IDC_CASE_CHECK,IDC_REGEX_CHECK,&FindDialog::OnOptions_Changed)
         ON_WM_SHOWWINDOW()
         ON_WM_CLOSE()
         ON_WM_DESTROY()
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
         DDX_Control(pDX, IDC_FIND_ALL, FindAllButton);
         DDX_Control(pDX, IDC_FIND_COMBO, FindCombo);
         DDX_Control(pDX, IDC_OPTIONS, OptionsButton);
         DDX_Control(pDX, IDC_REPLACE, ReplaceButton);
         DDX_Control(pDX, IDC_REPLACE_ALL, ReplaceAllButton);
         DDX_Control(pDX, IDC_REPLACE_COMBO, ReplaceCombo);
         DDX_Control(pDX, IDC_TARGET_COMBO, TargetCombo);
      }

      /// <summary>Shrinks or Expands the dialog.</summary>
      /// <param name="expand">True to expand, false to shrink</param>
      void FindDialog::Expand(bool expand)
      {
         vector<int> Buttons  = { IDC_FIND, IDC_REPLACE, IDC_OPTIONS, IDC_FIND_ALL, IDC_REPLACE_ALL };
         vector<int> Controls = { IDC_REGEX_CHECK, IDC_CASE_CHECK, IDC_WORD_CHECK, IDC_RESULTS1_RADIO, IDC_RESULTS2_RADIO, IDC_FIND_GROUPBOX };

         // Show/Hide controls
         for (int id : Controls)
            GetDlgItem(id)->ShowWindow(expand ? SW_SHOW : SW_HIDE);

         // Calculate amount to resize
         CRect resizeRect;
         GetDlgItem(IDC_FIND_RESIZE)->GetWindowRect(resizeRect);
         int resize = (expand ? 1 : -1) * resizeRect.Height();

         // Resize window
         CRect wnd;
         GetWindowRect(wnd);
         SetWindowPos(nullptr, 0, 0, wnd.Width(), wnd.Height()+resize, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
         
         // Move buttons
         for (int id : Buttons)
         {
            CRect rc;
            GetDlgItem(id)->GetWindowRect(rc);
            ScreenToClient(rc);
            rc.OffsetRect(0, resize);
            GetDlgItem(id)->SetWindowPos(nullptr, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
         }

         // Set options text
         OptionsButton.SetWindowTextW(expand ? L"Hide Options" : L"Show Options");
         
         // Update/Save state
         PrefsLib.ShowFindOptions = (Expanded = expand);
      }

      /// <summary>Gets the feedback pane.</summary>
      /// <returns></returns>
      Operation  FindDialog::GetOutputPane() const
      {
         return IsDlgButtonChecked(IDC_RESULTS1_RADIO) ? Operation::FindAndReplace1 : Operation::FindAndReplace2;
      }

      /// <summary>Gets the search target.</summary>
      /// <returns></returns>
      SearchTarget  FindDialog::GetSearchTarget() const
      {
         return (SearchTarget)TargetCombo.GetCurSel();
      }

      /// <summary>Gets the search term.</summary>
      /// <returns></returns>
      wstring  FindDialog::GetSearchTerm() const
      {
         CString str;
         FindCombo.GetWindowText(str);
         return (const wchar*)str;
      }
      
      /// <summary>Gets the replacement term.</summary>
      /// <returns></returns>
      wstring  FindDialog::GetReplaceTerm() const
      {
         CString str;
         ReplaceCombo.GetWindowText(str);
         return (const wchar*)str;
      }
      

      void FindDialog::OnClose()
      {
         

         // Close
         CDialogEx::OnClose();
      }
      

      void FindDialog::OnDestroy()
      {
         try
         {
            // Store search terms
            PrefsLib.SearchTerms = FindCombo.GetAllStrings();
            PrefsLib.ReplaceTerms = ReplaceCombo.GetAllStrings();
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e);
            Console.Log(HERE, e);
         }

         CDialogEx::OnDestroy();

         // TODO: Add your message handler code here
      }

      /// <summary>One-time initialization</summary>
      /// <returns></returns>
      BOOL FindDialog::OnInitDialog()
      {
         CDialogEx::OnInitDialog();

         // Populate targets
         TargetCombo.AddString(L"Selection");
         TargetCombo.AddString(L"Current Document");
         TargetCombo.AddString(L"All Open Documents");
         TargetCombo.AddString(L"All Project Files");
         TargetCombo.AddString(L"Scripts Folder");
         TargetCombo.SetCurSel(0);
         
         // Select 'FindResults1'
         this->CheckRadioButton(IDC_RESULTS1_RADIO, IDC_RESULTS2_RADIO, IDC_RESULTS1_RADIO);

         // Populate search terms
         for (auto str : PrefsLib.SearchTerms)
            FindCombo.AddString(str.c_str());

         // Populate replace terms
         for (auto str : PrefsLib.ReplaceTerms)
            ReplaceCombo.AddString(str.c_str());

         // Display: Expand/Contract appropriately
         if (!PrefsLib.ShowFindOptions)
            Expand(false);

         return TRUE;  
      }

      /// <summary>Finds the next match, if any</summary>
      void FindDialog::OnFind_Click()
      {
         try
         {
            // Validate
            if (!ValidateState(SearchCommand::Find))
               return;

            // Feedback
            Feedback(Search ? L"Find Next" : L"Find First");

            // Init: Create operation
            if (!Search)
               NewSearch(SearchCommand::Find);
            
            // FindNext:
            if (!Search->FindNext())
            {
               // Complete
               Reset();
               AfxMessageBox(L"Search complete");
            }
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e);
         }
      }

      /// <summary>Resets the operation</summary>
      void FindDialog::OnOptions_Changed(UINT nID)
      {
         Reset();
      }
      
      /// <summary>Finds all matches</summary>
      void FindDialog::OnFindAll_Click()
      {
         try
         {
            // Validate
            if (!ValidateState(SearchCommand::FindAll))
               return;

            // Feedback
            Feedback(L"Find All");

            // Find All
            NewSearch(SearchCommand::FindAll);
            Search->FindAll();
            Reset();
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e);
         }
      }
      
      /// <summary>Shows/Hides options</summary>
      void FindDialog::OnOptions_Click()
      {
         // Toggle window size
         Expand(!Expanded);
      }

      /// <summary>Replaces the current match, if any, and finds the next</summary>
      void FindDialog::OnReplace_Click()
      {
         try
         {
            // Validate
            if (!ValidateState(SearchCommand::Replace))
               return;

            // Feedback
            Feedback(L"Replace");

            // Init: Create operation
            if (!Search)
               NewSearch(SearchCommand::Replace);

            // Replace:
            if (!Search->Replace(GetReplaceTerm()))
            {
               // Complete
               Reset();
               AfxMessageBox(L"Search complete");
            }
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e);
         }
      }

      /// <summary>Replaces all matches</summary>
      void FindDialog::OnReplaceAll_Click()
      {
         try
         {
            // Validate
            if (!ValidateState(SearchCommand::ReplaceAll))
               return;

            // Feedback
            Feedback(L"Replace All");

            // Replace All
            NewSearch(SearchCommand::ReplaceAll);
            Search->ReplaceAll(GetReplaceTerm());
            Reset();
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e);
         }
      }
      
      /// <summary>Resets the operation</summary>
      void FindDialog::OnShowWindow(BOOL bShow, UINT nStatus)
      {
         // Show/Hide window
         CDialogEx::OnShowWindow(bShow, nStatus);

         // Start anew
         Reset();
      }
      
      // ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Writes the operation name to the console</summary>
      /// <param name="operation">The operation.</param>
      void  FindDialog::Feedback(const wstring& operation)
      {
         UpdateData();

         // Feedback
         Console << Cons::UserAction << operation << " "
                 << Cons::Yellow     << GetSearchTerm() 
                 << Cons::White      << " in " 
                 << Cons::Yellow     << GetString(GetSearchTarget()) 
                 << Cons::White      << (UseRegEx ? L" using Regular Expressions" : L"") 
                 << ENDL;
      }

      /// <summary>Creates a new search.</summary>
      void  FindDialog::NewSearch(SearchCommand cmd)
      {
         // NotImpl: Projects
         if (GetSearchTarget() == SearchTarget::ProjectFiles)
            throw NotImplementedException(HERE, L"Find operations on project files has not been implemented");

         // Set dialog state to active
         SetState(true);

         // Create new search
         Search.reset(new SearchOperation(GetOutputPane(),
                                          GetSearchTarget(), 
                                          GetSearchTerm(), 
                                          GetReplaceTerm(), 
                                          MatchCase != FALSE, 
                                          MatchWholeWord != FALSE, 
                                          UseRegEx != FALSE));

         // Activate appropriate output window
         theApp.GetMainWindow()->ActivateOutputPane(GetOutputPane(), false);

         // Add new terms to ComboBox
         switch (cmd)
         {
         case SearchCommand::Find:
         case SearchCommand::FindAll:
            if (!FindCombo.Contains(GetSearchTerm(), true))
               FindCombo.AddString(GetSearchTerm().c_str());
            break;

         case SearchCommand::Replace:
         case SearchCommand::ReplaceAll:
            if (!ReplaceCombo.Contains(GetReplaceTerm(), true))
               ReplaceCombo.AddString(GetReplaceTerm().c_str());
            break;
         }
      }

      /// <summary>Resets the operation.</summary>
      void  FindDialog::Reset()
      {
         // Set dialog state to inactive
         SetState(false);

         // Clear search
         Search.reset(nullptr);
      }

      /// <summary>Sets the state of the dialog.</summary>
      /// <param name="active">Whether operation is in progress.</param>
      void  FindDialog::SetState(bool active)
      {
         BOOL enabled = GetSearchTerm().length() ? TRUE : FALSE;

         // Set 'find' text
         FindButton.SetWindowTextW(active ? L"Find Next" : L"Find");

         // Require 'find' text before allowing operations
         FindButton.EnableWindow(enabled);
         FindAllButton.EnableWindow(enabled);
         ReplaceButton.EnableWindow(enabled);
         ReplaceAllButton.EnableWindow(enabled);

         // Disable output radios once operation has begun
         GetDlgItem(IDC_RESULTS1_RADIO)->EnableWindow(!active);
         GetDlgItem(IDC_RESULTS2_RADIO)->EnableWindow(!active);
      }

      /// <summary>Validates the app and dialog state before starting a command</summary>
      /// <param name="cmd">The command.</param>
      /// <returns>True if successful, False if operation should not be started</returns>
      bool  FindDialog::ValidateState(SearchCommand cmd)
      {
         // Require game data + search term
         if (theApp.State == AppState::NoGameData || !GetSearchTerm().length())
            return false;

         // Ensure document present for those that require it
         switch (GetSearchTarget())
         {
         case SearchTarget::Selection:
         case SearchTarget::Document:
         case SearchTarget::OpenDocuments:
            return DocumentBase::GetActive() != nullptr;

         default:
            return true;
         }
      }

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)


