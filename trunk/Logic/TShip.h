#pragma once


#include "TObject.h"
#include "TFileReader.hpp"
#include "RaceLookup.h"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class LogicExport TShip : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:
         //const int MAX_TURRET_COUNT = 7;

      public:
         enum class TurretPosition : UINT { cockpit = 0, front = 1, rear, left, right, top, bottom };

         /// <summary>Turret weapon</summary>
	      class LogicExport Weapon 
	      {
            // --------------------- CONSTRUCTION ----------------------
         public:
            Weapon()
            {}

            // -------------------- REPRESENTATION ---------------------

            int     numLasers;
		      int     sceneNode1;
            int     sceneNode2;
            wstring modelName1;
		      wstring modelName2;
	      };

         /// <summary>Ship turret</summary>
	      class LogicExport Turret 
	      {
            // --------------------- CONSTRUCTION ----------------------
         public:
		      Turret() 
            {}
		      Turret(int index, TurretPosition pos) : cockpitIndex(index), position(pos)
            {}

            // -------------------- REPRESENTATION ---------------------

            bool           special;     // Special turret-like record used in some drone ships
		      int            cockpitIndex;
		      TurretPosition position;
		      wstring        modelFile;
		      int            sceneNodeIndex;
		      vector<Weapon> weapons;
	      };
   	
         
         // --------------------- CONSTRUCTION ----------------------

      public:
         TShip(MainType t) : TObject(MainType::Ship)
         {
            turrets.push_back(Turret(0, TurretPosition::cockpit));      // Add cockpit turret
         }
         TShip() : TObject(MainType::Ship)
         {
            turrets.push_back(Turret(0, TurretPosition::cockpit));   // Add cockpit turret
         }

         DEFAULT_COPY(TShip);	// Default copy semantics
         DEFAULT_MOVE(TShip);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      protected:
         wstring  GetInternalName() const
         {
            GuiString strRace, strName;

            // Pirate: special case
            if (IsPirate())
               strRace = L"Pirate";

            // Lookup race
            else if (race.Defined() && race.Exists())
               strRace = race.Text;

            // Prepend race, Except when name already contains race in some capacity
            strName = TObject::GetInternalName();
            strName = (!strRace.length() || strName.Contains(strRace, true) ? strName : strRace + L" " + strName);

            // Append variable, if any
            return variation.Defined() && variation.Exists() ? (strName + L" ") + variation.Text : strName;
         }

         /// <summary>Determines whether ship race is pirate</summary>
         /// <returns></returns>
         bool IsPirate() const
         {
            return id.Right(2) == L"_P" || id.Right(4) == L"_P_1" || id.Right(4) == L"_P_2" || id.Right(4) == L"_P_3";
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
	      int      speed;
	      int      acceleration;
	      int      engineSound;
	      int      averageReactionDelay;
	      int      engineEffect;
	      int      engineGlowEffect;
	      int      power;
	      int      soundVolumeMin;
	      int      soundVolumeMax;
	      wstring  modelScene;
	      wstring  cockpitScene;

	      UINT     laserMask;  // uint because it's bitmask with full 32 bit range
	      int      laserEnergy;
	      float    laserRechargeRate;
	      int      shieldType;
	      int      maxNumOfShileds;
	      UINT     missileMask;
	      int      maxMissileCountNPC;
	      int      extensionsSpeed;
	      int      extensionsSteering;
	      int      cargoMin;
	      int      cargoMax;
	      int      wareListID;
   	
	      vector<Turret> turrets;
   	
	      int         numDockingSlots;
	      WareSize    maxCargoSize;
         RaceLookup  race;
	      int         hullStrength;
	      int         explosionDefinition;
	      int         explosionBodyDefinition;
	      int         particleEmitter;
	      int         rotationAcceleration;

	      wstring         shipClass;      // Ship class LogicExport such as M1, M3
         VariationLookup variation;
      };

      /// <summary></summary>
      class LogicExport TShipReader : public TFileReader<TShip>
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TShip reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TShipReader(StreamPtr in) : TFileReader(in)
         {}
         virtual ~TShipReader()
         {}

         NO_COPY(TShipReader);	// No copy semantics
		   NO_MOVE(TShipReader);	// No move semantics

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
	      void  ReadObject(TShip& obj, GameVersion ver);

      private:
         /// <summary>Loads the gun groups.</summary>
         /// <param name="ship">The ship.</param>
         void LoadGunGroups(TShip ship);

         /// <summary>Loads the turrets.</summary>
         /// <param name="ship">The ship.</param>
         void LoadTurrets(TShip ship);

         /// <summary>Loads the turret weapons.</summary>
         /// <param name="turret">The turret.</param>
         /// <param name="count">The count.</param>
         void LoadWeapons(TShip::Turret turret, UINT count);

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
