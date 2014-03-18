#pragma once


#include "ScriptObject.h"
#include "WorkerData.h"
#include "LanguageFile.h"
#include "StringLibrary.h"
#include "MapIterator.hpp"

namespace Logic
{
   namespace Scripts
   {
      

      /// <summary>Provides access to script objects</summary>
      class LogicExport ScriptObjectLibrary
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

            /// <summary>Queries whether a script object is present</summary>
            /// <param name="grp">object group</param>
            /// <param name="id">object id</param>
            /// <returns></returns>
            bool  Contains(ScriptObjectGroup grp, UINT id) const
            {
               return find(ObjectID(grp, id)) != end();
            }

            /// <summary>Finds a script object by ID</summary>
            /// <param name="page">The page.</param>
            /// <param name="id">The ID.</param>
            /// <returns></returns>
            /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
            ScriptObjectRef  Find(ScriptObjectGroup grp, UINT id) const
            {
               const_iterator it;
               // Lookup and return string
               if ((it = find(ObjectID(grp, id))) != end())
                  return it->second;

               // Error: Not found
               throw ScriptObjectNotFoundException(HERE, grp, id);
            }
            
            /// <summary>Finds a script object by ID</summary>
            /// <param name="group">object group</param>
            /// <param name="id">object id</param>
            /// <param name="obj">object</param>
            /// <returns>true if found, false otherwise</returns>
            bool  TryFind(ScriptObjectGroup grp, UINT id, const ScriptObject* &obj) const
            {
               const_iterator it;

               // Lookup object, Set/clear result
               obj = (it=find(ObjectID(grp, id))) != end() ? &it->second : nullptr;
               return obj != nullptr;
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
         class LookupCollection : public map<GuiString, ScriptObject, less<GuiString>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            /// <summary>Check whether an object is present</summary>
            /// <param name="sz">The text</param>
            /// <returns></returns>
            bool  Contains(const GuiString& sz) const
            {
               return find(sz) != end();
            }

            /// <summary>Finds an object by text<summary>
            /// <param name="sz">Object text</param>
            /// <returns>Object</returns>
            /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
            ScriptObjectRef  Find(const GuiString& sz) const
            {
               const_iterator it;
               // Lookup and return string
               if ((it = find(sz)) != end())
                  return it->second;

               // Error: Not found
               throw ScriptObjectNotFoundException(HERE, sz);
            }

            /// <summary>Finds a script object by name</summary>
            /// <param name="name">object name</param>
            /// <param name="obj">object</param>
            /// <returns>true if found, false otherwise</returns>
            bool  TryFind(const GuiString& name, const ScriptObject* &obj) const
            {
               const_iterator it;

               // Lookup object, Set/clear result
               obj = (it=find(name)) != end() ? &it->second : nullptr;
               return obj != nullptr;
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
            void  Remove(const GuiString& sz)
            {
               erase(sz);
            }
         };

         /// <summary>Provides constant access to the objects within a library</summary>
         typedef MapIterator<const ScriptObject, LookupCollection, LookupCollection::const_iterator> const_iterator;
	  
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
         const_iterator  begin() const;
         bool            Contains(const GuiString& obj) const;
         bool            Contains(ScriptObjectGroup grp, UINT id) const;
         ScriptObjectRef Find(ScriptObjectGroup grp, UINT id) const;
         ScriptObjectRef Find(KnownPage page, UINT id) const;
         ScriptObjectRef Find(DataType type, UINT id) const;
         ScriptObjectRef Find(const GuiString& sz) const;
         const_iterator  end() const;
         UINT            GetCount() const;

      private:
         ObjectCollection  GetSpecialCases() const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);
         ScriptObjectArray Query(const GuiString& str) const;
         bool  TryFind(ScriptObjectGroup grp, UINT id, const ScriptObject* &obj) const;
         bool  TryFind(const GuiString& name, const ScriptObject* &obj) const;
      
      protected:
         bool  InsertConflicts(ScriptObject a, ScriptObject b);
         bool  MangleConflicts(ScriptObject a, ScriptObject b);
         UINT  Populate(WorkerData* data);

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
