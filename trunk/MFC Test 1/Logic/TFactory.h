#pragma once

#include "Common.h"
#include "TDock.h"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class TFactory : public TDock
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TFactory()
         {}
         virtual ~TFactory()
         {}

         DEFAULT_COPY(TFactory);	// Default copy semantics
         DEFAULT_MOVE(TFactory);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         int  factorySize;
      };

      /// <summary></summary>
      class TFactoryReader : public TFileReader<TFactory>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TFactory reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TFactoryReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TFactoryReader()
         {}

         NO_COPY(TFactoryReader);	// No copy semantics
		   NO_MOVE(TFactoryReader);	// No move semantics

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
	      void  ReadObject(TFactory& obj, GameVersion ver)
         {
            obj.flyBySound         = ReadInt("flyBySound");
		      obj.dockingDistance    = ReadFloat("dockingDistance");
		      obj.randezvousDistance = ReadFloat("randezvousDistance");
		      obj.soundVolume        = ReadInt("soundVolume");
		      obj.modelScene         = ReadString("modelScene");
		      obj.internalScene      = ReadString("internalScene");
		      //obj.race               = new XStringResource(ReadInt("race"), KnownPage.Races);
            obj.race               = ReadInt("race");
		      obj.effectExplosion    = ReadInt("effectExplosion");
		      obj.bodyExplosionDefinition = ReadInt("bodyExplosionDefinition");
		      obj.shieldRechargeRate = ReadInt("shieldRechargeRate");

            obj.factorySize        = ReadInt("factorySize");
		      obj.icon               = ReadString("hudIcon");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
