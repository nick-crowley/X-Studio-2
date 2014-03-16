#pragma once


#include "TObject.h"
#include "TFileReader.hpp"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class LogicExport TLaser : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TLaser(MainType t) : TObject(MainType::Laser)
         {}
         TLaser() : TObject(MainType::Laser)
         {}
         virtual ~TLaser()
         {}

         DEFAULT_COPY(TLaser);	// Default copy semantics
         DEFAULT_MOVE(TLaser);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         int rateOfFire;
	      int sound;
	      int projectileIndex;
	      int energy;
	      float chargeRate;
	      wstring icon;
      };

      /// <summary></summary>
      class LogicExport TLaserReader : public TFileReader<TLaser>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TLaser reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TLaserReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TLaserReader()
         {}

         NO_COPY(TLaserReader);	// No copy semantics
		   NO_MOVE(TLaserReader);	// No move semantics

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
	      void  ReadObject(TLaser& obj, GameVersion ver)
         {
            obj.rateOfFire      = ReadInt(L"rateOfFire");
		      obj.sound           = ReadInt(L"sound");
		      obj.projectileIndex = ReadInt(L"projectileIndex");
		      obj.energy          = ReadInt(L"energy");
		      obj.chargeRate      = ReadFloat(L"chargeRate");
		      obj.icon            = ReadString(L"hudIcon");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
