#include "stdafx.h"
#include "DocumentBase.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DocumentBase, CDocument)

   BEGIN_MESSAGE_MAP(DocumentBase, CDocument)
      ON_COMMAND(ID_FILE_CLOSE, &DocumentBase::OnCommand_Close)
      ON_COMMAND(ID_FILE_RELOAD, &DocumentBase::OnCommand_Reload)
      ON_COMMAND(ID_FILE_SAVE, &DocumentBase::OnCommand_Save)
      ON_COMMAND(ID_FILE_SAVE_AS, &DocumentBase::OnCommand_SaveAs)
      ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_RELOAD, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, &DocumentBase::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   DocumentBase::DocumentBase() : IsVirtual(false)
   {
   }

   DocumentBase::DocumentBase(DocumentType type, bool isVirtual) : Type(type), IsVirtual(isVirtual)
   {
   }

   DocumentBase::~DocumentBase()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Gets the active script or language document</summary>
   /// <returns></returns>
   DocumentBase*  DocumentBase::GetActive()
   {
      CMDIChildWnd* pChild = theApp.GetMainWindow()->MDIGetActive();

      if (!pChild)
         return nullptr;

      return (DocumentBase*)pChild->GetActiveDocument();
   }
   
   /// <summary>Renames a file on disc.</summary>
   /// <param name="oldPath">The old path.</param>
   /// <param name="newPath">The new path.</param>
   /// <exception cref="Logic::ApplicationException">New file path already exists -or- project already contains new path</exception>
   /// <exception cref="Logic::IOException">Unable to rename file</exception>
   void DocumentBase::RenameFile(Path oldPath, Path newPath)
   {
      auto proj = ProjectDocument::GetActive();

      // Ensure unique
      if (newPath.Exists())
         throw ApplicationException(HERE, L"A file with that name already exists");
      else if (proj && proj->Contains(newPath))
         throw ApplicationException(HERE, L"Project already contains a file with that path");

      // Rename file
      if (!MoveFileEx(oldPath.c_str(), newPath.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
         throw IOException(HERE, SysErrorString());
   }
   
   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Activates this document</summary>
   void  DocumentBase::Activate()
   {
      auto pos = GetFirstViewPosition();

      // Activate parent frame of view (if any)
      if (auto view = GetNextView(pos))
         if (auto parent = view->GetParentFrame())
            parent->ActivateFrame();
   }
   
   /// <summary>Close the document, query to save if modified</summary>
   /// <returns>TRUE if successful, FALSE user cancelled or saving failed</returns>
   BOOL  DocumentBase::CloseModified()
   {
      // Feedback
      Console << Cons::UserAction << "Closing document" << ENDL;

      // [Modified] Query to save, return FALSE if cancelled/failed
      if (!SaveModified())
      {
         Console << "Closing sequence aborted" << ENDL;
		   return FALSE;
      }

      // [Saved/Unmodified] Close doc
	   OnCloseDocument();
      return TRUE;
   }
   
   /// <summary>Enable/disable detection of changes to the document file</summary>
   /// <param name="enable">Whether to enable or disable notifications.</param>
   /// <param name="view">view to receive notifications.</param>
   /// <returns>True if successful, otherwise false</returns>
   bool DocumentBase::DetectChanges(bool enable, CView* view)
   {
      try
      {
         // Enable: Start background worker
         if (enable && !FullPath.Empty())  // Require document have a path
            FileWatcher.Start(view, FullPath);

         // Disable: Stop background worker
         else if (!enable)
            FileWatcher.Stop();

         return true;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return false;
      }
   }

   /// <summary>Invoked by MFC to save the document.</summary>
   /// <param name="szPathName">Path to save under, or nullptr for 'Save As'</param>
   /// <param name="bReplace">TRUE to update project item, FALSE to preserve it</param>
   /// <returns>TRUE if saved, FALSE if cancelled/error</returns>
   BOOL DocumentBase::DoSave(LPCTSTR szPathName, BOOL /*bReplace*/)
   {
	   CString path = szPathName;
      auto proj = ProjectDocument::GetActive();

      try
      {
         // SaveAs/SaveCopyAs: 
         if (!szPathName)
         {
            // Query for new filename
            path = FullPath.c_str();
            if (!AfxGetApp()->DoPromptFileName(path, AFX_IDS_SAVEFILE, OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, FALSE, GetDocTemplate()))
               return FALSE;
         }

         // Save document
	      if (!OnSaveDocument(path))
	      {
            // SaveAs/SaveCopyAs: Delete partial file
		      if (!szPathName)
               DeleteFile(path);

		      return FALSE;
	      }

         // Preserve path
         auto oldPath = FullPath;

         // Update path/title/modified + project item
         FullPath = (LPCWSTR)path;
         SetModifiedFlag(FALSE);

         // Success: Raise 'AFTER SAVE'
	      OnDocumentEvent(onAfterSaveDocument);  // Refreshes backup window
	      return TRUE;     
      }
      // Renaming failed
      catch (ExceptionBase& e) 
      {
         theApp.ShowError(HERE, ApplicationException(e), VString(L"Unable to save document '%s'", (LPCWSTR)path));
         return FALSE;
      }
   }

   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="start">Starting offset</param>
   /// <param name="m">Match data</param>
   /// <returns>True if found, false otherwise</returns>
   bool  DocumentBase::FindNext(UINT start, MatchData& m) const
   {
      return false;
   }

   /// <summary>Gets the filename from the title, without the '*'</summary>
   /// <returns></returns>
   /// <remarks>If the file has been renamed the title represents the desired filename, whereas the fullpath contains the actual filename</remarks>
   wstring DocumentBase::GetFileName() const
   {
      // Trim '*' from end, if any
      return GuiString(GetTitle()).TrimRight(L"*");
   }

   /// <summary>Get the full document path.</summary>
   /// <returns></returns>
   Path  DocumentBase::GetFullPath() const
   {
      return Path((const wchar*)GetPathName());
   }
   
   /// <summary>Gets the text selection.</summary>
   /// <returns></returns>
   CHARRANGE  DocumentBase::GetSelection() const
   {
      return {0,0};
   }

   /// <summary>Get document type.</summary>
   /// <returns></returns>
   DocumentType  DocumentBase::GetType() const
   {
      return Type;
   }

   /// <summary>Get whether virtual.</summary>
   bool  DocumentBase::GetVirtual() const
   {
      return IsVirtual;
   }
   

   /// <summary>closes document.</summary>
   void  DocumentBase::OnCloseDocument() 
   {
      try
      {
         // Feedback
         Console << "Closing and destroying document: title=" << Cons::Yellow << GetTitle() << Cons::White << " path=" << FullPath << ENDL;

         // Stop file watcher
         if (FileWatcher.IsRunning())
            FileWatcher.Stop();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
      
      // Close/Destroy
      __super::OnCloseDocument();
   }


   /// <summary>Refresh revisions after opening/saving</summary>
   /// <param name="deEvent">The event.</param>
   void DocumentBase::OnDocumentEvent(DocumentEvent deEvent)
   {
      switch (deEvent)
      {
      // Close/Open/Save: Refresh backups
      case CDocument::onAfterCloseDocument:
      case CDocument::onAfterOpenDocument:
      case CDocument::onAfterSaveDocument:
         if (auto proj = ProjectDocument::GetActive())
            proj->RefreshRevisions(*this);
         break;
      }
   }

   /// <summary>Called when opening a document from a template.</summary>
   /// <param name="docPath">New document path</param>
   /// <param name="t">file template.</param>
   /// <returns></returns>
   BOOL  DocumentBase::OnOpenTemplate(Path docPath, const TemplateFile& t)
   {
      // Default implementation uses open document
      return OnOpenDocument(AppPath(t.SubPath).c_str());
   }

   /// <summary>Called when reloading a document from disc.</summary>
   /// <param name="szPathName">Full document path</param>
   /// <returns></returns>
   BOOL  DocumentBase::OnReloadDocument(LPCTSTR szPathName)
   {
      return OnOpenDocument(szPathName);
   }
   
   /// <summary>Renames the document if modified, otherwise renames the file in disc. Updates path/title/project appropriately</summary>
   /// <param name="newPath">New path.</param>
   /// <exception cref="Logic::ApplicationException">New path already exists, or project already contains new path</exception>
   /// <exception cref="Logic::IOException">Unable to rename file</exception>
   void  DocumentBase::Rename(Path newPath)
   {
      auto proj = ProjectDocument::GetActive();

      // Ensure path different
      if (FullPath == newPath)
         return;

      // Ensure document exists
      if (Virtual)
         throw AlgorithmException(HERE, L"Cannot rename virtual documents");

      // Project Member: Ensure new path unique within project
      if (proj && proj->Contains(FullPath) && proj->Contains(newPath))
         throw ApplicationException(HERE, L"Current project already contains a file with that name");

      // Modified: Update title only [and project item]
      if (IsModified())
         SetTitle(newPath.FileName.c_str());
      else
      {
         // Unmodified: Attempt to Rename file
         RenameFile(FullPath, newPath);

         // Update document path [and title and project item]
         auto oldPath = FullPath;
         FullPath = newPath;
      }
   }

   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   bool  DocumentBase::Replace(MatchData& m)
   {
      return false;
   }
   
   /// <summary>Invoked by MFC to save the modified document before closing.</summary>
   /// <returns>TRUE to continue closing, FALSE to abort</returns>
   BOOL DocumentBase::SaveModified()
   {
	   if (!IsModified())
		   return TRUE;        // ok to continue

      // Query save
      VString msg(L"The document '%s' has been modified, save changes?", FileName.c_str());
      switch (theApp.ShowMessage(Cons::Warning, msg, MB_ICONQUESTION|MB_YESNOCANCEL))
      {
      // Cancel: Abort closing
      case IDCANCEL:
		   return FALSE;

      // Yes: Attempt saving
      case IDYES:
		   if (!DoSave((FullPath.Folder+FileName).c_str(), TRUE))
			   return FALSE;       // Failed: Abort closing
		   break;
      }

      // Success: Close document
	   return TRUE;    
   }

   /// <summary>Changes the path of the document.</summary>
   /// <returns></returns>
   void  DocumentBase::SetFullPath(Path path) 
   {
      // Change path. (Don't add to MRU - will cause argument exception if file doesn't exist)
      SetPathName(path.c_str(), FALSE);

      // Update title
      SetTitle(path.FileName.c_str());
   }

   /// <summary>Sets/Clears the modified flag, updates title (and project item, if any)</summary>
   /// <param name="bModified">modified flag.</param>
   void  DocumentBase::SetModifiedFlag(BOOL bModified)
   {
      // Update flag
      __super::SetModifiedFlag(bModified);

      // Get title
      GuiString title = (LPCWSTR)GetTitle();
      if (title.empty())
         return;

      // Modified: Append *
      if (bModified && title.back() != '*')
         title += L'*';

      // Unmodified: Trim *
      else if (!bModified)
         title = title.TrimRight(L"*");

      // Update title
      __super::SetTitle(title.c_str());

      // Project: Update item
      if (auto proj = ProjectDocument::GetActive())
         proj->UpdateItem(*this);
   }

   /// <summary>Changes the document path (and relevant project item, if any)</summary>
   /// <param name="szPathName">New path.</param>
   /// <param name="bAddToMRU">Add to MRU [Path must exist].</param>
   void  DocumentBase::SetPathName(LPCTSTR szPathName, BOOL bAddToMRU /*= TRUE*/)
   {
      // Update path
      __super::SetPathName(szPathName, bAddToMRU);

      // Project: Update item
      if (auto proj = ProjectDocument::GetActive())
         proj->UpdateItem(*this);
   }

   /// <summary>Sets the selection.</summary>
   /// <param name="rng">char range.</param>
   void  DocumentBase::SetSelection(CHARRANGE rng)
   {
   }

   /// <summary>Changes the document title and relevant project item, if any.  (Adds a trailing '*' if modified)</summary>
   /// <param name="title">title.</param>
   void  DocumentBase::SetTitle(LPCTSTR title) 
   {
      wstring txt(title);

      // Modified: Append *  (if not already present)
      if (IsModified() && !txt.empty() && txt.back() != '*')
         txt += L"*";

      // Set title
      __super::SetTitle(txt.c_str());

      // Project: Update item
      if (auto proj = ProjectDocument::GetActive())
         proj->UpdateItem(*this);
   }
   
   /// <summary>Sets whether virtual.</summary>
   /// <param name="v">state.</param>
   void  DocumentBase::SetVirtual(bool v)
   {
      IsVirtual = v;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Query to save if modified</summary>
   void  DocumentBase::OnCommand_Close()
   {
      // Save/Closed Modified
      CloseModified();
   }
   
   /// <summary>Reload document.</summary>
   void  DocumentBase::OnCommand_Reload()
   {
      Console << Cons::UserAction << "Reloading document: " << GetTitle() << ENDL;

      // Modified: Query to save
      if (!SaveModified())
         return;

      // Reload document
      OnReloadDocument(FullPath.c_str());

      // Update all views
      UpdateAllViews(nullptr, DOCUMENT_RELOADED);
   }

   /// <summary>Save under existing path</summary>
   void  DocumentBase::OnCommand_Save()
   {
      // Save document under existing or modified path
      DoSave((FullPath.Folder+FileName).c_str(), TRUE);
   }

   /// <summary>Query for new path, rename document, and save.</summary>
   void  DocumentBase::OnCommand_SaveAs()
   {
      // Prompt for filename, save copy
      DoSave(nullptr, FALSE);
   }

   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void  DocumentBase::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      switch (pCmdUI->m_nID)
      {
      // Close: Always enabled
      case ID_FILE_CLOSE:    state = true;       break;

      // Save/SaveAs: Require file document
      case ID_FILE_SAVE:
      case ID_FILE_SAVE_AS:  state = !Virtual;   break;

      // Reload: Require file document
      case ID_FILE_RELOAD:   state = !Virtual;   break;

      // SaveAll: Always enabled
      case ID_FILE_SAVE_ALL: state = true;       break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
