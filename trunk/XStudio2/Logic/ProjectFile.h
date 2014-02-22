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
      private:
         //static function<bool (const ProjectItem&)>  IsEqual;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Checks whether a child item is present</summary>
         /// <param name="p">item</param>
         /// <returns></returns>
         bool  Contains(ProjectItem* p)
         {
            // Search children
            for (auto& c : Children)
               if (c.get() == p || c->Contains(p))
                  return true;

            return false;
         }

         /// <summary>Finds a child</summary>
         /// <param name="p">item</param>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Item does not exist</exception>
         ProjectItemPtr  Find(ProjectItem* p)
         {
            // Depth first search. 
            for (auto& c : Children)
            {
               // Found: Remove child
               if (p == c.get())
                  return c;
               
               // Failed: Search grandchildren
               else if (c->Contains(p))
                  return c->Find(p);
            }

            throw AlgorithmException(HERE, GuiString(L"Project item '%s' not found", p->Name.c_str()));
         }

         /// <summary>Removes a child</summary>
         /// <param name="p">item</param>
         /// <returns>True if found, false if not</returns>
         bool  Remove(ProjectItem* p)
         {
            // Depth first search. 
            for (auto pos = Children.begin(), end = Children.end(); pos != end; ++pos)
            {
               // Found: Remove child
               if (p == pos->get())
               {
                  Children.erase(pos);
                  return true;
               }
               // Failed: Search grandchildren
               else if ((*pos)->Remove(p))
                  return true;
            }

            // Not found
            return false;
         }

      protected:
         /// <summary>Equality test</summary>
         /// <param name="r">other item.</param>
         /// <returns></returns>
         virtual bool  Equals(const ProjectItem& r)
         {
            return Type == r.Type && Name == r.Name;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append child item</summary>
         /// <param name="p">item</param>
         void  Add(ProjectItem* p)
         {
            Children.push_back(ProjectItemPtr(p));
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
      protected:
         /// <summary>Equality test</summary>
         /// <param name="r">other item.</param>
         /// <returns></returns>
         bool  Equals(const ProjectItem& r) override
         {
            auto r2 = dynamic_cast<const ProjectFileItem&>(r);
            return ProjectItem::Equals(r) && FullPath == r2.FullPath;
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
            /// <summary>Append child item</summary>
            /// <param name="p">item</param>
            void  Add(ProjectItem* p)
            {
               push_back(ProjectItemPtr(p));
            }

            /// <summary>Finds an item</summary>
            /// <param name="p">item</param>
            /// <returns></returns>
            /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
            /// <exception cref="Logic::AlgorithmException">Item does not exist</exception>
            ProjectItemPtr  Find(ProjectItem* p)
            {
               REQUIRED(p);

               // Search folders
               for (auto& folder : *this)
               {
                  if (folder.get() == p)
                     return folder;

                  if (folder->Contains(p))
                     return folder->Find(p);
               }

               // Not found
               throw AlgorithmException(HERE, GuiString(L"Project item '%s' not found", p->Name.c_str()));
            }

            /// <summary>Finds an removes an item</summary>
            /// <param name="p">item</param>
            /// <returns></returns>
            /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
            bool  Remove(ProjectItem* p)
            {
               REQUIRED(p);

               // Search/Remove from folders
               for (auto& folder : *this)
               {
                  if (folder.get() == p)
                     throw AlgorithmException(HERE, GuiString(L"Cannot remove project base folder '%s'", p->Name.c_str()));

                  // Search folder
                  if (folder->Remove(p))
                     return true;
               }

               // Not found
               return false;
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
