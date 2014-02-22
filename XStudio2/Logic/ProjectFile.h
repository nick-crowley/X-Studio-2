#pragma once

#include "Common.h"
#include "FileIdentifier.h"

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
         ProjectFileItem(IO::Path path, FileType t) : ProjectItem(ProjectItemType::File, path.FileName, false), FullPath(path), FileType(t)
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

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         IO::Path   FullPath;
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

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
      };

      /// <summary></summary>
      class ProjectFile
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary></summary>
         class ItemCollection : public ProjectItemArray
         {
         public:
            ItemCollection()
            {}
            ~ItemCollection()
            {}

            /// <summary>Add base folder</summary>
            /// <param name="p">item</param>
            /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
            /// <exception cref="Logic::AlgorithmException">Item is not fixed</exception>
            void  Add(ProjectItem* p)
            {
               REQUIRED(p);

               // Ensure base folder
               if (!p->Fixed || p->Type != ProjectItemType::Folder)
                  throw AlgorithmException(HERE, GuiString(L"Cannot add '%s' - not a base folder", p->Name.c_str()));

               push_back(ProjectItemPtr(p));
            }

            /// <summary>Finds an removes an item</summary>
            /// <param name="p">item</param>
            /// <returns>Item removed, or nullptr</returns>
            /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
            /// <exception cref="Logic::AlgorithmException">Item is fixed</exception>
            ProjectItemPtr  Remove(ProjectItem* p)
            {
               REQUIRED(p);

               // Cannot remove base folders
               if (p->Fixed)
                  throw AlgorithmException(HERE, GuiString(L"Cannot remove fixed item '%s'", p->Name.c_str()));

               // Find and remove
               for (auto& folder : *this)
               {
                  if (ProjectItemPtr ptr = folder->Remove(p))
                     return ptr;
               }

               // Not found
               return ProjectItemPtr(nullptr);
            }
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFile();
         virtual ~ProjectFile();

         DEFAULT_COPY(ProjectFile);	// Default copy semantics
         DEFAULT_MOVE(ProjectFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         ItemCollection  Items;
         
      private:
      };

   }
}

using namespace Logic::Projects;
