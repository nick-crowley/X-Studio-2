#pragma once
#include "Common.h"
#include "ProjectItem.h"

FORWARD_DECLARATION2(Logic,IO,class ProjectFileWriter)

namespace Logic
{
   namespace Projects
   {
      
      /// <summary></summary>
      class ProjectFile
      {
         friend class ProjectFileWriter;

         // ------------------------ TYPES --------------------------
      public:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFile(const wstring& name) : Root(name)
         {}
         virtual ~ProjectFile()
         {}

         DEFAULT_COPY(ProjectFile);	// Default copy semantics
         DEFAULT_MOVE(ProjectFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         ProjectItemList::iterator begin()   { return Root.Children.begin(); }
         ProjectItemList::iterator end()     { return Root.Children.end();   }
         ProjectItemList::const_iterator begin() const  { return Root.Children.begin(); }
         ProjectItemList::const_iterator end() const    { return Root.Children.end();   }

         /// <summary>Query existence of item by path.</summary>
         /// <param name="path">Full path.</param>
         /// <returns></returns>
         bool Contains(IO::Path path) const
         {
            return Root.Contains(path);
         }

         /// <summary>Finds item by path.</summary>
         /// <param name="path">Full path.</param>
         /// <returns>Item if found, otherwise nullptr</returns>
         ProjectItem*  Find(IO::Path path) const
         {
            return Root.Find(path);
         }

         /// <summary>Get all items as a list.</summary>
         /// <returns></returns>
         list<ProjectItem*>  ToList() const
         {
            list<ProjectItem*> l;
            Root.ToList(l);
            return l;
         }
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Add base folder</summary>
         /// <param name="p">item</param>
         /// <exception cref="Logic::ArgumentException">Item is not fixed</exception>
         ProjectItem&  Add(const ProjectItem& p)
         {
            return Root.Add(p);
         }

         //ProjectItem& operator[](UINT index)
         //{
         //   UINT i = 0;

         //   // Linear search
         //   for (auto& item : Items)
         //      if (index == i++)
         //         return item;
         //   
         //   // Invalid index
         //   throw IndexOutOfRangeException(HERE, index, Items.size());
         //}

         /// <summary>Finds an removes an item</summary>
         /// <param name="p">item</param>
         /// <returns>Item removed, or nullptr</returns>
         /// <exception cref="Logic::ArgumentException">Item is fixed</exception>
         bool  Remove(ProjectItem& p)
         {
            // Cannot remove base folders
            if (p.Fixed)
               throw ArgumentException(HERE, L"p", GuiString(L"Cannot remove fixed item '%s'", p.Name.c_str()));

            // Remove
            return Root.Remove(p);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         ProjectItem  Root;
         
      };

   }
}

using namespace Logic::Projects;
