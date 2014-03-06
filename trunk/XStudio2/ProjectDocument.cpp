#include "stdafx.h"
#include "ProjectDocument.h"
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
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence  ProjectDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CSingleDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(lpszPathName) == FileType::Project ? yesAttemptNative : noAttempt;
   }

   CFrameWnd*  ProjectDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) 
   {
      Console << Cons::Error << "Framework attempting to create View for Project document" << ENDL;
      return theApp.GetMainWindow();
   }

   CDocument* ProjectDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible)
   {
	   CDocument* pDocument = NULL;
	   BOOL bCreated = FALSE;      // => doc and frame created
	   BOOL bWasModified = FALSE;

	   if (m_pOnlyDoc != NULL)
	   {
		   // already have a document - reinit it
		   pDocument = m_pOnlyDoc;
		   if (!pDocument->SaveModified())
		      return NULL;        // leave the original one
	   }
	   else
	   {
		   // create a new document
		   pDocument = CreateNewDocument();
		   bCreated = TRUE;
	   }

	   if (pDocument == NULL)
	   {
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   return NULL;
	   }
	   ASSERT(pDocument == m_pOnlyDoc);

      // create a new document
	   if (lpszPathName == NULL)
	   {
		   SetDefaultTitle(pDocument);

		   // avoid creating temporary compound file when starting up invisible
		   if (!bMakeVisible)
			   pDocument->m_bEmbedded = TRUE;

		   if (!pDocument->OnNewDocument())
		   {
			   // user has been alerted to what failed in OnNewDocument
			   TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
			   if (bCreated)
            {
               delete pDocument;
               m_pOnlyDoc = NULL;
            }
				
			   return NULL;
		   }
	   }
	   else
	   {
		   CWaitCursor wait;

		   // open an existing document
		   bWasModified = pDocument->IsModified();
		   pDocument->SetModifiedFlag(FALSE);  // not dirty for open

		   if (!pDocument->OnOpenDocument(lpszPathName))
		   {
			   // user has been alerted to what failed in OnOpenDocument
			   TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
			   if (bCreated)
			   {
               delete pDocument;
               m_pOnlyDoc = NULL;
			   }
			   else if (!pDocument->IsModified())
			   {
				   // original document is untouched
				   pDocument->SetModifiedFlag(bWasModified);
			   }
			   else
			   {
				   // we corrupted the original document
				   SetDefaultTitle(pDocument);

				   if (!pDocument->OnNewDocument())
				   {
					   TRACE(traceAppMsg, 0, "Error: OnNewDocument failed after trying to open a document - trying to continue.\n");
					   // assume we can continue
				   }
			   }
			   return NULL;        // open failed
		   }
		   pDocument->SetPathName(lpszPathName, bAddToMRU);
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

   BOOL ProjectDocument::OnNewDocument()
   {
      // Raise 'PROJECT LOADED'
      Loaded.Raise();

      return DocumentBase::OnNewDocument();
   }


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

