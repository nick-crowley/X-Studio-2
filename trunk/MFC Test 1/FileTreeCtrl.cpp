
#include "stdafx.h"
#include "FileTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /////////////////////////////////////////////////////////////////////////////
   // CFileTreeCtrl

   CFileTreeCtrl::CFileTreeCtrl()
   {
   }

   CFileTreeCtrl::~CFileTreeCtrl()
   {
   }

   BEGIN_MESSAGE_MAP(CFileTreeCtrl, CTreeCtrl)
   END_MESSAGE_MAP()

   /////////////////////////////////////////////////////////////////////////////
   // CFileTreeCtrl message handlers

   BOOL CFileTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
   {
	   BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	   NMHDR* pNMHDR = (NMHDR*)lParam;
	   ASSERT(pNMHDR != NULL);

	   if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	   {
		   GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	   }

	   return bRes;
   }
   
NAMESPACE_END2(GUI,Controls)
