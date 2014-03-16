#pragma once


#include "TObject.h"

namespace Logic
{
   namespace Types
   {
      
      /// <summary>Occurs when a MSCI game object is unrecognised</summary>
      class GameObjectNotFoundException : public ExceptionBase
      {
      public:
         /// <summary>Create a GameObjectNotFoundException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="name">object name</param>
         GameObjectNotFoundException(wstring  src, wstring name) 
            : ExceptionBase(src, VString(L"Cannot find game object '%s'", name.c_str()))
         {}

         /// <summary>Create a GameObjectNotFoundException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="maintype">maintype</param>
         /// <param name="subtype">subtype</param>
         GameObjectNotFoundException(wstring  src, MainType maintype, UINT subtype) 
            : ExceptionBase(src, VString(L"Cannot find %s with id %d", GetString(maintype).c_str(), subtype))
         {}

         /// <summary>Create a GameObjectNotFoundException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="maintype">maintype</param>
         /// <param name="subtype">subtype</param>
         GameObjectNotFoundException(wstring  src, MainType maintype) 
            : ExceptionBase(src, VString(L"%s game objects have not been loaded", GetString(maintype).c_str()))
         {}
      };

      class GameObject;

      /// <summary>Constant reference to a game object</summary>
      typedef const GameObject&  GameObjectRef;

      /// <summary>Represents a ware, ship or station</summary>
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

         /// <summary>Encodes a ware</summary>
         /// <returns></returns>
         static UINT  GetEncodedValue(MainType main, UINT sub) 
         {
            return MAKELONG(sub, main);
         }

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(GuiString,DisplayText,GetDisplayText);
         PROPERTY_GET(UINT,EncodedValue,GetEncodedValue);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name formatted for display</summary>
         GuiString  GetDisplayText() const
         {
            return VString(L"{%s}", Name.c_str());
         }

         /// <summary>Gets the encoded value.</summary>
         /// <returns></returns>
         UINT  GetEncodedValue() const
         {
            return MAKELONG(SubType, Type);
         }

         /// <summary>Appends an object id to name</summary>
         /// <param name="id">object id</param>
         /// <returns>New game object with id appended</returns>
         GameObject operator+(const GuiString& id)
         {
            return GameObject(*this, Name+VString(L" (%s)", id.c_str()));
         }
         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         const MainType  Type;
         const UINT      SubType;
         const GuiString ID,     // object ID. eg. SS_P_BUSTER
                         Name, 
                         Description;
      };

      /// <summary>Vector of game object pointers</summary>
      typedef vector<const GameObject*> GameObjectArray;
   }
}

using namespace Logic::Types;
