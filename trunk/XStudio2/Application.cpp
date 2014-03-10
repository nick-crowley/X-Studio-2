
// MFC Test 1.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "afxcview.h"
#include "Application.h"
#include "AboutDlg.h"
#include "NewDocumentDialog.h"
#include "MainWnd.h"

#include "ScriptDocument.h"
#include "LanguageDocument.h"
#include "ProjectDocument.h"
#include "DiffDocument.h"

#include "Logic/ConsoleLog.h"

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
	ON_COMMAND(ID_APP_ABOUT, &Application::OnCommandAbout)
	ON_COMMAND(ID_FILE_NEW, &Application::OnCommandNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
   ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &Application::OnQueryCommand)
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &Application::OnQueryCommand)
   ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, &Application::OnQueryCommand)
END_MESSAGE_MAP()

// -------------------------------- CONSTRUCTION --------------------------------

Application::Application() : GameDataState(AppState::NoGameData)
{
	SetAppID(_T("BearWare.X-Studio.2"));
}

// ------------------------------- STATIC METHODS -------------------------------

void  Application::OnCriticalError()
{
   try
   {
      // Ensure console is valid RTF by appending footer
      LogFile.Close();
   }
   catch (ExceptionBase&) {
   }
}

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
   try
   {
      // Free resources
      FreeLibrary(ResourceLibrary);
      ResourceLibrary = NULL;

      // Close LogFile
      LogFile.Close();
   }
   catch (ExceptionBase& e) {
      theApp.ShowError(HERE, e);
   }
	
	return CWinAppEx::ExitInstance();
}


/// <summary>Gets an open document</summary>
/// <param name="p">Full path.</param>
/// <returns>Document if open, otherwise nullptr</returns>
DocumentBase*  Application::GetOpenDocument(IO::Path p) const
{
   // Find by path
   for (auto& doc : *this)
      if (doc.GetFullPath() == p)
         return &doc;
   
   // Not found: 
   return nullptr;
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
   return dynamic_cast<MainWnd*>(theApp.m_pMainWnd);
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
	// Visual Leak Detector
   VLDEnable();

   try
   {
      // Set termination handler
      set_terminate(OnCriticalError);

      // Load resource library
      if(ResourceLibrary = LoadLibrary(L"X-Studio II.Resources.dll"))
         AfxSetResourceHandle(ResourceLibrary);
      else
         throw Win32Exception(HERE, L"Unable to load resource library");

      // LogFile
      LogFile.Open();

	   // Init common controls
	   INITCOMMONCONTROLSEX InitCtrls;
	   InitCtrls.dwSize = sizeof(InitCtrls);
	   InitCtrls.dwICC = ICC_WIN95_CLASSES;
	   if (!InitCommonControlsEx(&InitCtrls))
         throw Win32Exception(HERE, L"Unable to initialize common controls library");

      // Initialise OLE/COM
	   CWinAppEx::InitInstance();
      AfxOleInit();

      // Sockets
	   if (!AfxSocketInit())
         throw Win32Exception(HERE, L"Windows sockets initialization failed.");

      // ITaskBar
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

      // Create window fonts
      UpdateFonts();

	   // document templates
	   AddDocTemplate(new ScriptDocTemplate());
	   AddDocTemplate(new LanguageDocTemplate());
      AddDocTemplate(new ProjectDocTemplate());
      AddDocTemplate(new DiffDocTemplate());

	   // Frame window
	   m_pMainWnd = new MainWnd;
	   if (!dynamic_cast<MainWnd*>(m_pMainWnd)->LoadFrame(IDR_MAINFRAME))
	   {
		   delete m_pMainWnd;
		   return FALSE;
	   }

	   // Show window
	   m_pMainWnd->ShowWindow(m_nCmdShow);
	   m_pMainWnd->UpdateWindow();

	   return TRUE;
   }
   catch (ExceptionBase& e) {
      theApp.ShowError(HERE, e);
      return FALSE;
   }
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

/// <summary>Query whether main window is minimized</summary>
/// <returns></returns>
bool Application::IsMimized() const
{
   return m_pMainWnd && m_pMainWnd->IsIconic();
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
   // Load bitmap
   HBITMAP h = (HBITMAP)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_BITMAP, cx, cy, flags);
   if (!h)
      throw Win32Exception(HERE, L"Failed to load bitmap");

   // Attach to CBitmap
   CBitmap* bmp = new CBitmap();
   bmp->Attach(h);
   return bmp;
}


/// <summary>Dispay about box</summary>
void Application::OnCommandAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/// <summary>Dispay new document dialog</summary>
void Application::OnCommandNew()
{
	NewDocumentDialog dlg;
	dlg.DoModal();
}

/// <summary>Update all windows</summary>
void Application::OnPreferencesChanged()
{
   // Update fonts
   UpdateFonts();

   // Raise 'PREFERENCES CHANGED'
   GetMainWindow()->SendMessageToDescendants(WM_SETTINGCHANGE);
}

/// <summary>Opens the string library.</summary>
/// <returns></returns>
DocumentBase* Application::OpenStringLibrary()
{
   // Ensure not already open
   if (auto doc = GetOpenDocument(L"String Library"))
      return doc;
   
   // Open manually
   auto templ = GetDocumentTemplate<LanguageDocTemplate>();
   return (DocumentBase*)templ->OpenDocumentFile(L"String Library", FALSE, TRUE);
}

/// <summary>Query file menu item state</summary>
void Application::OnQueryCommand(CCmdUI* pCmdUI)
{
   bool state = false;
   
   // Query 
   switch (pCmdUI->m_nID)
   {
   case ID_FILE_NEW:
   case ID_FILE_OPEN:
   case ID_FILE_MRU_FILE1:
   case ID_FILE_MRU_FILE2:
   case ID_FILE_MRU_FILE3:
   case ID_FILE_MRU_FILE4:
   case ID_FILE_MRU_FILE5:
   case ID_FILE_MRU_FILE6:
   case ID_FILE_MRU_FILE7:
   case ID_FILE_MRU_FILE8:
   case ID_FILE_MRU_FILE9:
   case ID_FILE_MRU_FILE10:
   case ID_FILE_MRU_FILE11:
   case ID_FILE_MRU_FILE12:
   case ID_FILE_MRU_FILE13:
   case ID_FILE_MRU_FILE14:
   case ID_FILE_MRU_FILE15:
   case ID_FILE_MRU_FILE16:
      state = State == AppState::GameDataPresent;
      break;
   }

   // Set state
   pCmdUI->Enable(state ? TRUE : FALSE);
}

/// <summary>Application customization load/save methods</summary>
void Application::PreLoadState()
{
	GetContextMenuManager()->AddMenu(GuiString(IDS_EDIT_MENU).c_str(), IDM_EDIT_POPUP);
	GetContextMenuManager()->AddMenu(GuiString(IDR_PROJECTVIEW).c_str(), IDM_PROJECT_POPUP);
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
BOOL Application::ShowError(const GuiString& src, const exception& e, const GuiString& msg) const
{
   Console.Log(src, e);
   return AfxMessageBox(GuiString(L"%s : %s\n\nCaught: %s", msg.c_str(), 
                                                            StringResource::Convert(e.what(), CP_ACP).c_str(), 
                                                            src.c_str()).c_str(), MB_ICONERROR|MB_OK);
}

/// <summary>Displays and logs an exception</summary>
/// <param name="src">The handler location</param>
/// <param name="e">The exception</param>
/// <param name="msg">The display message</param>
/// <returns></returns>
BOOL Application::ShowError(const GuiString& src, const ExceptionBase& e, const GuiString& msg) const
{
   Console.Log(src, e, msg);

   // Application exception: Display verbatim
   if (auto app = dynamic_cast<const ApplicationException*>(&e))
      return AfxMessageBox(app->Message.c_str());
   
   // Exception: Display source/sink data
   return AfxMessageBox(GuiString(L"%s : %s\n\nSink: %s\nSource: %s", msg.c_str(), e.Message.c_str(), src.c_str(), e.Source.c_str()).c_str(), MB_ICONERROR|MB_OK);
}


/// <summary>Displays and logs an exception</summary>
/// <param name="src">The handler location</param>
/// <param name="e">The exception</param>
/// <returns></returns>
BOOL Application::ShowError(const GuiString& src, const ExceptionBase& e) const
{
   Console.Log(src, e);

   // Application exception: Display verbatim
   if (auto app = dynamic_cast<const ApplicationException*>(&e))
      return AfxMessageBox(app->Message.c_str());

   // Exception: Display source/sink data
   return AfxMessageBox(GuiString(L"%s\n\nSink: %s\nSource: %s", e.Message.c_str(), src.c_str(), e.Source.c_str()).c_str(), MB_ICONERROR|MB_OK);
}

/// <summary>Re-creates the window fonts.</summary>
void Application::UpdateFonts()
{
   // Cleanup previous
   ToolWindowFont.DeleteObject();
   TooltipFont.DeleteObject();

   // Toolwindow
   auto lf = PrefsLib.ToolWindowFont;
   ToolWindowFont.CreateFontIndirectW(&lf);

   // Tooltip
   lf = PrefsLib.TooltipFont;
   TooltipFont.CreateFontIndirectW(&lf);
}

// ------------------------------ PROTECTED METHODS -----------------------------

// ------------------------------- PRIVATE METHODS ------------------------------


