#include "stdafx.h"
#include "TObject.h"

namespace Logic
{
   namespace Types
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      TObject::TObject(MainType t) 
         : Type(t), name(0, KnownPage::NAMES_DESCRIPTIONS), description(0, KnownPage::NAMES_DESCRIPTIONS), rotation(0.0f,0.0f,0.0f)
      {
      }


      TObject::~TObject()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      wstring TObject::GetFullName() const
      {
         return !name.Defined() ? L"*** UNDEFINED ***"
              : !name.Exists()  ? L"*** MISSING ***"
                                : name.Text;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
