#include "stdafx.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace Projects
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      ProjectFile::ProjectFile(const Path& path) : Root(path.FileName)
      {
         Root.FullPath = path;
      }

      ProjectFile::~ProjectFile()
      {}

      // ------------------------------- STATIC METHODS -------------------------------


      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Add base folder</summary>
      /// <param name="p">item</param>
      /// <exception cref="Logic::ArgumentException">Item is not fixed</exception>
      ProjectItem&  ProjectFile::Add(const ProjectItem& p)
      {
         return Root.Add(p);
      }

      /// <summary>Query existence of item by path.</summary>
      /// <param name="path">Full path.</param>
      /// <returns></returns>
      bool  ProjectFile::Contains(Path path) const
      {
         //return Root.Contains(path);
         return any_of(Root.Children.begin(), Root.Children.end(), [&path](const ProjectItem& item) {return item.Contains(path);} );
      }

      /// <summary>Finds item by path.</summary>
      /// <param name="path">Full path.</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      ProjectItem*  ProjectFile::Find(Path path) const
      {
         return Root.Find(path);
      }

      /// <summary>Finds item by name and type.</summary>
      /// <param name="type">Type.</param>
      /// <param name="name">Name.</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      ProjectItem*  ProjectFile::Find(ProjectItemType type, const wstring& name) const
      {
         return Root.Find(type, name);
      }

         /// <summary>Finds root folder for a filetype.</summary>
      /// <param name="type">File Type.</param>
      /// <returns>Item if found, otherwise nullptr</returns>
      ProjectItem*  ProjectFile::FindFolder(FileType type) const
      {
         switch (type)
         {
         case FileType::Script:    return Root.Find(ProjectItemType::Folder, L"MSCI Scripts");
         case FileType::Language:  return Root.Find(ProjectItemType::Folder, L"Language Files");
         case FileType::Mission:   return Root.Find(ProjectItemType::Folder, L"MD Scripts");
         default:                  return Root.Find(ProjectItemType::Folder, L"Other Files");
         }
      }

      /// <summary>Get all items as a list.</summary>
      /// <returns></returns>
      list<ProjectItem*>  ProjectFile::ToList() const
      {
         list<ProjectItem*> l;
         Root.ToList(l);
         return l;
      }

      /// <summary>Finds an removes an item</summary>
      /// <param name="p">item</param>
      /// <returns>Item removed, or nullptr</returns>
      /// <exception cref="Logic::ArgumentException">Item is fixed</exception>
      bool  ProjectFile::Remove(ProjectItem& p)
      {
         // Cannot remove base folders
         if (p.Fixed)
            throw ArgumentException(HERE, L"p", VString(L"Cannot remove fixed item '%s'", p.Name.c_str()));

         // Remove
         return Root.Remove(p);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
