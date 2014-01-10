
// MFC Test 1View.cpp : implementation of the CMFCTest1View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFC Test 1.h"
#endif

#include "MFC Test 1Doc.h"
#include "MFC Test 1View.h"

#include "Logic/DebugTests.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTest1View

IMPLEMENT_DYNCREATE(CMFCTest1View, CFormView)

BEGIN_MESSAGE_MAP(CMFCTest1View, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
   ON_BN_CLICKED(IDC_MFCBUTTON1, &CMFCTest1View::OnBnClickedMfcbutton1)
   ON_WM_SIZE()
END_MESSAGE_MAP()

// CMFCTest1View construction/destruction

CMFCTest1View::CMFCTest1View()
	: CFormView(CMFCTest1View::IDD)
{
	// TODO: add construction code here
}

CMFCTest1View::~CMFCTest1View()
{
}

void CMFCTest1View::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_RICHEDIT, m_RichEdit);
}

BOOL CMFCTest1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CMFCTest1View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

}

void CMFCTest1View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCTest1View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCTest1View diagnostics

#ifdef _DEBUG
void CMFCTest1View::AssertValid() const
{
	CFormView::AssertValid();
}

void CMFCTest1View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMFCTest1Doc* CMFCTest1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCTest1Doc)));
	return (CMFCTest1Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCTest1View message handlers


void CMFCTest1View::OnBnClickedMfcbutton1()
{
   // TODO: Add your control notification handler code here
   //DebugTests::RunAll();

   ScriptFile f( DebugTests::LoadScript(L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\plugin.piracy.enslavepassengers.xml") );
   wstring txt;

   for (ScriptCommand s : f.Commands)
      txt += s.Syntax.Text + L'\n';

   m_RichEdit.SetWindowTextW(txt.c_str());
}


void CMFCTest1View::OnSize(UINT nType, int cx, int cy)
{
   CFormView::OnSize(nType, cx, cy);

   // TODO: Add your message handler code here
   if (m_RichEdit.m_hWnd != nullptr)
   {
      CRect edit, wnd;
      GetWindowRect(&wnd);
      m_RichEdit.GetWindowRect(&edit);

      m_RichEdit.SetWindowPos(nullptr, NULL, NULL, wnd.Width() -(edit.left-wnd.left)-10, wnd.Height() -(edit.top-wnd.top)-10, SWP_NOMOVE | SWP_NOZORDER);
   }
}
