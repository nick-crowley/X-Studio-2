#include "stdafx.h"
#include "SyntaxFile.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      SyntaxFile::SyntaxFile()
      {
      }


      SyntaxFile::~SyntaxFile()
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      bool SyntaxCollection::Add(CommandSyntax&& s) 
      { 
         insert(value_type(s.ID, s)); 
         return true; 
      }

      void  SyntaxCollection::Merge(SyntaxFile&& f) 
      { 
         for (value_type pair : f.Commands)
            insert(pair);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

