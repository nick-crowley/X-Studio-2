#pragma once
#include "Common.h"
#include "ProjectItem.h"

namespace Logic
{
   namespace Projects
   {
      
      /// <summary></summary>
      class ProjectFile
      {
         // ------------------------ TYPES --------------------------
      public:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFile();
         virtual ~ProjectFile();

         DEFAULT_COPY(ProjectFile);	// Default copy semantics
         DEFAULT_MOVE(ProjectFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query existence of item by path.</summary>
         /// <param name="p">Full path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const
         {
            return any_of(Items.begin(), Items.end(), [&path](const ProjectItem& item) {return item.Contains(path);} );
         }

         /// <summary>Finds item by path.</summary>
         /// <param name="p">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         //ProjectItem  Find(IO::Path p) const
         //{
         //   // Find 
         //   for (auto& item : Items)
         //      if ()
         //         return ptr;

         //   // Not found
         //   return ProjectItemPtr(nullptr);
         //}

         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         list<ProjectItem*>  ToList() const
         {
            list<ProjectItem*> l;
            for (auto& it : *this)
               it->ToList(l);
            return l;
         }
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Add base folder</summary>
         /// <param name="p">item</param>
         /// <exception cref="Logic::ArgumentException">Item is not fixed</exception>
         void  Add(const ProjectItem& p)
         {
            // Ensure base folder
            if (!p.Fixed || !p.IsFolder())
               throw ArgumentException(HERE, L"p", GuiString(L"Cannot add '%s' - not a base folder", p.Name.c_str()));

            // Append
            Items.push_back(p);
         }

         ProjectItem& operator[](UINT index)
         {
            UINT i = 0;

            // Linear search
            for (auto& item : Items)
               if (index == i++)
                  return item;
            
            // Invalid index
            throw IndexOutOfRangeException(HERE, index, Items.size());
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

         // -------------------- REPRESENTATION ---------------------
      public:
         ProjectItem  Root;
         
      private:
      };

   }
}

using namespace Logic::Projects;
