#include "stdafx.h"
#include "TFile.h"

namespace Logic
{
   namespace Types
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      TFile::TFile(UINT count)
      {
         Objects.reserve(count);
      }


      TFile::~TFile()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
