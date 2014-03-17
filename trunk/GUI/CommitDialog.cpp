#include "stdafx.h"
#include "CommitDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CommitDialog, CDialog)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   CommitDialog::CommitDialog(CWnd* parent) 
       : CDialog(IDD_COMMIT, parent)
   {
   }

   CommitDialog::~CommitDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Stub.</summary>
   /// <returns></returns>
   BOOL CommitDialog::OnInitDialog()
   {
      if (!__super::OnInitDialog())
         return FALSE;

      // Centre
      CenterWindow();
      return TRUE;
   }
   
   /// <summary>Stub.</summary>
   void CommitDialog::OnOK()
   { 
      __super::OnOK();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  CommitDialog::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect wnd(this);

      // TODO: Layout code
   }
   
   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void CommitDialog::DoDataExchange(CDataExchange* pDX)
   {
      DDX_Text(pDX, IDC_COMMIT_EDIT, Description);
      __super::DoDataExchange(pDX);
   }
   
   /// <summary>Adjusts the layout on resize</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void CommitDialog::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)


