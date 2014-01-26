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
      
      const wchar* GetString(const MainType& m)
      {
         static const wchar* str[] = 
         {
            L"", L"", L"", L"", L"", L"Dock", L"Factory", L"Ship", L"Laser",
            L"Shield", L"Missile", L"Energy Ware", L"Natural Ware", L"Bio Ware", L"Food Ware", 
            L"Mineral Ware", L"TechWare"
         };

         return str[(UINT)m];
      }

      MainType operator++(MainType& m, int)
      {
         MainType ret = m;
         return (++m, ret);
      }

      MainType& operator++(MainType& m)
      {
         return m = (MainType)((UINT)m + 1);
      }

      UINT operator-(const MainType& a, const MainType& b)
      {
         return (UINT)a - (UINT)b;
      }

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
