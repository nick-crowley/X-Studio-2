#pragma once
#include "Common.h"
#include "ScriptFile.h"
#include "ScriptRevision.h"

/// <summary>Forward declaration</summary>
FORWARD_DECLARATION2(Logic,IO,class BackupFileReader)

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
      class BackupFile
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary></summary>
         class RevisionCollection : protected list<ScriptRevision>
         {
            friend class BackupFileReader;

            // --------------------- CONSTRUCTION ----------------------
         public:
            RevisionCollection()
            {}

            // ---------------------- PROPERTIES ------------------------			
         public:
            PROPERTY_GET(UINT,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Iterate thru revisions</summary>
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
            /// <summary>Iterate thru revisions</summary>
            iterator begin()  { return __super::begin(); }
            iterator end()    { return __super::end();   }

            /// <summary>Clears this instance.</summary>
            void  Clear()
            {
               __super::clear();
            }

            /// <summary>Adds a new revision</summary>
            /// <param name="r">revision</param>
            void  Commit(const ScriptRevision& r)
            {
               push_front(r);
            }

            /// <summary>Adds a new revision</summary>
            /// <param name="r">revision</param>
            void  Commit(ScriptRevision&& r)
            {
               push_front(r);
            }

            /// <summary>Find a revision by index</summary>
            /// <param name="index">Zero-based index</param>
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
            /// <param name="index">Zero-based index.</param>
            /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
            void  Remove(UINT index)
            {
               UINT i = 0;

               // Linear search
               for (iterator it = begin(); it != end(); ++it)
                  if (index == i++)
                  {
                     erase(it);
                     return;
                  }

               // Not found
               throw IndexOutOfRangeException(HERE, index, size());
            }

         protected:
            /// <summary>Appends a historic revision</summary>
            /// <param name="r">revision</param>
            void  AppendHistoric(ScriptRevision&& r)
            {
               push_back(r);
            }
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
      public:
         /// <summary>Clears this instance.</summary>
         void  Clear()
         {
            Revisions.Clear();
         }

         /// <summary>Removes a revision.</summary>
         /// <param name="index">Zero-based index.</param>
         /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
         void  Remove(UINT index)
         {
            Revisions.Remove(index);
         }

         /// <summary>Find a revision by index</summary>
         /// <param name="index">The index</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
         ScriptRevision&  operator[](UINT index) 
         {
            return Revisions.FindByIndex(index);
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         RevisionCollection  Revisions;

         BackupType Type;
         //wstring Name;
      };

   }
}

using namespace Logic::Projects;
