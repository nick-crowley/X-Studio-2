#pragma once

#include "Common.h"

namespace Logic
{
   namespace Language
   {

      /// <summary></summary>
      class DescriptionLibrary
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         DescriptionLibrary();
         virtual ~DescriptionLibrary();

         DEFAULT_COPY(DescriptionLibrary);	// Default copy semantics
         DEFAULT_MOVE(DescriptionLibrary);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static DescriptionLibrary  Instance;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

      /// <summary>Description library singleton access</summary>
      #define DescriptionLib     DescriptionLibrary::Instance
   }

}

using namespace Logic::Language;
