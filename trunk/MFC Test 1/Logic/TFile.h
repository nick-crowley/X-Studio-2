#pragma once

#include "Common.h"
#include "TObject.h"

namespace Logic
{
   namespace Types
   {
      typedef shared_ptr<TObject>  TObjectPtr;

      /// <summary></summary>
      class TFile
      {
         // ------------------------ TYPES --------------------------
      public:
         

         // --------------------- CONSTRUCTION ----------------------

      public:
         TFile(UINT count);
         virtual ~TFile();

         DEFAULT_COPY(TFile);	// Default copy semantics
         DEFAULT_MOVE(TFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         vector<TObjectPtr>  Objects;
      };
   
   }
}

using namespace Logic::Types;
