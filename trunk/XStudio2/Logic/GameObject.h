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
         GameObject(const GameObject& r, const GuiString& txt);

      public:
         GameObject(UINT subtype, const TObject* obj);
         GameObject(GameObject&& r);
         virtual ~GameObject();
      
         //DEFAULT_COPY(GameObject);	// Default copy semantics
         //DEFAULT_MOVE(GameObject);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(GuiString,DisplayText,GetDisplayText);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name formatted for display</summary>
         GuiString  GetDisplayText() const
         {
            return GuiString(L"{%s}", Name.c_str());
         }

         /// <summary>Appends an object id to name</summary>
         /// <param name="id">object id</param>
         /// <returns>New game object with id appended</returns>
         GameObject operator+(const GuiString& id)
         {
            return GameObject(*this, Name+GuiString(L" (%s)", id));
         }
         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         const MainType  Type;
         const UINT      SubType;
         const GuiString ID, 
                         Name, 
                         Description;
      };

      /// <summary>Vector of game objects</summary>
      typedef vector<GameObject> GameObjectArray;
   }
}

using namespace Logic::Types;
