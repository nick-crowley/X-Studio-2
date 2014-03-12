#pragma once
#include "Common.h"

namespace Logic
{
   namespace Projects
   {
      /// <summary>Types of backup file</summary>
      enum class BackupType { MSCI, MD };

      /// <summary></summary>
      class ScriptRevision
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptRevision()
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

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
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
         BackupFile()
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
