
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
#include <Richedit.h>
#include "Logic/RtfScriptWriter.h"
#include "Logic/ScriptParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTest1View

IMPLEMENT_DYNCREATE(CMFCTest1View, CFormView)

BEGIN_MESSAGE_MAP(CMFCTest1View, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
   ON_BN_CLICKED(IDC_LOADSCRIPT, &CMFCTest1View::OnBnClickedLoadScript)
   ON_WM_SIZE()
   ON_BN_CLICKED(IDC_RUNTESTS, &CMFCTest1View::OnBnClickedRuntests)
   ON_BN_CLICKED(IDC_COMPILE, &CMFCTest1View::OnBnClickedCompile)
END_MESSAGE_MAP()


// CMFCTest1View construction/destruction

CMFCTest1View::CMFCTest1View()
	: CFormView(CMFCTest1View::IDD)
{
	// TODO: add construction code here
   //MSFTEDIT_CLASS
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

#define ST_UNICODE (DWORD)8

void CMFCTest1View::OnBnClickedLoadScript()
{
   const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\plugin.piracy.enslavepassengers.xml";
   try
   {
      ScriptFile f( DebugTests::LoadScript(path) );
      
      

      string txt;
      RtfScriptWriter w(txt);
      w.Write(f);
      w.Close();

      /*SETTEXTEX opt = {ST_DEFAULT, CP_UTF8};
      unique_ptr<CHAR> utf8(new CHAR[txt.length()*2]);
      utf8.get()[WideCharToMultiByte(CP_UTF8, NULL, txt.c_str(), txt.length(), utf8.get(), txt.length()*2, NULL, NULL)] = NULL;
      
      m_RichEdit.SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)utf8.get());*/

      SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
      m_RichEdit.SetBackgroundColor(FALSE, RGB(0,0,0));
      m_RichEdit.SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)txt.c_str());
   }
   catch (ExceptionBase&  e)
   {
      CString sz;
      sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
      AfxMessageBox(sz);
   }
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


void CMFCTest1View::OnBnClickedRuntests()
{
   DebugTests::RunAll();
   AfxMessageBox(L"Tests complete");
}


void CMFCTest1View::OnBnClickedCompile()
{
   LineArray lines;
   WCHAR  buf[512];
   
   // Get text in lines
   for (INT i = 0; i < m_RichEdit.GetLineCount(); i++)
   {
      buf[Edit_GetLine(m_RichEdit.m_hWnd, i, buf, 512)]=NULL;
      lines.push_back(buf);
   }

   // Test parser
   DebugTests::CompileScript(lines);
}
