#include "stdafx.h"
#include "ParameterSyntax.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ParameterSyntax::ParameterSyntax(Declaration& d)
         : Type(d.Type), PhysicalIndex(d.PhysicalIndex), DisplayIndex(d.DisplayIndex), Ordinal(d.Ordinal), Usage(d.Usage)
      {
      }


      ParameterSyntax::~ParameterSyntax()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get parameter type string</summary>
      GuiString  GetString(ParameterType t)
      {
         return StringLib.Contains(KnownPage::PARAMETER_TYPES, (UINT)t) ? StringLib.Find(KnownPage::PARAMETER_TYPES, (UINT)t).Text 
                                                                        : GuiString(L"Missing (%d)", (UINT)t);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
