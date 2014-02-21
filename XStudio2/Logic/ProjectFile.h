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

      
      /// <summary>Vector of project items</summary>
      typedef vector<ProjectItem*>  ProjectItemArray;

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
         /*bool  Contains(ProjectItemType t, wstring name, bool recurse)
         {
            
            auto item = any_of(Children.begin(), Children.end(), IsEqual);
            if (item != Children.end())
               return true;

            
         }

         ProjectItem&  Find(ProjectItemType t, wstring name, bool recurse)
         {
         }

         bool operator==(const ProjectItem& r)
         {
            return Equals(r);
         }

         bool operator!=(const ProjectItem& r)
         {
            return !Equals(r);
         }*/

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
         void  Add(ProjectItem* p)
         {
            Children.push_back(p);
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
