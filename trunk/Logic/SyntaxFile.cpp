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

      /// <summary>Create syntax file</summary>
      /// <param name="title">The title.</param>
      /// <param name="version">The version.</param>
      SyntaxFile::SyntaxFile(const wstring& title, const wstring& version) : Title(title), Version(version)
      {
      }


      SyntaxFile::~SyntaxFile()
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------


		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

