#pragma once
#include "Common.h"
#include "FileIdentifier.h"   // FileType

namespace Logic
{
   namespace Projects
   {
      class ProjectItem;

      /// <summary>Defines item types within a project</summary>
      enum class ProjectItemType  { File, Folder, Variable };

      
      /// <summary>Shared pointer to a project item</summary>
      typedef shared_ptr<ProjectItem>  ProjectItemPtr;

      /// <summary>Vector of project items</summary>
      typedef vector<ProjectItemPtr>  ProjectItemArray;

      /// <summary>Base class for all items/folders within a project</summary>
      class ProjectItem
      {
         // ------------------------ TYPES --------------------------
      public:
         
         // --------------------- CONSTRUCTION ----------------------
      protected:
         ProjectItem(ProjectItemType t, wstring name, bool fixed) : Type(t), Name(name), Fixed(fixed)
         {}
      public:
         virtual ~ProjectItem()
         {}

         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether any child contains a given path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         virtual bool Contains(IO::Path path) const PURE;

         /// <summary>Finds item by path.</summary>
         /// <param name="p">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         virtual ProjectItemPtr  Find(IO::Path p) const PURE;

         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         void  ToList(list<ProjectItem*>& list) const
         {
            list.push_back(const_cast<ProjectItem*>(this));

            // Add children
            for (auto& c : Children)
               c->ToList(list);
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append child item</summary>
         /// <param name="p">item</param>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         void  Add(ProjectItem* p)
         {
            REQUIRED(p);

            Children.push_back(ProjectItemPtr(p));
         }

         /// <summary>Append child item</summary>
         /// <param name="p">item</param>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         void  Add(ProjectItemPtr& p)
         {
            REQUIRED(p);

            Children.push_back(p);
         }

         /// <summary>Removes a child without destroying it</summary>
         /// <param name="p">item</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         ProjectItemPtr  Remove(ProjectItem* p)
         {
            // Depth first search. 
            for (auto pos = Children.begin(), end = Children.end(); pos != end; ++pos)
            {
               // Found: Remove child
               if (p == pos->get())
               {
                  ProjectItemPtr ptr(*pos);
                  Children.erase(pos);
                  return ptr;
               }
               // Failed: Search grandchildren
               else if (ProjectItemPtr ptr = pos->get()->Remove(p))
                  return ptr;
            }

            // Not found
            return ProjectItemPtr(nullptr);
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const ProjectItemType  Type;
         const bool             Fixed;
         wstring                Name;
         
         ProjectItemArray       Children;
      };

      /// <summary>Variable within a project</summary>
      class ProjectVariableItem : public ProjectItem
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectVariableItem(const wstring& name, int val) : ProjectItem(ProjectItemType::Variable, name, false), Value(val)
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Variables cannot contain a path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const override
         {
            return false;
         }

         /// <summary>Variables cannot contain a path</summary>
         /// <param name="p">Full path.</param>
         /// <returns>nullptr</returns>
         ProjectItemPtr  Find(IO::Path p) const override
         {
            return ProjectItemPtr(nullptr);
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         int  Value;
      };

      /// <summary>File within a project</summary>
      class ProjectFileItem : public ProjectItem
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFileItem(IO::Path path, FileType t) 
            : ProjectItem(ProjectItemType::File, path.FileName, false), FullPath(path), FileType(t)
         {}
         ProjectFileItem(IO::Path path, FileType t, IO::Path backup) 
            : ProjectItem(ProjectItemType::File, path.FileName, false), FullPath(path), FileType(t), BackupPath(backup)
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Compare path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const override
         {
            return FullPath == path;
         }

         /// <summary>Compare path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         ProjectItemPtr  Find(IO::Path path) const override
         {
            return ProjectItemPtr(FullPath == path ? const_cast<ProjectFileItem*>(this) : nullptr);
         }
      
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Generates a unique path for the backup file.</summary>
         /// <param name="folder">Project folder.</param>
         void SetBackupPath(IO::Path folder)
         {
            // Set default path (Filename.zip)
            BackupPath = folder + (FullPath.FileName + L".zip");
            
            // [EXISTS] Append numbers until unique
            for (int i = 2; BackupPath.Exists(); ++i)
               BackupPath = folder + GuiString(L"%s%d.zip", FullPath.FileName.c_str(), i);
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         IO::Path   FullPath,
                    BackupPath;
         FileType   FileType;
      };

      /// <summary>Folder within a project</summary>
      class ProjectFolderItem : public ProjectItem
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFolderItem(const wstring& name, bool fixed) : ProjectItem(ProjectItemType::Folder, name, fixed)
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Check for child with matching path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const override
         {
            return any_of(Children.begin(), Children.end(), [path](const ProjectItemPtr& it) {return it->Contains(path);} );
         }

         /// <summary>Finds item by path.</summary>
         /// <param name="p">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         ProjectItemPtr  Find(IO::Path p) const override
         {
            // Depth first search. 
            for (auto c = Children.begin(), end = Children.end(); c != end; ++c)
               if (auto item = (*c)->Find(p))
                  // Found: Return child
                  return item;

            // Failed: Return nullptr
            return ProjectItemPtr(nullptr);
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
      };

   }
}

using namespace Logic::Projects;
