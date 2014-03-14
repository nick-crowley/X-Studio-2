#pragma once
#include "Common.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Projects
   {
      /// <summary>Types of backup file</summary>
      enum class BackupType { MSCI, MD };

      /// <summary>Get backup file type string</summary>
      /// <param name="t">type.</param>
      /// <exception cref="Logic::ArgumentException">Unknown type</exception>
      inline wstring GetString(BackupType t)
      {
         switch (t)
         {
         case BackupType::MSCI:  return L"msci";
         case BackupType::MD:    return L"md";
         }
         throw ArgumentException(HERE, L"t", GuiString(L"Unrecognised backup type %d", t));
      }

      /// <summary></summary>
      class ScriptRevision
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty revision.</summary>
         ScriptRevision()
         {}
         /// <summary>Create script revision.</summary>
         /// <param name="title">Revision title.</param>
         /// <param name="path">Script path.</param>
         /// <param name="content">Script content.</param>
         /// <param name="script">script properties.</param>
         ScriptRevision(const wstring& title, IO::Path path, const wstring& content, const ScriptFile& script)
            : Date(_time64(nullptr)),
              Title(title), 
              FullPath(path), 
              Content(content), 
              ScriptName(script.Name), 
              Description(script.Description), 
              Game(script.Game), 
              Version(script.Version)
         {}
         virtual ~ScriptRevision()
         {}

         DEFAULT_COPY(ScriptRevision);	// Default copy semantics
         DEFAULT_MOVE(ScriptRevision);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         IO::Path      FullPath;       // Full path of document when committed
         wstring       Title,          // Revision description
                       Content;        // Revision text
         COleDateTime  Date;           // Revision date

         wstring      ScriptName,      // Script name
                      Description;     // Script description
         GameVersion  Game;            // Script engine version
         UINT         Version;         // Script version
      };

      /// <summary></summary>
      class BackupFile
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary></summary>
         class RevisionCollection : protected list<ScriptRevision>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            RevisionCollection()
            {}

            // ---------------------- PROPERTIES ------------------------			
         public:
            PROPERTY_GET(UINT,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------			
         public:
            iterator begin()              { return __super::begin(); }
            iterator end()                { return __super::end();   }
            const_iterator begin() const  { return __super::begin(); }
            const_iterator end() const    { return __super::end();   }

            /// <summary>Gets the count.</summary>
            /// <returns></returns>
            UINT GetCount() const
            {
               return size();
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a revision</summary>
            /// <param name="r">revision</param>
            void  Add(const ScriptRevision& r)
            {
               push_front(r);
            }

            /// <summary>Adds a revision</summary>
            /// <param name="r">revision</param>
            void  Add(ScriptRevision&& r)
            {
               push_front(r);
            }

            /// <summary>Find a revision by index</summary>
            /// <param name="index">The index</param>
            /// <returns></returns>
            /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
            ScriptRevision&  FindByIndex(UINT index) 
            {
               UINT i = 0;

               // Linear search
               for (iterator it = begin(); it != end(); ++it)
                  if (index == i++)
                     return *it;

               // Not found
               throw IndexOutOfRangeException(HERE, index, size());
            }

            /// <summary>Removes a revision.</summary>
            /// <param name="r">revision.</param>
            /*void  Remove(const ScriptRevision& r)
            {
               remove(r);
            }*/
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         BackupFile(BackupType t) : Type(t)
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         

         // ---------------------- ACCESSORS ------------------------			
      public:
         

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         RevisionCollection  Revisions;

         BackupType Type;
         //wstring Name;
      };

   }
}

using namespace Logic::Projects;
