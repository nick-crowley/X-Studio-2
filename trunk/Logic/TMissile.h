#pragma once


#include "TObject.h"
#include "TFileReader.hpp"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class LogicExport TMissile : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TMissile(MainType t) : TObject(MainType::Missile)
         {}
         TMissile() : TObject(MainType::Missile)
         {}
         virtual ~TMissile()
         {}

         DEFAULT_COPY(TMissile);	// Default copy semantics
         DEFAULT_MOVE(TMissile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         int lifetime;
	      int speed;

	      int effectTrail;
	      int effectImpact;
	      int effectLaunch;
	      int engineTrail;

	      int soundAmbient;
	      int soundVolumeMin;
	      int soundVolumeMax;

         int acceleration;
	      int soundLaunch;
	      int collisionType;
	      int damage;
	      int blastRadius;
	      int effectTrial;
	      int effectGlow;
	      int soundExplosion;
	      int effectExplosion;
	      int fireRate;
	      wstring icon;
	      wstring sceneFile;

	      int flags;
      };

      /// <summary></summary>
      class LogicExport TMissileReader : public TFileReader<TMissile>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TMissile reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TMissileReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TMissileReader()
         {}

         NO_COPY(TMissileReader);	// No copy semantics
		   NO_MOVE(TMissileReader);	// No move semantics

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
	      void  ReadObject(TMissile& obj, GameVersion ver)
         {
            obj.speed           = ReadInt(L"speed");
	         obj.acceleration    = ReadInt(L"acceleration");
	         obj.soundLaunch     = ReadInt(L"soundLaunch");
	         obj.soundAmbient    = ReadInt(L"soundAmbient");
	         obj.collisionType   = ReadInt(L"collisionType");
	         obj.damage          = ReadInt(L"damage");
	         obj.blastRadius     = ReadInt(L"blastRadius");
	         obj.lifetime        = ReadInt(L"lifetime");
	         obj.effectTrial     = ReadInt(L"effectTrial");
	         obj.effectGlow      = ReadInt(L"effectGlow");
	         obj.soundExplosion  = ReadInt(L"soundExplosion");
	         obj.soundVolumeMin  = ReadInt(L"soundVolumeMin");
	         obj.soundVolumeMax  = ReadInt(L"soundVolumeMax");
	         obj.effectImpact    = ReadInt(L"effectImpact");
	         obj.effectExplosion = ReadInt(L"effectExplosion");
	         obj.engineTrail     = ReadInt(L"engineTrial");
	         obj.flags           = ReadInt(L"missileFlags");
	         obj.fireRate        = ReadInt(L"fireRate");
	         obj.icon            = ReadString(L"hudIcon");
	         obj.sceneFile       = ReadString(L"sceneFile");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
