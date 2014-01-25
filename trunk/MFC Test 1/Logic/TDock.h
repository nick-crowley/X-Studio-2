#pragma once

#include "Common.h"
#include "TObject.h"
#include "TFileReader.hpp"

namespace Logic
{
   namespace Types
   {
      /// <summary>Any dock</summary>
      class TDock : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TDock(MainType t) : TObject(MainType::Dock)
         {}
         TDock() : TObject(MainType::Dock)
         {}
         virtual ~TDock()
         {}

         DEFAULT_COPY(TDock);	// Default copy semantics
         DEFAULT_MOVE(TDock);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         int              flyBySound;
	      float            dockingDistance;
	      float            randezvousDistance;
	      int              soundVolume;
	      wstring          modelScene;
         LookupString     race;
	      wstring          internalScene;
	      int              effectExplosion;
	      int              bodyExplosionDefinition;
	      int              shieldRechargeRate;
	      wstring          icon;
      };

      /// <summary>TDock file</summary>
      typedef TFile<TDock>  TDockFile;

      /// <summary>TDock file reader</summary>
      class TDockReader : public TFileReader<TDock>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TDock reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TDockReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TDockReader()
         {}

         NO_COPY(TDockReader);	// No copy semantics
		   NO_MOVE(TDockReader);	// No move semantics

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
	      void  ReadObject(TDock& obj, GameVersion ver)
         {
            obj.flyBySound         = ReadInt(L"flyBySound");
		      obj.dockingDistance    = ReadFloat(L"dockingDistance");
		      obj.randezvousDistance = ReadFloat(L"randezvousDistance");
		      obj.soundVolume        = ReadInt(L"soundVolume");
		      obj.modelScene         = ReadString(L"modelScene");
		      obj.internalScene      = ReadString(L"internalScene");
            obj.race               = LookupString(ReadInt(L"race"), KnownPage::RACES);
		      obj.effectExplosion    = ReadInt(L"effectExplosion");
		      obj.bodyExplosionDefinition = ReadInt(L"bodyExplosionDefinition");
		      obj.shieldRechargeRate = ReadInt(L"shieldRechargeRate");
		      obj.icon               = ReadString(L"hudIcon");
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
