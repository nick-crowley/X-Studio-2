#pragma once


#include "TObject.h"
#include "TFileReader.hpp"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class TWare : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TWare(MainType t) : TObject(t)
         {}
         virtual ~TWare()
         {}

         DEFAULT_COPY(TWare);	// Default copy semantics
         DEFAULT_MOVE(TWare);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
      };

      /// <summary></summary>
      class TWareReader : public TFileReader<TWare>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TWare reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TWareReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TWareReader()
         {}

         NO_COPY(TWareReader);	// No copy semantics
		   NO_MOVE(TWareReader);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

      protected:
         /// <summary>This type has no unique properties</summary>
         /// <param name="o">Object to read into</param>
         /// <param name="ver">File version</param>
	      void  ReadObject(TWare& o, GameVersion ver)
         {}

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
