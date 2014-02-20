#include "stdafx.h"

#include "OutputList.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(OutputList, CListCtrl)
      ON_WM_CREATE()
	   ON_WM_CONTEXTMENU()
	   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	   ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	   ON_WM_WINDOWPOSCHANGING()
      ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   OutputList::OutputList()
   {
   }

   OutputList::~OutputList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Add progress item to bottom of list.</summary>
   /// <param name="wp">The wp.</param>
   void OutputList::InsertItem(const WorkerProgress& wp)
   {
      // Create item
      LVItem item(GetItemCount(), wp.Text, NULL, LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT);
      item.iImage = (UINT)wp.Type;
      item.iIndent = wp.Indent;

      // Insert as last item
      CListCtrl::InsertItem(&item);
      EnsureVisible(GetItemCount()-1, FALSE);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   int OutputList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		   return -1;

      // Set font
      SetFont(&afxGlobalData.fontRegular);

      // Insert hidden column
      InsertColumn(0, L"Text");
      SetExtendedStyle(LVS_EX_FULLROWSELECT);
	   return 0;
   }

   void OutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
   {
	   CMenu menu;
	   menu.LoadMenu(IDM_OUTPUT_POPUP);

	   CMenu* pSumMenu = menu.GetSubMenu(0);

	   if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	   {
		   CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		   if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			   return;

		   ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		   UpdateDialogControls(this, FALSE);
	   }

	   SetFocus();
   }

   void OutputList::OnEditCopy()
   {
	   MessageBox(_T("Copy output"));
   }

   void OutputList::OnEditClear()
   {
	   MessageBox(_T("Clear output"));
   }

   void OutputList::OnViewOutput()
   {
	   CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	   CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	   if (pMainFrame != NULL && pParentBar != NULL)
	   {
		   pMainFrame->SetFocus();
		   pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		   pMainFrame->RecalcLayout();

	   }
   }

   
   void OutputList::OnSize(UINT nType, int cx, int cy)
   {
      CListCtrl::OnSize(nType, cx, cy);

      // Adjust column
      CRect  rc;
      GetClientRect(&rc);
      SetColumnWidth(0, rc.Width());
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


