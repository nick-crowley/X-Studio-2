#pragma once
#include "ProjectItem.h"

namespace Logic
{
   namespace Projects
   {
      
      /// <summary></summary>
      class LogicExport ProjectFile
      {
         // ------------------------ TYPES --------------------------
      public:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFile(const Path& path);
         virtual ~ProjectFile();

         DEFAULT_COPY(ProjectFile);	// Default copy semantics
         DEFAULT_MOVE(ProjectFile);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Iterate thru items</summary>
         ProjectItemList::const_iterator begin() const  { return Root.Children.begin(); }
         ProjectItemList::const_iterator end() const    { return Root.Children.end();   }

         /// <summary>Query existence of item by path.</summary>
         /// <param name="path">Full path.</param>
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

          /// <summary>Finds root folder for a filetype.</summary>
         /// <param name="type">File Type.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         ProjectItem*  FindFolder(FileType type) const;
         
         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         list<ProjectItem*>  ToList() const;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Iterate thru items</summary>
         ProjectItemList::iterator begin()   { return Root.Children.begin(); }
         ProjectItemList::iterator end()     { return Root.Children.end();   }

         /// <summary>Add base folder</summary>
         /// <param name="p">item</param>
         /// <exception cref="Logic::ArgumentException">Item is not fixed</exception>
         ProjectItem&  Add(const ProjectItem& p);

         /// <summary>Finds an removes an item</summary>
         /// <param name="p">item</param>
         /// <returns>Item removed, or nullptr</returns>
         /// <exception cref="Logic::ArgumentException">Item is fixed</exception>
         bool  Remove(ProjectItem& p);


         // -------------------- REPRESENTATION ---------------------
      public:
         ProjectItem  Root;
         
      };

   }
}

using namespace Logic::Projects;
