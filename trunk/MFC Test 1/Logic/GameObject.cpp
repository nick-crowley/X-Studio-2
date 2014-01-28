#include "stdafx.h"
#include "GameObject.h"

namespace Logic
{
   namespace Types
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      GameObject::GameObject(UINT subtype, const TObject* obj) : Type(obj->Type), SubType(subtype), ID(obj->id), Name(obj->FullName)
      {
      }

      GameObject::GameObject(const GameObject& r, const wstring& txt) : Type(r.Type), SubType(r.SubType), ID(r.ID), Name(txt)
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

