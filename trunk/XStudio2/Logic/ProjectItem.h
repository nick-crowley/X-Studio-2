#pragma once
#include "Common.h"
#include "FileIdentifier.h"   // FileType

namespace Logic
{
   namespace Projects
   {
      class ProjectItem;

      /// <summary>Defines item types within a project</summary>
      enum class ProjectItemType  { Root, File, Folder, Variable };

      /// <summary>List of project items</summary>
      typedef list<ProjectItem>  ProjectItemList;

      /// <summary>Represents any file/folder/variable within a project</summary>
      class ProjectItem
      {
         // ------------------------ TYPES --------------------------
      public:
         
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create root</summary>
         /// <param name="name">project name.</param>
         ProjectItem(const wstring& name) 
            : Type(ProjectItemType::Root), Name(name), Fixed(true), FileType(FileType::Unknown), Value(-1)
         {}
         /// <summary>Create file item</summary>
         /// <param name="file">file type.</param>
         /// <param name="path">Full path.</param>
         /// <param name="backup">backup name.</param>
         ProjectItem(FileType file, IO::Path path, IO::Path backup)
            : Type(ProjectItemType::File), FileType(file), FullPath(path), BackupName(backup), Name(path.FileName), Fixed(false), Value(-1)
         {}
         /// <summary>Create variable item</summary>
         /// <param name="name">variable name.</param>
         /// <param name="value">value.</param>
         ProjectItem(const wstring& name, int value)
            : Type(ProjectItemType::Variable), Name(name), Value(value), Fixed(false), FileType(FileType::Unknown)
         {}
         /// <summary>Create folder item</summary>
         /// <param name="name">folder name.</param>
         /// <param name="fixed">whether fixed</param>
         ProjectItem(const wstring& name, bool fixed)
            : Type(ProjectItemType::Folder), Name(name), Fixed(fixed), FileType(FileType::Unknown), Value(-1)
         {}

         virtual ~ProjectItem()
         {}

         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether any child contains a given path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const
         {
            if (FullPath == path)
               return true;

            return any_of(Children.begin(), Children.end(), [&path](ProjectItem& p) {return p.FullPath == path;} );
         }

         /// <summary>Finds item by path.</summary>
         /// <param name="path">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         //ProjectItem  Find(IO::Path path) const
         //{
         //   if (FullPath == path)
         //      return *this;

         //   // Search children
         //   auto pos = find_if(Children.begin(), Children.end(), [&path](ProjectItem& p) {return p.Find(path);} );
         //   if (pos == Children.end())
         //      throw;

         //   return *pos;
         //}
         
         /// <summary>Determines whether this instance is a file.</summary>
         /// <returns></returns>
         bool  IsFile() const
         {
            return Type == ProjectItemType::File;
         }

         /// <summary>Determines whether this instance is folder.</summary>
         /// <returns></returns>
         bool  IsFolder() const
         {
            return Type == ProjectItemType::Folder;
         }

         /// <summary>Determines whether this instance is a variable.</summary>
         /// <returns></returns>
         bool  IsVariable() const
         {
            return Type == ProjectItemType::Variable;
         }

         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         void  ToList(ProjectItemList& list) const
         {
            list.push_back(*this);

            // Add children
            for (auto& c : Children)
               c.ToList(list);
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append child item</summary>
         /// <param name="p">item</param>
         /// <returns>Item that was added</returns>
         ProjectItem& Add(const ProjectItem& p)
         {
            Children.push_back(p);
            return Children.back();
         }

         /// <summary>Removes a child without destroying it</summary>
         /// <param name="p">item</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         bool Remove(const ProjectItem& p)
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
         void SetBackupPath(IO::Path folder)
         {
            // Set default path (Filename.zip)
            auto path = folder + (FullPath.FileName + L".zip");
            
            // [EXISTS] Append numbers until unique
            for (int i = 2; path.Exists(); ++i)
               path = folder + GuiString(L"%s%d.zip", FullPath.FileName.c_str(), i);

            // Set filename
            BackupName = path.FileName;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const ProjectItemType  Type;        // Item type
         const bool             Fixed;       // Whether immoveable
         wstring                Name;        // Item name

         IO::Path               FullPath;    // [Files] Full path
         wstring                BackupName;  // [Files] Filename of backup file
         FileType               FileType;    // [Files] File type

         int                    Value;       // [Variables] Value

         ProjectItemList        Children;    // Children
      };



   }
}

using namespace Logic::Projects;
