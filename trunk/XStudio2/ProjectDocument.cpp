#include "stdafx.h"
#include "ProjectDocument.h"
#include "ImportProjectDialog.h"
#include "Logic/FileIdentifier.h"
#include "Logic/LegacyProjectFileReader.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary>Project has been loaded/unloaded</summary>
   SimpleEvent   ProjectDocument::Loaded;

   /// <summary>Project item has been added, changed or removed</summary>
   ProjectItemEvent  ProjectDocument::ItemAdded,
                     ProjectDocument::ItemChanged,
                     ProjectDocument::ItemRemoved;

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(ProjectDocTemplate, CSingleDocTemplate)
   
   /// <summary>Creates script document template</summary>
   ProjectDocTemplate::ProjectDocTemplate()
         : CSingleDocTemplate(IDR_PROJECTVIEW, RUNTIME_CLASS(ProjectDocument), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CView))
   {}

   /// <summary>Queries whether an external file should be opened as a script</summary>
   /// <param name="szPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence  ProjectDocTemplate::MatchDocType(LPCTSTR szPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CSingleDocTemplate::MatchDocType(szPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(szPathName) == FileType::Project ? yesAttemptNative : noAttempt;
   }

   CFrameWnd*  ProjectDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) 
   {
      Console << Cons::Error << "Framework attempting to create View for Project document" << ENDL;
      return theApp.GetMainWindow();
   }

   CDocument* ProjectDocTemplate::OpenDocumentFile(LPCTSTR szFullPath, BOOL bAddToMRU, BOOL bMakeVisible)
   {
	   ProjectDocument* pDocument = nullptr;
      bool Created = false;

      // already have a document - reinit it
	   if (pDocument = dynamic_cast<ProjectDocument*>(m_pOnlyDoc))
	   {
		   // Query to save 
		   if (!pDocument->SaveModified())
		      return nullptr; 
	   }
	   else 
      {
	      // create a new document
		   pDocument = dynamic_cast<ProjectDocument*>(CreateNewDocument());
         Created = true;
      }

      // Verify created
	   if (!pDocument)
	   {
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   return nullptr;
	   }
	   ASSERT(pDocument == m_pOnlyDoc);

      // New Document:
	   if (!szFullPath)
	   {
		   SetDefaultTitle(pDocument);

		   // avoid creating temporary compound file when starting up invisible
		   if (!bMakeVisible)
			   pDocument->m_bEmbedded = TRUE;

         // NEW DOCUMENT:
		   if (!pDocument->OnNewDocument())
		   {
			   Console << Cons::Error << "ProjectDocument::OnNewDocument returned FALSE";
			 
            // Cleanup
            if (Created)
            {
               delete pDocument;
               m_pOnlyDoc = nullptr;
            }
			   return nullptr;
		   }
	   }
	   else
	   {
		   CWaitCursor wait;

		   // Preserve 'Modified' flag
		   BOOL bWasModified = pDocument->IsModified();
		   pDocument->SetModifiedFlag(FALSE);  // not dirty for open

         // Upgrade:
         ImportProjectDialog dlg(szFullPath);
         if (dlg.DoModal() == IDOK)
         {
            // IMPORT DOCUMENT
            if (!pDocument->OnImportDocument(szFullPath, dlg.NewPath))
            {
               Console << Cons::Error << "ProjectDocument::OnImportDocument returned FALSE";
               return nullptr;
            }

            // Open upgraded project, not legacy
            szFullPath = dlg.NewPath.c_str();
            bAddToMRU = TRUE;
         }

         // OPEN DOCUMENT
		   if (!pDocument->OnOpenDocument(szFullPath))
		   {
			   Console << Cons::Error << "ProjectDocument::OnOpenDocument returned FALSE";

            // Failed: Destroy
			   if (Created)
			   {
               delete pDocument;
               m_pOnlyDoc = nullptr;
			   }
            // Failed (but Unmodified): Restore 'modified' flag
			   else if (!pDocument->IsModified())
			      pDocument->SetModifiedFlag(bWasModified);
			   else
			   {
				   // Failed (and Modified): Document is now corrupt. Start new document.
				   SetDefaultTitle(pDocument);

               // NEW DOCUMENT:
				   if (!pDocument->OnNewDocument())
				      Console << Cons::Error << "ProjectDocument::OnNewDocument failed after trying to open a document";
			   }
			   return nullptr;        // open failed
		   }
		   pDocument->SetPathName(szFullPath, bAddToMRU);
		   pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);
	   }

	   return pDocument;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ProjectDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ProjectDocument, DocumentBase)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectDocument::ProjectDocument() : DocumentBase(DocumentType::Project)
   {

   }

   ProjectDocument::~ProjectDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Gets the active project document, if any</summary>
   /// <returns></returns>
   ProjectDocument*  ProjectDocument::GetActive()
   {
      // Linear search: (Only one project can ever be open at a time)
      for (DocumentBase& doc : theApp)
         if (doc.GetType() == DocumentType::Project)
            return dynamic_cast<ProjectDocument*>(&doc);

      return nullptr;
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Adds a file item.</summary>
   /// <param name="path">Full path.</param>
   /// <param name="parent">parent item.</param>
   /// <returns>True if added, False if already existed</returns>
   /// <exception cref="Logic::ArgumentNullException">Parent is null</exception>
   bool ProjectDocument::AddFile(IO::Path path, ProjectFolderItem* parent)
   {
      REQUIRED(parent);

      // Ensure not already present
      if (Contains(path))
         return false;

      // Identify type
      FileType type = FileIdentifier::Identify(path);

      // Modify
      SetModifiedFlag(TRUE);

      // Raise 'ITEM ADDED'
      auto item = new ProjectFileItem(path, type);
      parent->Add(item);
      ItemAdded.Raise(item, parent);
      return true;
   }

   /// <summary>Adds a new folder.</summary>
   /// <param name="name">name.</param>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::ArgumentNullException">Parent is null</exception>
   void ProjectDocument::AddFolder(const wstring& name, ProjectFolderItem* parent)
   {
      REQUIRED(parent);

      // Modify
      SetModifiedFlag(TRUE);

      // Add new folder
      auto folder = new ProjectFolderItem(name, false);
      parent->Add(folder);

      // Raise 'ITEM ADDED'
      ItemAdded.Raise(folder, parent);
   }

   /// <summary>Check whether document contains a file</summary>
   /// <param name="path">The path.</param>
   /// <returns></returns>
   bool ProjectDocument::Contains(IO::Path path) const
   {
      // Search folders
      for (auto& folder : Project.Items)
         if (folder->Contains(path))
            return true;

      // Not found
      return false;
   }

   /// <summary>Moves an item to a new folder</summary>
   /// <param name="item">item.</param>
   /// <param name="folder">Destination.</param>
   /// <exception cref="Logic::ArgumentNullException">Item/folder is null</exception>
   void ProjectDocument::MoveItem(ProjectItem* item, ProjectFolderItem* folder)
   {
      REQUIRED(item);
      REQUIRED(folder);

      // Modify
      SetModifiedFlag(TRUE);

      // Remove item. Raise 'ITEM REMOVED'
      auto ptr = Project.Items.Remove(item);
      ItemRemoved.Raise(ptr.get(), nullptr);

      // Add to folder. Raise 'ITEM ADDED'
      folder->Add(ptr);
      ItemAdded.Raise(ptr.get(), folder);
   }

   /// <summary>Removes an item from the project</summary>
   /// <param name="item">item.</param>
   /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
   ProjectItemPtr ProjectDocument::RemoveItem(ProjectItem* item)
   {
      REQUIRED(item);

      // Modify
      SetModifiedFlag(TRUE);

      // Remove item. Raise 'ITEM REMOVED'
      auto ptr = Project.Items.Remove(item);
      ItemRemoved.Raise(ptr.get(), nullptr);

      return ptr;
   }

   /// <summary>Changes the project filename and title</summary>
   /// <param name="name">New name.</param>
   void  ProjectDocument::Rename(const wstring& name)
   {
      // Rename document/title
      DocumentBase::Rename(name);

      // Raise 'ITEM CHANGED'
      ItemChanged.Raise(nullptr, nullptr);
   }

   /// <summary>Renames a project item.</summary>
   /// <param name="item">The item.</param>
   /// <param name="name">New name.</param>
   /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
   /// <exception cref="Logic::Win32Exception">Unable to rename file</exception>
   void  ProjectDocument::RenameItem(ProjectItem* item, const wstring& name)
   {
      REQUIRED(item);

      // Modify
      SetModifiedFlag(TRUE);

      // Update name
      item->Name = name;

      // File: Rename file/document
      if (auto file = dynamic_cast<ProjectFileItem*>(item))
      {
         // Open: Rename document
         if (auto doc = theApp.GetOpenDocument(file->FullPath))
            doc->Rename(name);
         else
         {
            // New file path
            IO::Path newPath = file->FullPath.RenameFileName(name);
            
            // Closed: Rename file on disc
            if (newPath.Exists() || !MoveFile(file->FullPath.c_str(), newPath.c_str()))
               throw Win32Exception(HERE, GuiString(L"Unable to rename '%s' to '%s'", file->FullPath.FileName.c_str(), newPath.FileName.c_str()));

            // Set new path
            file->FullPath = newPath;
         }
      }

      // Raise 'ITEM CHANGED'
      ItemChanged.Raise(item, nullptr);
   }

   void ProjectDocument::OnDocumentEvent(DocumentEvent deEvent) 
   {
      // Raise 'PROJECT LOADED'
      if (deEvent == CDocument::onAfterOpenDocument)
         Loaded.Raise();
   }

   /// <summary>Called on new document.</summary>
   /// <returns></returns>
   BOOL ProjectDocument::OnNewDocument()
   {
      // Raise 'PROJECT LOADED'
      Loaded.Raise();

      return DocumentBase::OnNewDocument();
   }
   

   /// <summary>Called on open document.</summary>
   /// <param name="legacy">Legacy project path.</param>
   /// <param name="upgrade">New project path.</param>
   /// <returns></returns>
   BOOL ProjectDocument::OnImportDocument(IO::Path legacy, IO::Path upgrade)
   {
      try
      {
         // Read legacy
         auto fs = StreamPtr(new FileStream(legacy, FileMode::OpenExisting, FileAccess::Read));
         Project = LegacyProjectFileReader(fs).ReadFile(legacy);
         
         // Success: 
         return TRUE;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         return FALSE;
      }
   }


   /// <summary>Called on open document.</summary>
   /// <param name="szPath">The path.</param>
   /// <returns></returns>
   BOOL ProjectDocument::OnOpenDocument(LPCTSTR szPath)
   {
      try
      {
         // Read file
         auto fs = StreamPtr(new FileStream(szPath, FileMode::OpenExisting, FileAccess::Read));
         Project = LegacyProjectFileReader(fs).ReadFile(szPath);
         
         // Success: 
         return TRUE;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         return FALSE;
      }
   }


   BOOL ProjectDocument::OnSaveDocument(LPCTSTR lpszPathName)
   {
      // TODO: Add your specialized code here and/or call the base class

      return DocumentBase::OnSaveDocument(lpszPathName);
   }

   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

