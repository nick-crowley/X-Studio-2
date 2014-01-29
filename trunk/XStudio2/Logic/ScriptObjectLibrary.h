#pragma once

#include "Common.h"
#include "ScriptObject.h"
#include "WorkerFeedback.h"
#include "LanguageFile.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      

      /// <summary>Provides access to script objects</summary>
      class ScriptObjectLibrary
      {
		   // ------------------------ TYPES --------------------------
      private:
         /// <summary>Defines a {KnownPage,ID} pair</summary>
         class ObjectID
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ObjectID(ScriptObjectGroup g, UINT id) : Group(g), ID(id)
            {}

            // ---------------------- ACCESSORS ------------------------	
         public:
            bool operator==(const ObjectID& r) const {
               return Group == r.Group && ID == r.ID;
            }
            bool operator<(const ObjectID& r) const {
               return Group < r.Group || (Group == r.Group && ID < r.ID);
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            const ScriptObjectGroup Group;
            const UINT              ID;
         };

         /// <summary>Collection of Script objects sorted by ID</summary>
         class ObjectCollection : public map<ObjectID, ScriptObject, less<ObjectID>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            /// <summary>Finds a script object by ID</summary>
            /// <param name="page">The page.</param>
            /// <param name="id">The ID.</param>
            /// <returns></returns>
            /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
            ScriptObject  Find(ScriptObjectGroup grp, UINT id) const
            {
               const_iterator it;
               // Lookup and return string
               if ((it = find(ObjectID(grp, id))) != end())
                  return it->second;

               // Error: Not found
               throw ScriptObjectNotFoundException(HERE, grp, id);
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a script object</summary>
            /// <param name="obj">The object</param>
            /// <returns>True if successful, false if ID already present</returns>
            bool  Add(const ScriptObject& obj)
            {
               return insert(value_type(ObjectID(obj.Group, obj.ID), obj)).second;
            }

            /// <summary>Adds a script object</summary>
            /// <param name="obj">The object</param>
            /// <returns>True if successful, false if ID already present</returns>
            bool  Add(ScriptObject&& obj)
            {
               return insert(value_type(ObjectID(obj.Group, obj.ID), move(obj))).second;
            }
         };
      
      public:
         /// <summary>Collection of script objects sorted by text</summary>
         class LookupCollection : public map<wstring, ScriptObject, less<wstring>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            /// <summary>Check whether an object is present</summary>
            /// <param name="sz">The text</param>
            /// <returns></returns>
            bool  Contains(const wstring& sz) const
            {
               return find(sz) != end();
            }

            /// <summary>Finds an object by text<summary>
            /// <param name="sz">The text</param>
            /// <returns>Object</returns>
            /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
            ScriptObject  Find(const wstring& sz) const
            {
               const_iterator it;
               // Lookup and return string
               if ((it = find(sz)) != end())
                  return it->second;

               // Error: Not found
               throw ScriptObjectNotFoundException(HERE, sz);
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds an object</summary>
            /// <param name="obj">The object.</param>
            /// <returns>True if successful, false if key already present</returns>
            bool  Add(const ScriptObject& obj)
            {
               return insert(value_type(obj.Text, obj)).second;
            }

            /// <summary>Adds an object</summary>
            /// <param name="obj">The object.</param>
            /// <returns>True if successful, false if key already present</returns>
            bool  Add(ScriptObject&& obj)
            {
               return insert(value_type(obj.Text, move(obj))).second;
            }

            /// <summary>Removes an object from the collection</summary>
            /// <param name="sz">The text</param>
            void  Remove(const wstring& sz)
            {
               erase(sz);
            }
         };
	  
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptObjectLibrary();
         virtual ~ScriptObjectLibrary();
		 
		   DEFAULT_COPY(ScriptObjectLibrary);	// Default copy semantics
		   DEFAULT_MOVE(ScriptObjectLibrary);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(UINT,Count,GetCount);

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool          Contains(const wstring& obj) const;
         ScriptObject  Find(ScriptObjectGroup grp, UINT id) const;
         ScriptObject  Find(const wstring& sz) const;
         UINT          GetCount() const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);
         ScriptObjectArray Query(const wstring& str) const;

      protected:
         bool  InsertConflicts(ScriptObject a, ScriptObject b);
         bool  MangleConflicts(ScriptObject a, ScriptObject b);
         UINT  PopulateLookup(WorkerData* data);
         UINT  PopulateObjects(WorkerData* data);

         // -------------------- REPRESENTATION ---------------------
      public:
         static ScriptObjectLibrary  Instance;
         
      private:
         ObjectCollection  Objects;
         LookupCollection  Lookup;
      };
   }

   /// <summary>Access to the script object library singleton</summary>
   #define ScriptObjectLib  ScriptObjectLibrary::Instance
}

using namespace Logic::Scripts;
