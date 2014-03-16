#pragma once


#include "StringLibrary.h"

namespace Logic
{
   namespace Language
   {

      /// <summary></summary>
      class LookupString
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         LookupString();
         LookupString(UINT id, KnownPage p);
         virtual ~LookupString();

         DEFAULT_COPY(LookupString);	// Default copy semantics
         DEFAULT_MOVE(LookupString);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(wstring,Text,GetText);

         // ---------------------- ACCESSORS ------------------------			

         virtual bool    Defined() const;
         virtual bool    Exists() const;
         virtual wstring GetText() const;

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         UINT      ID;
         KnownPage Page;
      };
   }
}

using namespace Logic::Language;