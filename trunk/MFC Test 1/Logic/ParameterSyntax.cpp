#include "stdafx.h"
#include "ParameterSyntax.h"

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

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
