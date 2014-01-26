#pragma once

#include "Common.h"
#include "TFile.hpp"

namespace Logic
{
   namespace Types
   {
      /// <summary></summary>
      class TFilenameReader
      {
      public:
         TFilenameReader(Path p) : Valid(true), Type(MainType::Unknown)
         {
            // Get lowercase filename without extension
            wstring name = GuiString(p.RemoveExtension().FileName).ToLower();

            // Identify file
            if (name == L"tlasers")
               Type = MainType::Laser;
            /*else if (name == L"tmissiles")
               Type = MainType::Missile;*/
            /*else if (name == L"tships")
               Type = MainType::Ship;*/
            else if (name == L"tdocks")
               Type = MainType::Dock;
            else if (name == L"tfactories")
               Type = MainType::Factory;
            else if (name == L"twaret")
               Type = MainType::TechWare;
            else if (name == L"twareb")
               Type = MainType::BioWare;
            else if (name == L"twaree")
               Type = MainType::EnergyWare;
            else if (name == L"twarem")
               Type = MainType::MineralWare;
            else if (name == L"twaren")
               Type = MainType::NaturalWare;
            else if (name == L"twaref")
               Type = MainType::FoodWare;
            else
               Valid = false;
         }

         bool      Valid;
         MainType  Type;
      };
      
      /// <summary></summary>
      class GameObject
      {
         // ------------------------ TYPES --------------------------
      private:
         enum class KnownID : UINT { Undefined=0, UndefinedName=9999, UndefinedRace=34000, UnknownObject1=9001, UnknownObject2=9041, UnknownObject3=17206 };

         // --------------------- CONSTRUCTION ----------------------
      public:
         GameObject(UINT id, TObject* obj) : Type(obj->Type), ID(id), Text(obj->FullName)
         {}
      private:
         GameObject(const GameObject& r, const wstring& txt) : Type(r.Type), ID(r.ID), Text(txt)
         {}

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Appends an object ID to name</summary>
         /// <param name="id">The id</param>
         /// <returns>New game object with ID appended</returns>
         GameObject operator+(const wstring& id)
         {
            return GameObject(*this, Text+GuiString(L" (%s)", id));
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const MainType Type;
         const UINT     ID;
         const wstring  Text;
      };

      /// <summary></summary>
      class GameObjectLibrary
      {
		   // ------------------------ TYPES --------------------------
      private:
	      /// <summary>Defines a {MainType,ID} pair</summary>
         class ObjectID
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ObjectID(MainType t, UINT id) : Type(t), ID(id)
            {}

            // ---------------------- ACCESSORS ------------------------	
         public:
            bool operator==(const ObjectID& r) const {
               return Type == r.Type && ID == r.ID;
            }
            bool operator<(const ObjectID& r) const {
               return Type < r.Type || (Type == r.Type && ID < r.ID);
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            const MainType Type;
            const UINT     ID;
         };

         /// <summary>Collection of game objects sorted by ID</summary>
         class ObjectCollection : public map<ObjectID, GameObject, less<ObjectID>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            /// <summary>Finds a game object by ID</summary>
            /// <param name="type">The type.</param>
            /// <param name="id">The ID.</param>
            /// <returns></returns>
            /// <exception cref="Logic::StringNotFoundException">Object not found</exception>
            GameObject  Find(MainType type, UINT id) const
            {
               const_iterator it;
               // Lookup and return string
               if ((it = find(ObjectID(type, id))) != end())
                  return it->second;

               // Error: Not found
               throw StringNotFoundException(HERE, (UINT)type, id);
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Creates and adds a new object</summary>
            /// <param name="id">The id of the object</param>
            /// <param name="obj">The TObject to create the GameObject from</param>
            /// <returns>True if successful, false if MainType/ID combination already present</returns>
            /*bool  Add(UINT id, const TObject* obj)
            {
               return insert( value_type(ObjectID(obj->Type,id), GameObject(obj->Type, id, obj->name.Text)) ).second;
            }*/
            bool  Add(const GameObject& obj)
            {
               return insert(value_type(ObjectID(obj.Type, obj.ID), obj)).second;
            }
         };

      public:
         /// <summary>Collection of game objects sorted by text</summary>
         class LookupCollection : public map<wstring, GameObject, less<wstring>>
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
            /// <exception cref="Logic::GameObjectNotFoundException">Object not found</exception>
            GameObject  Find(const wstring& sz) const
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
            bool  Add(const GameObject& obj)
            {
               return insert(value_type(obj.Text, obj)).second;
            }

            /// <summary>Creates a new game object and adds to the collection</summary>
            /// <param name="id">The object id</param>
            /// <param name="obj">The object to create a game object from</param>
            /// <returns>True if successful, false if key already present</returns>
            /*bool  Add(UINT id, const TObject* obj)
            {
               return insert( value_type(obj->name.Text, GameObject(obj->Type, id, obj->name.Text)) ).second;
            }*/

            /// <summary>Removes an object from the collection</summary>
            /// <param name="sz">The text</param>
            void  Remove(const wstring& sz)
            {
               erase(sz);
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         GameObjectLibrary();
         virtual ~GameObjectLibrary();
		 
		   NO_COPY(GameObjectLibrary);	// No copy semantics
		   NO_MOVE(GameObjectLibrary);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(const XFileSystem& vfs, WorkerData* data);
         TObject*  Find(MainType main, UINT subtype);

      protected:
         UINT  PopulateObjects(WorkerData* data);

         // -------------------- REPRESENTATION ---------------------
      public:
         static GameObjectLibrary  Instance;

         const LookupCollection& Content;
      private:
         vector<TFilePtr>  Files;
         ObjectCollection  Objects;
         LookupCollection  Lookup;
      };
   
      // Access to Game object library singleton
      #define GameObjectLib  GameObjectLibrary::Instance
   }
}

using namespace Logic::Types;
