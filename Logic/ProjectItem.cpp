#include "stdafx.h"
#include "ProjectItem.h"
#include "BackupFile.h"
#include "ScriptFileReader.h"
#include "XFileInfo.h"

namespace Logic
{
   namespace Projects
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create root</summary>
      /// <param name="name">project name.</param>
      ProjectItem::ProjectItem(const wstring& name) 
         : Type(ProjectItemType::Root), Name(name), Fixed(true), FileType(FileType::Unknown), Value(-1)
      {}

      /// <summary>Create file item</summary>
      /// <param name="file">file type.</param>
      /// <param name="path">Full path.</param>
      /// <param name="backup">backup name.</param>
      ProjectItem::ProjectItem(Logic::FileType file, Path path, Path backup)
         : Type(ProjectItemType::File), FileType(file), FullPath(path), BackupName(backup.c_str()), Name(path.FileName), Fixed(false), Value(-1)
      {}

      /// <summary>Create variable item</summary>
      /// <param name="name">variable name.</param>
      /// <param name="value">value.</param>
      ProjectItem::ProjectItem(const wstring& name, int value)
         : Type(ProjectItemType::Variable), Name(name), Value(value), Fixed(false), FileType(FileType::Unknown)
      {}

      /// <summary>Create folder item</summary>
      /// <param name="name">folder name.</param>
      /// <param name="fixed">whether fixed</param>
      ProjectItem::ProjectItem(const wstring& name, bool fixed)
         : Type(ProjectItemType::Folder), Name(name), Fixed(fixed), FileType(FileType::Unknown), Value(-1)
      {}

      ProjectItem::~ProjectItem()
      {}

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Append copy of item as child</summary>
      /// <param name="p">item</param>
      /// <returns>Reference to inserted child</returns>
      ProjectItem&  ProjectItem::Add(const ProjectItem& p)
      {
         Children.push_back(p);
         return Children.back();
      }

      /// <summary>Query whether any child contains a given path</summary>
      /// <param name="path">The path.</param>
      /// <returns></returns>
      bool  ProjectItem::Contains(Path path) const
      {
         if (FullPath == path)
            return true;

         return any_of(Children.begin(), Children.end(), [&path](const ProjectItem& p) {return p.Contains(path);} );
      }

      /// <summary>Finds item by path.</summary>
      /// <param name="path">Full path.</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      ProjectItem*  ProjectItem::Find(Path path) const
      {
         // Check self
         if (FullPath == path)
            return const_cast<ProjectItem*>(this);

         // Search children
         for (auto& c : Children)
            if (auto it = c.Find(path))
               return it;

         return nullptr;
      }
         
      /// <summary>Finds item by name and type.</summary>
      /// <param name="type">Type.</param>
      /// <param name="name">Name.</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      ProjectItem*  ProjectItem::Find(ProjectItemType type, const wstring& name) const
      {
         // Check self
         if (Type == type && Name == name)
            return const_cast<ProjectItem*>(this);

         // Search children
         for (auto& c : Children)
            if (auto it = c.Find(type, name))
               return it;

         // Not found
         return nullptr;
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
      void  ProjectItem::InitialCommit(const Path& folder) const
      {
         // Feedback
         Console << Cons::Heading << "Performing Initial Commit: " << FullPath << ENDL;

         // Ensure backup path has been set
         if (BackupName.empty())
            throw ArgumentException(HERE, L"item", VString(L"Missing backup file path for '%s'", Name.c_str()) );

         // Read script from disc
         auto script = ScriptFileReader(XFileInfo(FullPath).OpenRead()).ReadFile(FullPath, false);
         
         // Create script revision
         BackupFile backup(BackupType::MSCI);
         backup.Revisions.Commit( ScriptRevision(L"Initial Commit", FullPath, script.GetAllText(), script) );

         // Write backup
         backup.Write(folder+BackupName);

         /*catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e, VString(L"Unable to perform initial commit of '%s'", item.Name.c_str()));
         }*/
      }

      /// <summary>Determines whether this instance is a file.</summary>
      /// <returns></returns>
      bool  ProjectItem::IsFile() const
      {
         return Type == ProjectItemType::File;
      }

      /// <summary>Determines whether this instance is folder.</summary>
      /// <returns></returns>
      bool  ProjectItem::IsFolder() const
      {
         return Type == ProjectItemType::Folder;
      }
         
      /// <summary>Determines whether this instance is the root.</summary>
      /// <returns></returns>
      bool  ProjectItem::IsRoot() const
      {
         return Type == ProjectItemType::Root;
      }

      /// <summary>Determines whether this instance is a variable.</summary>
      /// <returns></returns>
      bool  ProjectItem::IsVariable() const
      {
         return Type == ProjectItemType::Variable;
      }
      
      /// <summary>Removes a child without destroying it</summary>
      /// <param name="p">item</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
      bool  ProjectItem::Remove(const ProjectItem& p)
      {
         // Depth first search. 
         for (auto c = Children.begin(), end = Children.end(); c != end; ++c)
         {
            // Found: Remove child
            if (&p == &*c)
            {
               Children.erase(c);
               return true;
            }
            // Failed: Search grandchildren
            else if (c->Remove(p))
               return true;
         }

         // Not found
         return false;
      }

      /// <summary>Generates a unique path for the backup file.</summary>
      /// <param name="folder">Project folder.</param>
      void  ProjectItem::SetBackupPath(Path folder)
      {
         // Set default path (Filename.zip)
         auto path = folder + (FullPath.FileName + L".zip");
            
         // [EXISTS] Append numbers until unique
         for (int i = 2; path.Exists(); ++i)
            path = folder + VString(L"%s%d.zip", FullPath.FileName.c_str(), i);

         // Set filename
         BackupName = path.FileName;
      }

      /// <summary>Get all items as a list.</summary>
      /// <returns></returns>
      void  ProjectItem::ToList(list<ProjectItem*>& list) const
      {
         list.push_back(const_cast<ProjectItem*>(this));

         // Add children
         for (auto& c : Children)
            c.ToList(list);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
