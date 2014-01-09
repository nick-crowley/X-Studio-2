#include "stdafx.h"
#include "ParameterSyntax.h"

namespace Library
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ParameterSyntax::ParameterSyntax(Declaration& d)
         : Type(d.Type), PhysicalIndex(d.PhysicalIndex), DisplayIndex(d.DisplayIndex), Optional(d.Optional),
           PageID(d.PageID), StringID(d.StringID), ScriptName(d.ScriptName)
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
