#include "stdafx.h"
#include "GameObject.h"

namespace Logic
{
   namespace Types
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      GameObject::GameObject(UINT subtype, const TObject* obj) 
         : Type(obj->Type), SubType(subtype), ID(obj->id), Name(obj->FullName)
      {
      }

      GameObject::GameObject(const GameObject& r, const GuiString& txt) 
         : Type(r.Type), SubType(r.SubType), ID(r.ID), Name(txt), Description(r.Description)
      {
      }

      GameObject::GameObject(GameObject&& r)  
         : Type(r.Type), SubType(r.SubType), ID(move(r.ID)), Name(move(r.Name)), Description(move(r.Description))
      {
      }

      GameObject::~GameObject()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

