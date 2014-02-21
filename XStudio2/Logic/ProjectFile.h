#pragma once

#include "Common.h"

namespace Logic
{
   namespace Projects
   {
      class ProjectItem;

      /// <summary>Defines item types within a project</summary>
      enum class ProjectItemType  { File, Folder, Variable };

      /// <summary>Vector of project items</summary>
      typedef vector<ProjectItem>  ProjectItemArray;

      /// <summary>Base class for all items/folders within a project</summary>
      class ProjectItem
      {
      protected:
         ProjectItem(ProjectItemType t, wstring name) : Type(t), Name(name)
         {}
      public:
         virtual ~ProjectItem()
         {}

      public:
         const ProjectItemType  Type;
         wstring                Name;

         ProjectItemArray       Children;
      };

      /// <summary>Variable within a project</summary>
      class ProjectVariableItem : public ProjectItem
      {
      public:
         ProjectVariableItem(const wstring& name, int val) : ProjectItem(ProjectItemType::Variable, name), Value(val)
         {}

      public:
         int  Value;
      };

      /// <summary>File within a project</summary>
      class ProjectFileItem : public ProjectItem
      {
      public:
         ProjectFileItem(IO::Path path) : ProjectItem(ProjectItemType::File, path.FileName), FullPath(path)
         {}

      public:
         IO::Path   FullPath;
      };

      /// <summary>Folder within a project</summary>
      class ProjectFolderItem : public ProjectItem
      {
      public:
         ProjectFolderItem(const wstring& name) : ProjectItem(ProjectItemType::Folder, name)
         {}

      public:
      };

      /// <summary></summary>
      class ProjectFile
      {
         // ------------------------ TYPES --------------------------
      private:

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
         ProjectItemArray  Items;
         
      private:
      };

   }
}

using namespace Logic::Projects;
