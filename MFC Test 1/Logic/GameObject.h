#pragma once

#include "Common.h"
#include "TObject.h"

namespace Logic
{
   namespace Types
   {

      /// <summary></summary>
      class GameObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      private:
         GameObject(const GameObject& r, const wstring& txt);

      public:
         GameObject(UINT subtype, const TObject* obj);
         virtual ~GameObject();
      
         //DEFAULT_COPY(GameObject);	// Default copy semantics
         //DEFAULT_MOVE(GameObject);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Appends an object id to name</summary>
         /// <param name="id">object id</param>
         /// <returns>New game object with id appended</returns>
         GameObject operator+(const wstring& id)
         {
            return GameObject(*this, Name+GuiString(L" (%s)", id));
         }
         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         const MainType Type;
         const UINT     SubType;
         const wstring  ID, 
                        Name, 
                        Description;
      };

      /// <summary>Vector of game objects</summary>
      typedef vector<GameObject> GameObjectArray;
   }
}

using namespace Logic::Types;
