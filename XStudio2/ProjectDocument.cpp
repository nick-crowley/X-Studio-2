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
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary>Project has been loaded/unloaded</summary>
   SimpleEvent   ProjectDocument::Loaded;

   /// <summary>Project item has been added, changed or removed</summary>
   ProjectItemEvent  ProjectDocument::ItemAdded,
                     ProjectDocument::ItemChanged,
                     ProjectDocument::ItemRemoved;

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ProjectDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ProjectDocument, DocumentBase)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectDocument::ProjectDocument() : DocumentBase(DocumentType::Project, false)
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
      auto item = ProjectItem(FileIdentifier::Identify(path), path, L"");

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
      auto item = ProjectItem(name, false);

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

   /// <summary>Get backup file for a document.</summary>
   /// <param name="doc">The document.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ProjectItemNotFoundException">Document not part of project</exception>
   /// <exception cref="Logic::ArgumentNullException">Invalid file format</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
   /// <exception cref="Logic::InvalidValueException">Invalid file format</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   BackupFile  ProjectDocument::GetAllRevisions(DocumentBase* doc) const
   {
      // Lookup project item
      auto file = Project.Find(doc->FullPath);

      // DEBUG:
      Console << "Opening backup file: " << file->BackupPath << ENDL;

      // Open backup file
      return BackupFileReader(XFileInfo(file->BackupPath).OpenRead()).ReadFile();
   }

   /// <summary>Moves an item to a new folder</summary>
   /// <param name="item">item.</param>
   /// <param name="folder">Destination.</param>
   void ProjectDocument::MoveItem(ProjectItem& item, ProjectItem& folder)
   {
      auto copy = ProjectItem(item);

      // Remove item. Raise 'ITEM REMOVED'
      Project.Remove(&item);
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
      Project.Remove(&item);
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

   void ProjectDocument::OnDocumentEvent(DocumentEvent deEvent) 
   {
      // Raise 'PROJECT LOADED'
      if (deEvent == CDocument::onAfterOpenDocument)
         Loaded.Raise();
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
         // Read legacy project
         auto in = StreamPtr(new FileStream(legacy, FileMode::OpenExisting, FileAccess::Read));
         auto proj = LegacyProjectFileReader(in).ReadFile(legacy);

         // Perform initial commits of file items
         for (auto& item : proj.ToList())
            if (item->IsFile() && item->FileType == FileType::Script)
            {
               // Generate unique filename + Commit
               item->SetBackupPath(upgrade.Folder);
               InitialCommit(*item);
            }

         // Write X-Studio2 project
         auto out = StreamPtr(new FileStream(upgrade, FileMode::CreateNew, FileAccess::Write));
         ProjectFileWriter w(out);
         w.Write(proj);
         w.Close();
         
         // Success: 
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
      try
      {
         // Read file
         auto fs = StreamPtr(new FileStream(szPath, FileMode::OpenExisting, FileAccess::Read));
         Project = ProjectFileReader(fs).ReadFile(szPath);
         
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
   
   /// <summary>Creates a backup file for a file item and performs an initial commit.</summary>
   /// <param name="item">File item.</param>
   /// <exception cref="Logic::ArgumentException">Backup path not set</exception>
   /// <exception cref="Logic::ArgumentNullException">Script format invalid</exception>
   /// <exception cref="Logic::ComException">COM Error</exception>
   /// <exception cref="Logic::FileFormatException">Script format invalid</exception>
   /// <exception cref="Logic::InvalidValueException">Script format invalid</exception>
   /// <exception cref="Logic::InvalidOperationException">Script format invalid</exception>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   void ProjectDocument::InitialCommit(const ProjectItem& item)
   {
      try
      {
         BackupFile backup(BackupType::MSCI);

         // Ensure backup path has been set
         if (item.BackupName.empty())
            throw ArgumentException(HERE, L"item", GuiString(L"Missing backup file path for '%s'", item.Name.c_str()) );

         // Read script from disc
         auto script = ScriptFileReader(XFileInfo(item.FullPath).OpenRead()).ReadFile(item.FullPath, false);
         
         // Create script revision
         ScriptRevision rev(L"Initial Commit", item.FullPath, script.GetAllText(), script);
         backup.Revisions.Add(rev);

         // Save backup
         BackupFileWriter w(XFileInfo(FullPath.Folder+item.BackupName).OpenWrite());
         w.WriteFile(backup);
         w.Close();
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, GuiString(L"Unable to perform initial commit of '%s'", item.Name.c_str()));
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

