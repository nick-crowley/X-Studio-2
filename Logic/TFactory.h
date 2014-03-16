#pragma once


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
         TFactory(MainType t) : TDock(MainType::Factory)
         {}
         TFactory() : TDock(MainType::Factory)
         {}
         virtual ~TFactory()
         {}

         DEFAULT_COPY(TFactory);	// Default copy semantics
         DEFAULT_MOVE(TFactory);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      protected:
         wstring  GetInternalName() const
         {
            return factorySize.Defined() && factorySize.Exists() ? TDock::GetInternalName()+L" "+factorySize.Text : TDock::GetInternalName();
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         StationSizeLookup  factorySize;
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
            obj.flyBySound         = ReadInt(L"flyBySound");
		      obj.dockingDistance    = ReadFloat(L"dockingDistance");
		      obj.randezvousDistance = ReadFloat(L"randezvousDistance");
		      obj.soundVolume        = ReadInt(L"soundVolume");
		      obj.modelScene         = ReadString(L"modelScene");
		      obj.internalScene      = ReadString(L"internalScene");
		      obj.race.ID            = ReadInt(L"race");
		      obj.effectExplosion    = ReadInt(L"effectExplosion");
		      obj.bodyExplosionDefinition = ReadInt(L"bodyExplosionDefinition");
		      obj.shieldRechargeRate = ReadInt(L"shieldRechargeRate");

            obj.factorySize.ID     = ReadInt(L"factorySize");
		      obj.icon               = ReadString(L"hudIcon");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
