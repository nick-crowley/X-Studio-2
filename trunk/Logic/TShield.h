#pragma once


#include "TObject.h"
#include "TFileReader.hpp"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class TShield : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TShield(MainType t) : TObject(MainType::Shield)
         {}
         TShield() : TObject(MainType::Shield)
         {}
         virtual ~TShield()
         {}

         DEFAULT_COPY(TShield);	// Default copy semantics
         DEFAULT_MOVE(TShield);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         int capacity;
	      int powerInput;
	      int hitEffect;
	      float efficiency;
      };

      /// <summary></summary>
      class TShieldReader : public TFileReader<TShield>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TShield reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TShieldReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TShieldReader()
         {}

         NO_COPY(TShieldReader);	// No copy semantics
		   NO_MOVE(TShieldReader);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

      protected:
         /// <summary>Reads the properties unique to this type</summary>
         /// <param name="obj">Object to read into</param>
         /// <param name="ver">File version</param>
         /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
	      void  ReadObject(TShield& obj, GameVersion ver)
         {
            obj.powerInput = ReadInt(L"powerDrain");
		      obj.capacity   = ReadInt(L"strength");
		      obj.hitEffect  = ReadInt(L"hitEffect");
		      obj.efficiency = ReadFloat(L"efficiency");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
