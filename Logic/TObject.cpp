#include "stdafx.h"
#include "TObject.h"
#include "ScriptObjectLibrary.h"

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
      
      /// <summary>Get main type string</summary>
      LogicExport GuiString GetString(const MainType& m)
      {
         return GuiString(IDS_FIRST_MAIN_TYPE + (UINT)m);
      }

      /// <summary>Increment main type</summary>
      LogicExport MainType operator++(MainType& m, int)
      {
         MainType ret = m;
         return (++m, ret);
      }

      /// <summary>Increment main type</summary>
      LogicExport MainType& operator++(MainType& m)
      {
         return m = (MainType)((UINT)m + 1);
      }

      /// <summary>Operator on main type as int</summary>
      LogicExport UINT operator-(const MainType& a, const MainType& b)
      {
         return (UINT)a - (UINT)b;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Gets the object display name.</summary>
      /// <returns></returns>
      wstring TObject::GetFullName() const
      {
         return !name.Defined() ? L"*** UNDEFINED ***"
              : !name.Exists()  ? L"*** MISSING ***"
                                : GetInternalName();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Gets the object display name.</summary>
      /// <returns></returns>
      wstring TObject::GetInternalName() const
      {
         return name.Text;
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
