#pragma once

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
      class LogicExport ProjectItem
      {
         // ------------------------ TYPES --------------------------
      public:
         
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create root</summary>
         /// <param name="name">project name.</param>
         ProjectItem(const wstring& name);

         /// <summary>Create file item</summary>
         /// <param name="file">file type.</param>
         /// <param name="path">Full path.</param>
         /// <param name="backup">backup name.</param>
         ProjectItem(FileType file, Path path, Path backup);

         /// <summary>Create variable item</summary>
         /// <param name="name">variable name.</param>
         /// <param name="value">value.</param>
         ProjectItem(const wstring& name, int value);

         /// <summary>Create folder item</summary>
         /// <param name="name">folder name.</param>
         /// <param name="fixed">whether fixed</param>
         ProjectItem(const wstring& name, bool fixed);

         virtual ~ProjectItem();

         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether any child contains a given path</summary>
         /// <param name="path">The path.</param>
         /// <returns></returns>
         bool Contains(Path path) const;

         /// <summary>Finds item by path.</summary>
         /// <param name="path">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         ProjectItem*  Find(Path path) const;
         
         /// <summary>Finds item by name and type.</summary>
         /// <param name="type">Type.</param>
         /// <param name="name">Name.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         ProjectItem*  Find(ProjectItemType type, const wstring& name) const;
         
         /// <summary>Creates a backup file for a file item and performs an initial commit.</summary>
         /// <param name="folder">Backup folder.</param>
         /// <exception cref="Logic::ArgumentException">Backup path not set</exception>
         /// <exception cref="Logic::ArgumentNullException">Script format invalid</exception>
         /// <exception cref="Logic::ComException">COM Error</exception>
         /// <exception cref="Logic::FileFormatException">Script format invalid</exception>
         /// <exception cref="Logic::InvalidValueException">Script format invalid</exception>
         /// <exception cref="Logic::InvalidOperationException">Script format invalid</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         void InitialCommit(const Path& folder) const;

         /// <summary>Determines whether this instance is a file.</summary>
         /// <returns></returns>
         bool  IsFile() const;

         /// <summary>Determines whether this instance is folder.</summary>
         /// <returns></returns>
         bool  IsFolder() const;
         
         /// <summary>Determines whether this instance is the root.</summary>
         /// <returns></returns>
         bool  IsRoot() const;

         /// <summary>Determines whether this instance is a variable.</summary>
         /// <returns></returns>
         bool  IsVariable() const;

         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         void  ToList(list<ProjectItem*>& list) const;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Append copy of item as child</summary>
         /// <param name="p">item</param>
         /// <returns>Reference to inserted child</returns>
         ProjectItem& Add(const ProjectItem& p);

         /// <summary>Removes a child without destroying it</summary>
         /// <param name="p">item</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         bool Remove(const ProjectItem& p);

         /// <summary>Generates a unique path for the backup file.</summary>
         /// <param name="folder">Project folder.</param>
         void SetBackupPath(Path folder);

         // -------------------- REPRESENTATION ---------------------
      public:
         ProjectItemType   Type;        // Item type
         bool              Fixed;       // Whether immoveable
         wstring           Name;        // Item name

         Path              FullPath;    // [Files] Full path
         wstring           BackupName;  // [Files] Filename of backup file
         FileType          FileType;    // [Files] File type

         int               Value;       // [Variables] Value

         ProjectItemList   Children;    // Children
      };



   }
}

using namespace Logic::Projects;
