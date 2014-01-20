#pragma once

#include "Common.h"
#include "TObject.h"

namespace Logic
{
   namespace Types
   {
      typedef shared_ptr<TObject>  TObjectPtr;

      /// <summary></summary>
      template <typename OBJ>
      class TFile
      {
         // ------------------------ TYPES --------------------------
      public:
         

         // --------------------- CONSTRUCTION ----------------------

      public:
         TFile(UINT count)
         {
            Objects.reserve(count);
         }
         virtual ~TFile()
         {}

         DEFAULT_COPY(TFile);	// Default copy semantics
         DEFAULT_MOVE(TFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         vector<OBJ>  Objects;
      };
   
   }
}

using namespace Logic::Types;
