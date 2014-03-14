#include "stdafx.h"
#include "ProjectDocument.h"
#include "ImportProjectDialog.h"
#include "Logic/FileIdentifier.h"
#include "Logic/BackupFileReader.h"
#include "Logic/BackupFileWriter.h"
#include "Logic/LegacyProjectFileReader.h"
#include "Logic/ProjectFileReader.h"
#include "Logic/ProjectFileWriter.h"
#include "Logic/XFileInfo.h"
#include "Logic/ScriptFileReader.h"
#include "ScriptDocument.h"
#include "MainWnd.h"

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

   ProjectDocument::ProjectDocument() : DocumentBase(DocumentType::Project, false), Project(L"Untitled")
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
   /// <param name="folder">parent item.</param>
   /// <returns>True if added, False if already existed</returns>
   bool ProjectDocument::AddFile(IO::Path path, ProjectItem& folder)
   {
      // Ensure not already present
      if (Contains(path))
         return false;

      // Create item
      ProjectItem item(FileIdentifier::Identify(path), path, L"");

      // Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(item), &folder);

      // Modify document
      SetModifiedFlag(TRUE);
      return true;
   }

   /// <summary>Adds a new folder.</summary>
   /// <param name="name">name.</param>
   /// <param name="parent">parent folder.</param>
   void ProjectDocument::AddFolder(const wstring& name, ProjectItem& folder)
   {
      // Modify
      SetModifiedFlag(TRUE);

      // Add new folder
      ProjectItem item(name, false);

      // Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(item), &folder);
   }

   /// <summary>Check whether document contains a file</summary>
   /// <param name="path">The path.</param>
   /// <returns></returns>
   bool ProjectDocument::Contains(IO::Path path) const
   {
      return Project.Contains(path);
   }

   /// <summary>Performs a commit on the specified document.</summary>
   /// <param name="doc">The document.</param>
   /// <param name="title">The title.</param>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   void ProjectDocument::Commit(const ScriptDocument& doc, const wstring& title)
   {
      // Verify member of project
      if (!Contains(doc.FullPath))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // Convert RichEdit '\v' lineBreaks to '\n'
      wstring content = doc.GetAllText();
      for (auto& ch : content)
         if (ch == '\v')
            ch = '\n';

      // Load backup. Insert revision
      auto backup = LoadBackupFile(doc);
      backup.Revisions.Commit( ScriptRevision(title, doc.FullPath, content, doc.Script) );

      // Generate backup path
      auto path = FullPath.Folder + Project.Find(doc.FullPath)->BackupName;

      // Overwrite backup file
      BackupFileWriter w(XFileInfo(path).OpenWrite(L"revisions.xml"));
      w.WriteFile(backup);
      w.Close();

      // Raise 'BACKUP CHANGED'
      BackupChanged.Raise(Project.Find(doc.FullPath));
   }

   /// <summary>Removes a revision from a document's backup file.</summary>
   /// <param name="doc">The document.</param>
   /// <param name="index">Zero-based index.</param>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   void ProjectDocument::DeleteRevision(const ScriptDocument& doc, UINT index)
   {
      // Verify member of project
      if (!Contains(doc.FullPath))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // Load backup. Delete revision
      auto backup = LoadBackupFile(doc);
      backup.Remove(index);

      // Generate backup path
      auto path = FullPath.Folder + Project.Find(doc.FullPath)->BackupName;

      // Overwrite backup file
      BackupFileWriter w(XFileInfo(path).OpenWrite(L"revisions.xml"));
      w.WriteFile(backup);
      w.Close();

      // Raise 'BACKUP CHANGED'
      BackupChanged.Raise(Project.Find(doc.FullPath));
   }

   /// <summary>Get backup file for a document.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
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

      // Modify document
      SetModifiedFlag(TRUE);

      // Add to folder. Raise 'ITEM ADDED'
      ItemAdded.Raise(&folder.Add(copy), &folder);
   }

   /// <summary>Removes an item from the project</summary>
   /// <param name="item">item.</param>
   void ProjectDocument::RemoveItem(ProjectItem& item)
   {
      // Remove item. Raise 'ITEM REMOVED'
      Project.Remove(item);
      ItemRemoved.Raise(&item);

      // Modify document
      SetModifiedFlag(TRUE);
   }

   /// <summary>Changes the project filename and title</summary>
   /// <param name="name">New name.</param>
   void  ProjectDocument::Rename(const wstring& name)
   {
      // Rename document/title
      DocumentBase::Rename(name);

      // Raise 'ITEM CHANGED'
      ItemChanged.Raise(nullptr);
   }

   /// <summary>Renames a project item.</summary>
   /// <param name="item">The item.</param>
   /// <param name="name">New name.</param>
   /// <exception cref="Logic::Win32Exception">Unable to rename file</exception>
   void  ProjectDocument::RenameItem(ProjectItem& item, const wstring& name)
   {
      // Update name
      item.Name = name;

      // File: Rename file/document
      if (item.IsFile())
      {
         // Open: Rename document
         if (auto doc = theApp.GetOpenDocument(item.FullPath))
            doc->Rename(name);
         else
         {
            // New file path
            IO::Path newPath = item.FullPath.RenameFileName(name);
            
            // Closed: Rename file on disc
            if (newPath.Exists() || !MoveFile(item.FullPath.c_str(), newPath.c_str()))
               throw Win32Exception(HERE, GuiString(L"Unable to rename '%s' to '%s'", item.FullPath.FileName.c_str(), newPath.FileName.c_str()));

            // Set new path
            item.FullPath = newPath;
         }
      }

      // Modify document
      SetModifiedFlag(TRUE);

      // Raise 'ITEM CHANGED'
      ItemChanged.Raise(&item);
   }

   /// <summary>Raises 'PROJECT CLOSED/LOADED' after loading/closing</summary>
   /// <param name="deEvent">The event.</param>
   void ProjectDocument::OnDocumentEvent(DocumentEvent deEvent)
   {
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
   BOOL ProjectDocument::OnImportDocument(IO::Path legacy, IO::Path upgrade)
   {
      try
      {
         // Feedback
         Console << Cons::UserAction << "Importing legacy project " << legacy << " into " << upgrade << ENDL;

         // Read legacy project
         auto in = StreamPtr(new FileStream(legacy, FileMode::OpenExisting, FileAccess::Read));
         auto proj = LegacyProjectFileReader(in).ReadFile(legacy);

         // Perform initial commits of file items
         for (auto& item : proj.ToList())
            if (item->IsFile() && item->FileType == FileType::Script)
            {
               // Generate unique filename + Commit
               item->SetBackupPath(upgrade.Folder);
               InitialCommit(upgrade.Folder, *item);
            }

         // Write X-Studio2 project
         auto out = StreamPtr(new FileStream(upgrade, FileMode::CreateNew, FileAccess::Write));
         ProjectFileWriter w(out);
         w.Write(proj);
         w.Close();
         
         // Success: Feedback
         Console << Cons::UserAction << "Importing completed successfully" << ENDL;
         theApp.ShowMessage(L"Project Imported Successfully", MB_OK);
         return TRUE;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         return FALSE;
      }
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
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading project '%s'", szPath));

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
         theApp.ShowError(HERE, e, GuiString(L"Failed to load project '%s'", szPath));
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
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Saving project '%s'", szPath));
      
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
         theApp.ShowError(HERE, e, GuiString(L"Failed to save project '%s'", szPath));
         return FALSE;
      }
   }

   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Get full path of backup file for a document.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Document not part of project</exception>
   IO::Path  ProjectDocument::GetBackupPath(const ScriptDocument& doc) const
   {
      // Verify member of project
      if (!Contains(doc.FullPath))
         throw ArgumentException(HERE, L"doc", L"Document is not part of project");

      // ProjectFolder + BackupName
      return FullPath.Folder + Project.Find(doc.FullPath)->BackupName;
   }

   /// <summary>Creates a backup file for a file item and performs an initial commit.</summary>
   /// <param name="folder">Backup folder.</param>
   /// <param name="item">File item.</param>
   /// <exception cref="Logic::ArgumentException">Backup path not set</exception>
   /// <exception cref="Logic::ArgumentNullException">Script format invalid</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::FileFormatException">Script format invalid</exception>
   /// <exception cref="Logic::InvalidValueException">Script format invalid</exception>
   /// <exception cref="Logic::InvalidOperationException">Script format invalid</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   void ProjectDocument::InitialCommit(const IO::Path& folder, const ProjectItem& item)
   {
      try
      {
         // Feedback
         Console << Cons::Heading << "Performing Initial Commit: " << item.FullPath << ENDL;

         // Ensure backup path has been set
         if (item.BackupName.empty())
            throw ArgumentException(HERE, L"item", GuiString(L"Missing backup file path for '%s'", item.Name.c_str()) );

         // Read script from disc
         auto script = ScriptFileReader(XFileInfo(item.FullPath).OpenRead()).ReadFile(item.FullPath, false);
         
         // Create script revision
         BackupFile backup(BackupType::MSCI);
         backup.Revisions.Commit( ScriptRevision(L"Initial Commit", item.FullPath, script.GetAllText(), script) );

         // Save backup
         SaveBackupFile(folder+item.BackupName, backup);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, GuiString(L"Unable to perform initial commit of '%s'", item.Name.c_str()));
      }
   }
   
   
   /// <summary>Perform commands.</summary>
   void ProjectDocument::OnPerformCommand(UINT nID)
   {
      try
      {
         switch (nID)
         {
         // Close Project
         case ID_FILE_PROJECT_CLOSE:
            if (!SaveModified())
		         return;

	         OnCloseDocument();
            break;

         // Save Project
         case ID_FILE_PROJECT_SAVE:
            DoFileSave();
            break;

         // Save Project As
         case ID_FILE_PROJECT_SAVE_AS:
            DoSave(NULL);
            break;
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
   
   
   /// <summary>Saves the backup file.</summary>
   /// <param name="path">The path.</param>
   /// <param name="f">The f.</param>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::GZipException">Unable to inititalise stream</exception>
   /// <exception cref="Logic::IOException">Unable to create file</exception>
   void ProjectDocument::SaveBackupFile(const IO::Path& path, const BackupFile& f) const
   {
      TempPath tmp;

      // Write to a temp file to prevent destroying all revisions if case of failure
      BackupFileWriter w(XFileInfo(tmp).OpenWrite(L"revisions.xml"));
      w.WriteFile(f);
      w.Close();

      // Success: Copy file
      if (!MoveFileEx(tmp.c_str(), path.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
         throw IOException(HERE, L"Unable to overwrite backup file: " + SysErrorString());
   }

   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

