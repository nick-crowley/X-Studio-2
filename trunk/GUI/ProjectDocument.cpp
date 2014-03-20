#include "stdafx.h"
#include "ProjectDocument.h"
#include "ProgressDialog.h"
#include "ScriptDocument.h"
#include "MainWnd.h"
#include "../Logic/FileIdentifier.h"
#include "../Logic/BackupFileReader.h"
#include "../Logic/ProjectFileReader.h"
#include "../Logic/ProjectFileWriter.h"
#include "../Logic/XFileInfo.h"
#include "../Logic/ImportProjectWorker.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary>Project has been loaded/unloaded</summary>
   SimpleEvent   ProjectDocument::Loaded,
                 ProjectDocument::Closed;

   /// <summary>Project item has been added</summary>
   ProjectItemAddedEvent  ProjectDocument::ItemAdded;

   /// <summary>Project item has been changed or removed</summary>
   ProjectItemEvent       ProjectDocument::ItemChanged,
                          ProjectDocument::ItemRemoved;

   /// <summary>Project item backups has been added/removed</summary>
   ProjectItemEvent       ProjectDocument::BackupChanged;
   

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ProjectDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ProjectDocument, DocumentBase)
      ON_COMMAND(ID_FILE_PROJECT_CLOSE, &ProjectDocument::OnCommandCloseProject)
      ON_COMMAND(ID_FILE_PROJECT_SAVE, &ProjectDocument::OnCommandSaveProject)
      ON_COMMAND(ID_FILE_PROJECT_SAVE_AS, &ProjectDocument::OnCommandSaveProjectAs)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectDocument::ProjectDocument() : DocumentBase(DocumentType::Project, false), Project(L"Untitled"), IsClosing(false)
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
            if (auto proj = dynamic_cast<ProjectDocument*>(&doc))
               // Don't return project if closing down
               return !proj->IsClosing ? proj : nullptr;

      return nullptr;
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Adds a file item to the appropriate base folder, based on file type</summary>
   /// <param name="path">Full path.</param>
   /// <returns>True if added, False if already existed</returns>
   bool ProjectDocument::AddFile(Path path)
   {
      // Add to appropriate root folder
      if (auto folder = Project.FindFolder(FileIdentifier::Identify(path)))
         return AddFile(path, *folder);

      // Error: Unable to base folder
      return false;
   }

   /// <summary>Adds a file item.</summary>
   /// <param name="path">Full path.</param>
   /// <param name="folder">parent item.</param>
   /// <returns>True if added, False if already existed</returns>
   bool ProjectDocument::AddFile(Path path, ProjectItem& folder)
   {
      // Ensure not already present
      if (Contains(path))
         return false;

      // Create item
      auto type = FileIdentifier::Identify(path);
      ProjectItem item(type, path, L"");

      // Script: Initial commit
      if (type == FileType::Script)
      {
         item.SetBackupPath(FullPath.Folder);
         item.InitialCommit(FullPath.Folder);
      }

      // Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(item), &folder);

      // Modify project
      SetModifiedFlag(TRUE);
      return true;
   }

   /// <summary>Adds a new folder.</summary>
   /// <param name="name">name.</param>
   /// <param name="parent">parent folder.</param>
   void ProjectDocument::AddFolder(const wstring& name, ProjectItem& folder)
   {
      // Modify project
      SetModifiedFlag(TRUE);

      // Add new folder
      ProjectItem item(name, false);

      // Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(item), &folder);
   }

   /// <summary>Check whether project contains a file</summary>
   /// <param name="path">The path.</param>
   /// <returns></returns>
   bool ProjectDocument::Contains(Path path) const
   {
      return Project.Contains(path);
   }
   
   /// <summary>Check whether project contains a document</summary>
   /// <param name="doc">Document.</param>
   /// <returns></returns>
   bool ProjectDocument::Contains(const DocumentBase& doc) const
   {
      return Project.Contains(doc.FullPath);
   }

   /// <summary>Performs a commit on the specified document.</summary>
   /// <param name="doc">The document.</param>
   /// <param name="title">The title.</param>
   /// <returns>True if commited, false if aborted</returns>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   bool ProjectDocument::Commit(const ScriptDocument& doc, const wstring& title)
   {
      // Verify member of project
      if (!Contains(doc))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // Backup File Missing: 
      if (!HasBackup(doc))
      {  
         auto item = Project.Find(doc.FullPath);

         // Query: Re-create backup?
         VString msg(L"The backup file '%s' is missing, would you like to create a new one?", item->BackupName.c_str());
         if (theApp.ShowMessage(msg, MB_YESNO|MB_ICONQUESTION) == IDNO)
            return false;

         // Perform initial commit
         item->SetBackupPath(FullPath.Folder);
         item->InitialCommit(FullPath.Folder);
      }
      
      // Get script content 
      wstring content = doc.GetAllText();
      for (auto& ch : content)   // Convert RichEdit '\v' lineBreaks to '\n'
         if (ch == '\v')
            ch = '\n';

      // Load backup. Insert revision
      auto backup = LoadBackupFile(doc);
      backup.Revisions.Commit( ScriptRevision(title, doc.FullPath, content, doc.Script) );

      // Overwrite backup file
      backup.Write(GetBackupPath(doc));

      // Raise 'BACKUP CHANGED'
      RefreshRevisions(doc);
      return true;
   }

   /// <summary>Removes a revision from a document's backup file.</summary>
   /// <param name="doc">The document.</param>
   /// <param name="index">Zero-based index.</param>
   /// <exception cref="Logic::ArgumentException">Document not member of project</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   void ProjectDocument::DeleteRevision(const ScriptDocument& doc, UINT index)
   {
      // Verify member of project
      if (!Contains(doc))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // Load backup. Delete revision
      auto backup = LoadBackupFile(doc);
      backup.Remove(index);

      // Overwrite backup file
      backup.Write(GetBackupPath(doc));

      // Raise 'BACKUP CHANGED'
      RefreshRevisions(doc);
   }
   
   /// <summary>Get full path of backup file for a document.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
   Path  ProjectDocument::GetBackupPath(const ScriptDocument& doc) const
   {
      // Verify member of project
      if (!Contains(doc))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // ProjectFolder + BackupName
      return FullPath.Folder + Project.Find(doc.FullPath)->BackupName;
   }

   /// <summary>Query whether backup file for a document exists.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Document not member of project</exception>
   bool ProjectDocument::HasBackup(const ScriptDocument& doc) const
   {
      // Verify member of project
      if (!Contains(doc))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // Query existence
      return GetBackupPath(doc).Exists();
   }

   /// <summary>Get backup file for a document.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Document not member of project</exception>
   /// <exception cref="Logic::ArgumentNullException">Invalid file format</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
   /// <exception cref="Logic::InvalidValueException">Invalid file format</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   BackupFile  ProjectDocument::LoadBackupFile(const ScriptDocument& doc) const
   {
      // Open backup file
      auto s = XFileInfo(GetBackupPath(doc)).OpenRead();

      // Read contents
      return BackupFileReader(s).ReadFile();
   }

   /// <summary>Moves an item to a new folder</summary>
   /// <param name="item">item.</param>
   /// <param name="folder">Destination.</param>
   void ProjectDocument::MoveItem(ProjectItem& item, ProjectItem& folder)
   {
      auto copy = ProjectItem(item);

      // Remove item. Raise 'ITEM REMOVED'
      Project.Remove(item);
      ItemRemoved.Raise(&item);

      // Modify project
      SetModifiedFlag(TRUE);

      // Add to folder. Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(copy), &folder);
   }
   
   /// <summary>Raises 'PROJECT CLOSED/LOADED' after loading/closing</summary>
   /// <param name="deEvent">The event.</param>
   void ProjectDocument::OnDocumentEvent(DocumentEvent deEvent)
   {
      __super::OnDocumentEvent(deEvent);

      // Raise 'PROJECT LOADED'
      if (deEvent == CDocument::onAfterOpenDocument)
         Loaded.Raise();

      // Raise 'PROJECT CLOSED'
      else if (deEvent == CDocument::onAfterCloseDocument)
         Closed.Raise();
   }
   
   /// <summary>Called on new X-Studio 2 project.</summary>
   /// <returns></returns>
   BOOL ProjectDocument::OnNewDocument()
   {
      // TODO: Clear existing data

      // Raise 'PROJECT LOADED'
      Loaded.Raise();

      return DocumentBase::OnNewDocument();
   }


   /// <summary>Called on convert an X-Studio 1 project to X-Studio 2.</summary>
   /// <param name="legacy">Legacy project path.</param>
   /// <param name="upgrade">New project path.</param>
   /// <returns>TRUE if successfully upgraded, otherwise FALSE</returns>
   BOOL ProjectDocument::OnImportDocument(Path legacy, Path upgrade)
   {
      ImportProjectWorker w;
      ProgressDialog dlg(L"Import Project", L"Importing files...", false);
      BOOL result;
      
      // Display compiler output window
      theApp.GetMainWindow()->ActivateOutputPane(Operation::ImportProject, true);

      // Import project
      w.Start(legacy, upgrade);

      // Display progress dialog until complete
      dlg.DoModal(&w);
      result = w.GetExitCode();
      w.Close();

      // Feedback
      if (result)
         theApp.ShowMessage(L"Project Imported Successfully", MB_OK|MB_ICONINFORMATION);
      else
         theApp.ShowMessage(L"Failed to import project", MB_OK|MB_ICONERROR);

      // Return result
      return result;
   }

   /// <summary>Called on open an X-Studio 2 project.</summary>
   /// <param name="szPath">The path.</param>
   /// <returns></returns>
   BOOL ProjectDocument::OnOpenDocument(LPCTSTR szPath)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << "Loading project: " << Path(szPath) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Loading project '%s'", szPath));

         // Read file
         auto fs = StreamPtr(new FileStream(szPath, FileMode::OpenExisting, FileAccess::Read));
         Project = ProjectFileReader(fs).ReadFile(szPath);
         
         // Activate project window
         theApp.GetMainWindow()->ActivateProjectPane();

         // Success: Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Project loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase& e) 
      {
         // Feedback/Display error
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to load project");
         theApp.ShowError(HERE, e, VString(L"Failed to load project '%s'", szPath));
         return FALSE;
      }
   }

   /// <summary>Called to save document.</summary>
   /// <param name="szPath">The new/existing path.</param>
   /// <returns></returns>
   BOOL ProjectDocument::OnSaveDocument(LPCTSTR szPath)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      try
      {
         REQUIRED(szPath);

         // Feedback
         Console << Cons::UserAction << L"Saving project: " << FullPath << " as " << Path(szPath) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Saving project '%s'", szPath));
      
         // Write project
         ProjectFileWriter w(XFileInfo(szPath).OpenWrite());
         w.Write(Project);
         w.Close();

         // Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Project saved successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save project");
         theApp.ShowError(HERE, e, VString(L"Failed to save project '%s'", szPath));
         return FALSE;
      }
   }

   /// <summary>Refreshes the backup window for the current document.</summary>
   /// <param name="doc">The document.</param>
   void  ProjectDocument::RefreshRevisions(const DocumentBase& doc)
   {
      /*if (Contains(doc))
         BackupChanged.Raise(Project.Find(doc.FullPath));*/

      BackupChanged.Raise(nullptr);
   }
   
   /// <summary>Removes an item from the project</summary>
   /// <param name="item">item.</param>
   void ProjectDocument::RemoveItem(ProjectItem& item)
   {
      // Remove item. Raise 'ITEM REMOVED'
      Project.Remove(item);
      ItemRemoved.Raise(&item);

      // Modify project
      SetModifiedFlag(TRUE);
   }

   /// <summary>Renames a project item.</summary>
   /// <param name="item">The item.</param>
   /// <param name="name">New name (or filename).</param>
   /// <exception cref="Logic::ApplicationException">New file path already exists -or- project already contains new path</exception>
   /// <exception cref="Logic::IOException">Unable to rename file</exception>
   void  ProjectDocument::RenameItem(ProjectItem& item, const wstring& name)
   {
      // Project: Rename document/root, but not file
      if (item.IsRoot())
      {
         SetModifiedFlag(TRUE); // Mark modified before rename so file is not renamed
         Rename(FullPath.RenameFileName(name));
      }
      // File: Rename file/document
      else if (item.IsFile())
      {
         // Document: Rename document/item, but not file
         if (auto doc = theApp.GetOpenDocument(item.FullPath))
            doc->Rename(item.FullPath.RenameFileName(name));
         else
         {
            // File: Attempt to Rename on disc
            Path newPath = item.FullPath.RenameFileName(name);
            RenameFile(item.FullPath, newPath);

            // Set new name+path
            item.Name = name;
            item.FullPath = newPath;
         }
      }
      // Variable: Update name 
      else
         item.Name = name;

      // Modify project
      SetModifiedFlag(TRUE);

      // Raise 'ITEM CHANGED'
      ItemChanged.Raise(&item);
   }
   
   /// <summary>Changes the path and updates the root item to reflect new path/title.</summary>
   /// <param name="path">The path.</param>
   void  ProjectDocument::SetFullPath(Path path)
   {
      __super::SetFullPath(path);

      // Set new name+path
      Project.Root.Name = GetTitle();
      Project.Root.FullPath = path;

      // Raise 'ROOT CHANGED'
      ItemChanged.Raise(&Project.Root);
   }

   /// <summary>Refreshes the root in response to changing the modified flag.</summary>
   /// <param name="bModified">modified.</param>
   void  ProjectDocument::SetModifiedFlag(BOOL bModified)
   {
      __super::SetModifiedFlag(bModified);

      // Set new name
      Project.Root.Name = GetTitle();

      // Raise 'ROOT CHANGED'
      ItemChanged.Raise(&Project.Root);
   }
   
   /// <summary>Updates a document's project item to match current path/title</summary>
   /// <param name="doc">document.</param>
   void  ProjectDocument::UpdateItem(const DocumentBase& doc)
   {
      auto item = Project.Find(doc.FullPath);

      // Ensure document is member
      if (item == nullptr)
         return;
      
      // Update name + path
      item->Name = doc.GetTitle();
      item->FullPath = doc.FullPath;

      // Item: Raise 'ITEM CHANGED'
      ItemChanged.Raise(item);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Perform commands.</summary>
   void ProjectDocument::OnPerformCommand(UINT nID)
   {
      try
      {
         switch (nID)
         {
         // Close Project
         case ID_FILE_PROJECT_CLOSE:
            // Save Modified
            if (!SaveModified())
		         return;

            // Close 
            IsClosing = true;    // Closing flag prevents ProjectDoc::GetActive() returning document during events fired during closing
	         OnCloseDocument();
            break;

         // Save Project
         case ID_FILE_PROJECT_SAVE:       OnCommand_Save();       break;
         case ID_FILE_PROJECT_SAVE_AS:    OnCommand_SaveAs();     break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void  ProjectDocument::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      switch (pCmdUI->m_nID)
      {
      // Close/Save/SaveAs: Always enabled
      case ID_FILE_PROJECT_SAVE:
      case ID_FILE_PROJECT_CLOSE: 
      case ID_FILE_PROJECT_SAVE_AS: 
         state = true;
         break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

