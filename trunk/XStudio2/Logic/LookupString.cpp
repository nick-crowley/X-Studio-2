#include "stdafx.h"
#include "LookupString.h"

namespace Logic
{
   namespace Language
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      LookupString::LookupString() : ID(0), Page(KnownPage::COMMAND_SYNTAX)
      {
      }

      LookupString::LookupString(UINT id, KnownPage p) : ID(id), Page(p)
      {
      }


      LookupString::~LookupString()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Queries whether string ID is defined</summary>
      /// <returns></returns>
      bool  LookupString::Defined() const
      {
         return ID != 0 && ID != 9999;
      }

      /// <summary>Queries whether a string is present in the string library</summary>
      /// <returns></returns>
      bool  LookupString::Exists() const
      {
         return StringLib.Contains(Page, ID);
      }

      /// <summary>Finds the string text in the string library</summary>
      /// <returns></returns>
      /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
      wstring  LookupString::GetText() const
      {
         return StringLib.Resolve(Page, ID);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

