
// Application.cpp : Defines the class behaviors for the application.
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

#include "../Logic/ConsoleLog.h"

//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------- GLOBAL --------------------------------

Application theApp;

// --------------------------------- TYPES ---------------------------------

// --------------------------------- APP WIZARD ---------------------------------
  
BEGIN_MESSAGE_MAP(Application, AppBase)
	ON_COMMAND(ID_APP_ABOUT, &Application::OnCommand_About)
	ON_COMMAND(ID_FILE_NEW, &Application::OnCommand_New)
	ON_COMMAND(ID_FILE_OPEN, &Application::OnCommand_Open)
   ON_COMMAND(ID_FILE_IMPORT, &Application::OnCommand_Import)
   ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &Application::OnQueryCommand)
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &Application::OnQueryCommand)
   ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, &Application::OnQueryCommand)
END_MESSAGE_MAP()

// -------------------------------- CONSTRUCTION --------------------------------

Application::Application() : GameDataState(AppState::NoGameData)
{
	
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
   Console << Cons::UserAction << "Performing main cleanup..." << ENDL;

   return __super::ExitInstance();
}


/// <summary>Gets an open document</summary>
/// <param name="p">Full path.</param>
/// <returns>Document if open, otherwise nullptr</returns>
DocumentBase*  Application::GetOpenDocument(Path p) const
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
//GuiString  Application::GetProfileSectionPath(const wstring& section) const
//{
//   return VString(L"SOFTWARE\\Bearware\\X-Studio II\\%s", section.c_str());
//}

/// <summary>Gets the registry path of section</summary>
/// <param name="section">section name</param>
/// <param name="subsection">sub-section name</param>
/// <returns></returns>
//GuiString  Application::GetProfileSectionPath(const wstring& section, const wstring& subsection) const
//{
//   return VString(L"SOFTWARE\\Bearware\\X-Studio II\\%s\\%s", section.c_str(), subsection.c_str());
//}

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
   try
   {
      // Initialise base
	   __super::InitInstance();
      Console << Cons::UserAction << "Performing main initialization..." << ENDL;

      try
      {
         Console << "Performing version check..." << ENDL;

         // Check for upgrade
         if (PrefsLib.AppVersion < BUILD_VERSION)
         {
            // Upgrade: Inform user
            VString msg(L"An upgrade to version %s has been detected. Your previous preferences (if any) have been preserved, but window placements have been reset.", BUILD_NAME);
            theApp.ShowMessage(Cons::Warning, msg, MB_OK|MB_ICONINFORMATION);

            // Delete 'workspace' key + Store new version
            PrefsLib.ResetWorkspace();
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }

      // Store build version
      PrefsLib.AppVersion = BUILD_VERSION;
      
      // Sockets
	   /*if (!AfxSocketInit())
         throw Win32Exception(HERE, L"Windows sockets initialization failed.");*/

      // ITaskBar
	   EnableTaskbarInteraction();

	   // Initialise RichEdit
      AfxInitRichEdit2();
	   AfxInitRichEdit5();

	
	   // Set app registry key 
	   //SetRegistryKey(L"Bearware");
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

      // Feedback
      Console << Cons::Success << "Initialization complete" << ENDL
              << Cons::UserAction << "Creating main window" << ENDL;

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
bool Application::IsDocumentOpen(Path p) const
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
      throw Win32Exception(HERE, VString(L"Failed to load icon %d (%dx%d)", nResID, iSize, iSize));

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

/// <summary>Open files dragged onto the window from windows explorer.</summary>
/// <param name="hDropInfo">The drop information.</param>
void Application::OnDropFiles(HDROP hDropInfo)
{
   const static UINT DROP_COUNT = 0xFFFFFFFF;

   Path path;
      
   // Feedback
   Console << Cons::UserAction << "Opening files dropped onto main window" << ENDL;

   // Iterate thru file paths
   for (UINT i = 0, count = DragQueryFile(hDropInfo, DROP_COUNT, NULL, 0); i < count; ++i)
   {
      DragQueryFile(hDropInfo, i, (wchar*)path, MAX_PATH);

      // Attempt to open file
      if (!theApp.OpenDocumentFile(path.c_str()))
      {
         Console << Cons::Warning << "Unable to open document: " << path << ENDL;
         theApp.ShowMessage(wstring(L"Unable to open document: ")+path.c_str(), MB_OK|MB_ICONERROR);
      }
   }
       
   // Cleanup
   Console << Cons::UserAction << "Finished opening drag'drop files" << ENDL;
   DragFinish(hDropInfo);
}

/// <summary>Update all windows</summary>
void Application::OnPreferencesChanged()
{
   // Update fonts
   UpdateFonts();

   // Raise 'PREFERENCES CHANGED'
   GetMainWindow()->SendMessageToDescendants(WM_SETTINGCHANGE);
}


/// <summary>Opens a difference document.</summary>
/// <param name="doc">The document.</param>
/// <param name="diff">The difference.</param>
/// <returns></returns>
DiffDocument* Application::OpenDiffDocument(ScriptDocument& doc, const wstring& diff)
{
   // Ensure not already open
   /*if (auto doc = GetOpenDocument(L"String Library"))
      return doc;*/
   
   // Open manually
   auto templ = GetDocumentTemplate<DiffDocTemplate>();
   return templ->OpenDocumentFile(doc, diff);
}

/// <summary>Opens a document file and adds to the MRU</summary>
/// <param name="szFileName">Full path</param>
/// <returns></returns>
CDocument* Application::OpenDocumentFile(const wchar* szFileName)
{
   return OpenDocumentFile(szFileName, TRUE);
}
   
/// <summary>Opens a document file.</summary>
/// <param name="szFileName">Full path</param>
/// <param name="bAddToMRU">add to MRU.</param>
/// <returns></returns>
CDocument* Application::OpenDocumentFile(const wchar* szFileName, BOOL bAddToMRU)
{
   try
   {
      REQUIRED(szFileName);

      // Strip inverted commas + Resolve links
      auto path = Path(GuiString(szFileName).Trim(L"\"")).Resolve;
      
      // Prepare
      CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
      CDocTemplate* bestTemplate = nullptr;
      DocumentBase* doc = nullptr;

      // Search templates
      for (auto pos = theApp.GetFirstDocTemplatePosition(); pos; )
      {
         CDocTemplate::Confidence match;

         // Match file vs template
         auto t = theApp.GetNextDocTemplate(pos);
		   match = t->MatchDocType(path.c_str(), (CDocument*&)doc);
		  
         // Open: Activate + return
		   if (match == CDocTemplate::yesAlreadyOpen)
         {
            REQUIRED(doc);
            doc->Activate();
            return doc;
         }
         // Better match: Record
         else if (match > bestMatch)
		   {
			   bestMatch = match;
			   bestTemplate = t;
		   }
      }

      // Found: Open file
      if (bestTemplate)
         return bestTemplate->OpenDocumentFile(path.c_str(), bAddToMRU, TRUE);

      // Unrecognised: Return null
      Console << "Unable to match " << path << " against any document template" << ENDL;
      return nullptr;
   }
   catch (ExceptionBase& e)
   {
      Console.Log(HERE, e);
      return nullptr;
   }
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

/// <summary>Application customization load/save methods</summary>
void Application::PreLoadState()
{
   // Load context menus
	GetContextMenuManager()->AddMenu(GuiString(IDM_EDIT_POPUP).c_str(), IDM_EDIT_POPUP);
   GetContextMenuManager()->AddMenu(GuiString(IDM_OUTPUT_POPUP).c_str(), IDM_OUTPUT_POPUP);
   GetContextMenuManager()->AddMenu(GuiString(IDM_PROJECT_POPUP).c_str(), IDM_PROJECT_POPUP);
   GetContextMenuManager()->AddMenu(GuiString(IDM_BACKUP_POPUP).c_str(), IDM_BACKUP_POPUP);
	GetContextMenuManager()->AddMenu(GuiString(IDM_SCRIPTEDIT_POPUP).c_str(), IDM_SCRIPTEDIT_POPUP);
   GetContextMenuManager()->AddMenu(GuiString(IDM_STRINGVIEW_POPUP).c_str(), IDM_STRINGVIEW_POPUP);
   GetContextMenuManager()->AddMenu(GuiString(IDM_GAMEDATA_POPUP).c_str(), IDM_GAMEDATA_POPUP);
}


/// <summary>Sets the game data state.</summary>
/// <param name="s">state</param>
void  Application::SetState(AppState s) 
{
   GameDataState = s;
   StateChanged.Raise(s);
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

/// <summary>Dispay about box</summary>
void Application::OnCommand_About()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/// <summary>Dispay new document dialog</summary>
void Application::OnCommand_New()
{
   // Feedback
   Console << Cons::UserAction << L"User invoked new Document dialog" << ENDL;

   // Display new document dialog
	NewDocumentDialog dlg;
	dlg.DoModal();
}

/// <summary>Display open file dialog</summary>
void Application::OnCommand_Import()
{
   static const wchar* filter = L"Legacy Custom Commands (*.txt)|*.txt|" 
                                L"All files (*.*)|*.*||";
   try
   {
      // Feedback
      Console << Cons::UserAction << "Import legacy custom syntax file" << ENDL;

      auto folder = PrefsLib.OpenDocumentFolder;
         
      // Query for file
	   CFileDialog dlg(TRUE, L".txt", L"Custom.Syntax.txt", OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, filter, m_pMainWnd, 0, TRUE);
      if (dlg.DoModal() == IDOK)
      {
         // Store last folder
         Path path = (LPCWSTR)dlg.GetPathName();
         PrefsLib.OpenDocumentFolder = path.Folder;

         // Feedback
         Console << "User selected: " << path << ENDL;

         // Query whether to merge with SyntaxLib
         auto msg = L"Would you like to merge the currently loaded commands with selected file into a new syntax file?";
         bool merge = theApp.ShowMessage(Cons::Warning, msg, MB_YESNO|MB_ICONQUESTION) == IDYES;

         // Query for output path
         CFileDialog save(FALSE, L".xml", L"Upgraded.Syntax.xml", OFN_ENABLESIZING|OFN_EXPLORER|OFN_OVERWRITEPROMPT, L"Xml Files (*.xml)|*.xml||", m_pMainWnd, 0, TRUE);
         if (dlg.DoModal() == IDOK)
            // Generate + Save
            SyntaxLib.Upgrade(path, (LPCWSTR)save.GetPathName(), merge);
         else
            // Feedback
            Console << "Action cancelled" << ENDL;
      }
   }
   catch (ExceptionBase& e) {
      theApp.ShowError(HERE, e);
   }
}

/// <summary>Display open file dialog</summary>
void Application::OnCommand_Open()
{
   static const wchar* filter = L"All Supported Files (*.xml,*.pck,*.xprj)|*.xml;*.pck;*.xprj|" 
                                L"Uncompressed Files (*.xml)|*.xml|" 
                                L"Compressed Files (*.pck)|*.pck|" 
                                L"Project Files (*.xprj)|*.xprj|" 
                                L"All files (*.*)|*.*||";
   try
   {
      auto folder = PrefsLib.OpenDocumentFolder;
         
      // Query for file
	   CFileDialog dlg(TRUE, L".xml", L"", OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, filter, m_pMainWnd, 0, TRUE);
      if (dlg.DoModal() == IDOK)
      {
         // Store last folder
         Path path = (LPCWSTR)dlg.GetPathName();
         PrefsLib.OpenDocumentFolder = path.Folder;

         // Open document
         OpenDocumentFile(path.c_str(), TRUE);
      }
   }
   catch (ExceptionBase& e) {
      theApp.ShowError(HERE, e);
   }
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

// ------------------------------- PRIVATE METHODS ------------------------------


