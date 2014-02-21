
// MFC Test 1.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "afxcview.h"
#include "Application.h"
#include "AboutDlg.h"
#include "MainWnd.h"

#include "ScriptDocument.h"
#include "LanguageDocument.h"
#include "ProjectDocument.h"

//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------- GLOBAL --------------------------------

Application theApp;

// --------------------------------- TYPES ---------------------------------

// --------------------------------- APP WIZARD ---------------------------------
  
BEGIN_MESSAGE_MAP(Application, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Application::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// -------------------------------- CONSTRUCTION --------------------------------

Application::Application() : GameDataState(AppState::NoGameData)
{
	SetAppID(_T("BearWare.X-Studio.2"));
}

// ------------------------------- STATIC METHODS -------------------------------

// ------------------------------- PUBLIC METHODS -------------------------------

/// <summary>Get document start iterator.</summary>
/// <returns></returns>
Application::DocumentIterator  Application::begin()
{
   return DocumentIterator(*this, this->GetFirstDocTemplatePosition());
}

/// <summary>Get document end iterator.</summary>
/// <returns></returns>
Application::DocumentIterator  Application::end()
{
   return DocumentIterator(*this, nullptr);
}

/// <summary>Get document start iterator.</summary>
/// <returns></returns>
Application::DocumentIterator  Application::begin() const
{
   return DocumentIterator(*this, this->GetFirstDocTemplatePosition());
}

/// <summary>Get document end iterator.</summary>
/// <returns></returns>
Application::DocumentIterator  Application::end() const
{
   return DocumentIterator(*this, nullptr);
}


/// <summary>Exits the instance.</summary>
/// <returns></returns>
int Application::ExitInstance()
{
   // Free resources
   FreeLibrary(ResourceLibrary);
   ResourceLibrary = NULL;

	
	return CWinAppEx::ExitInstance();
}


/// <summary>Gets an open document</summary>
/// <param name="p">Full path.</param>
/// <returns></returns>
/// <exception cref="Logic::InvalidOperationException">Document not found</exception>
DocumentBase&  Application::GetDocument(IO::Path p) const
{
   // Find by path
   for (auto& doc : *this)
      if (doc.GetFullPath() == p)
         return doc;
   
   // Not found: Error
   throw InvalidOperationException(HERE, GuiString(L"Cannot find document '%s'", p.c_str()));
}


/// <summary>Get all open documents</summary>
/// <returns></returns>
DocumentList  Application::GetOpenDocuments() const
{
   DocumentList docs;

   // Enumerate docs
   for (auto& doc : *this)
      docs.push_back(&doc);

   // Return list
   return docs;
}


/// <summary>Gets the main window.</summary>
/// <returns></returns>
GUI::Windows::MainWnd*  Application::GetMainWindow() const
{
   return dynamic_cast<MainWnd*>(AfxGetMainWnd());
}


/// <summary>Gets the registry path of section</summary>
/// <param name="section">section name</param>
/// <returns></returns>
GuiString  Application::GetProfileSectionPath(const wstring& section) const
{
   return GuiString(L"SOFTWARE\\Bearware\\X-Studio II\\%s", section.c_str());
}

/// <summary>Gets the registry path of section</summary>
/// <param name="section">section name</param>
/// <param name="subsection">sub-section name</param>
/// <returns></returns>
GuiString  Application::GetProfileSectionPath(const wstring& section, const wstring& subsection) const
{
   return GuiString(L"SOFTWARE\\Bearware\\X-Studio II\\%s\\%s", section.c_str(), subsection.c_str());
}

/// <summary>Get game data state.</summary>
/// <returns></returns>
AppState  Application::GetState() const
{
   return GameDataState;
}


/// <summary>Initializes the instance.</summary>
/// <returns></returns>
BOOL Application::InitInstance()
{
	
   VLDEnable();
   //VLDDisable();
   //_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
   
   // Load resource library
   ResourceLibrary = LoadLibrary(_T("X-Studio II.Resources.dll"));
   if(ResourceLibrary)
      AfxSetResourceHandle(ResourceLibrary);
   else
   {
      AfxMessageBox(L"Failed to load resource library");
      return FALSE;
   }


	// Init common controls
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

   // Initialise OLE/COM
	CWinAppEx::InitInstance();
   AfxOleInit();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDS_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	// Initialise RichEdit
   AfxInitRichEdit2();
	AfxInitRichEdit5();

	
	// Set app registry key 
	SetRegistryKey(L"Bearware");
	LoadStdProfileSettings(10);  // Load MRU

   // Menu manager
	InitContextMenuManager();

   // Keyboard manager
	InitKeyboardManager();

   // Tooltip manager
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// document templates
	AddDocTemplate(new ScriptDocTemplate());
	AddDocTemplate(new LanguageDocTemplate());
   AddDocTemplate(new ProjectDocTemplate());

	// Frame window
	MainWnd* pMainFrame = new MainWnd;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Show window
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/// <summary>Query whether a document is open</summary>
/// <param name="p">Full path</param>
/// <returns></returns>
bool Application::IsDocumentOpen(IO::Path p) const
{
   // Find by path
   for (const auto& doc : *this)
      if (doc.GetFullPath() == p)
         return true;

   // Not found
   return false;
}


/// <summary>Query whether a document is still open</summary>
/// <param name="doc">Document</param>
/// <returns></returns>
bool Application::IsDocumentOpen(DocumentBase* d) const
{
   // Find by address
   for (const auto& doc : *this)
      if (&doc == d)
         return true;

   // Not found
   return false;
}


/// <summary>Loads an icon.</summary>
/// <param name="nResID">The resource identifier.</param>
/// <param name="iSize">Size of the icon</param>
/// <returns></returns>
/// <exception cref="Logic::Win32Exception">Failed to load icon</exception>
HICON  Application::LoadIconW(UINT nResID, UINT iSize) const
{
   HICON icon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_ICON, iSize, iSize, 0);
   
   // Ensure loaded
   if (icon == nullptr)
      throw Win32Exception(HERE, GuiString(L"Failed to load icon %d (%dx%d)", nResID, iSize, iSize));

   return icon;
}


/// <summary>Loads a bitmap.</summary>
/// <param name="nResID">The resource identifier.</param>
/// <param name="cx">The width.</param>
/// <param name="cy">The height.</param>
/// <param name="flags">Loading flags.</param>
/// <returns></returns>
CBitmap*  Application::LoadBitmapW(UINT nResID, int cx, int cy, UINT flags) const
{
   CBitmap* bmp = new CBitmap();
   HBITMAP h = (HBITMAP)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_BITMAP, cx, cy, flags);
   bmp->Attach(h);
   return bmp;
}


/// <summary>Dispay about box</summary>
void Application::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/// <summary>Opens the string library.</summary>
/// <returns></returns>
BOOL  Application::OpenStringLibrary()
{
   for (POSITION pos = GetFirstDocTemplatePosition(); pos != NULL; )
   {
      LanguageDocTemplate* doc = dynamic_cast<LanguageDocTemplate*>(GetNextDocTemplate(pos));
      if (doc != nullptr)
         return doc->OpenDocumentFile(L"String Library", FALSE, TRUE) != nullptr;
   }
   return FALSE;
}


/// <summary>Application customization load/save methods</summary>
void Application::PreLoadState()
{
	GetContextMenuManager()->AddMenu(GuiString(IDS_EDIT_MENU).c_str(), IDM_EDIT_POPUP);
	GetContextMenuManager()->AddMenu(GuiString(IDR_PROJECT).c_str(), IDM_PROJECT_POPUP);
}


/// <summary>Sets the game data state.</summary>
/// <param name="s">state</param>
void  Application::SetState(AppState s) 
{
   GameDataState = s;
   StateChanged.Raise(s);
}


/// <summary>Displays and logs an exception</summary>
/// <param name="src">The handler location</param>
/// <param name="e">The exception</param>
/// <param name="msg">The display message</param>
/// <returns></returns>
BOOL Application::ShowError(const GuiString& src, const ExceptionBase& e, const GuiString& msg) const
{
   Console.Log(src, e, msg);
   return AfxMessageBox(GuiString(L"%s : %s\n\nSink: %s\nSource: %s", msg.c_str(), e.Message.c_str(), src.c_str(), e.Source.c_str()).c_str(), MB_ICONERROR|MB_OK);
}


/// <summary>Displays and logs an exception</summary>
/// <param name="src">The handler location</param>
/// <param name="e">The exception</param>
/// <returns></returns>
BOOL Application::ShowError(const GuiString& src, const ExceptionBase& e) const
{
   Console.Log(src, e);
   return AfxMessageBox(GuiString(L"%s\n\nSink: %s\nSource: %s", e.Message.c_str(), src.c_str(), e.Source.c_str()).c_str(), MB_ICONERROR|MB_OK);
}

// ------------------------------ PROTECTED METHODS -----------------------------

// ------------------------------- PRIVATE METHODS ------------------------------


