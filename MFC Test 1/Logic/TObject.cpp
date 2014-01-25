#include "stdafx.h"
#include "TObject.h"

namespace Logic
{
   namespace Types
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      TObject::TObject(MainType t) : Type(t), rotation(0.0f,0.0f,0.0f)
      {
      }


      TObject::~TObject()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
