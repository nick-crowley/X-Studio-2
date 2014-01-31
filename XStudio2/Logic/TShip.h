#pragma once

#include "Common.h"
#include "TObject.h"
#include "TFileReader.hpp"
#include "RaceLookup.h"

namespace Logic
{
   namespace Types
   {
      
      /// <summary></summary>
      class TShip : public TObject
      {
         // ------------------------ TYPES --------------------------
      private:
         const int MAX_TURRET_COUNT = 7;

      public:
         enum class TurretPosition : UINT { cockpit = 0, front = 1, rear, left, right, top, bottom }

      public:
         /// <summary>Ship turret</summary>
	      class Turret 
	      {
		      /// <summary>Special turret-like record used in some drone ships.</summary>
		      bool special;
   		
		      int cockpitIndex;
		      TurretPosition position;

		      string modelFile;
		      int sceneNodeIndex;
   		
		      List<Weapon> weapons = new List<Weapon>();
   		
            // -------------------------------- CONSTRUCTION --------------------------------

		      Turret() {}

		      Turret(int cockpitIndex, TurretPosition position) 
		      { 
			      this.cockpitIndex = cockpitIndex; 
			      this.position = position; 
               modelFile = string.Empty;
		      }

            // --------------------------------- INTERFACES ---------------------------------

		      object ICloneable.Clone() { return Clone(); }
   		
            // ----------------------------- PROPERTIES ------------------------------

            // ------------------------------- METHODS -------------------------------

		      Turret Clone()
		      {
			      Turret copy = (Turret)MemberwiseClone();

               // Clone weapons
   			   copy.weapons = weapons.Select(u => u.Clone()).ToList();
			      return copy;
		      }
	      } 
   	
         /// <summary>Turret weapon</summary>
	      public class Weapon : ICloneable
	      {
		      public int numLasers;
		      public string modelName1;
		      public int sceneNode1;
		      public string modelName2;
		      public int sceneNode2;

            // -------------------------------- CONSTRUCTION --------------------------------

            public Weapon()
            {
               modelName1 = modelName2 = string.Empty;
            }

            // --------------------------------- INTERFACES ---------------------------------

		      object ICloneable.Clone() 
            {
			      return Clone();
		      }

            // ----------------------------- PUBLIC PROPERTIES ------------------------------

            // ------------------------------- PUBLIC METHODS -------------------------------

            public Weapon Clone()
            {
               return (Weapon)MemberwiseClone();
            }
	      } 
         // --------------------- CONSTRUCTION ----------------------

      public:
         TShip(MainType t) : TObject(MainType::Ship)
         {}
         TShip() : TObject(MainType::Ship)
         {}
         virtual ~TShip()
         {}

         DEFAULT_COPY(TShip);	// Default copy semantics
         DEFAULT_MOVE(TShip);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
	      int speed;
	      int acceleration;
	      int engineSound;
	      int averageReactionDelay;
	      int engineEffect;
	      int engineGlowEffect;
	      int power;
	      int soundVolumeMin;
	      int soundVolumeMax;
	      wstring modelScene;
	      wstring cockpitScene;

	      UINT laserMask; // uint because it's bitmask with full 32 bit range
	      int laserEnergy;
	      float laserRechargeRate;
	      int shieldType;
	      int maxNumOfShileds;
	      UINT missileMask;
	      int maxMissileCountNPC;
	      int extensionsSpeed;
	      int extensionsSteering;
	      int cargoMin;
	      int cargoMax;
	      int wareListID;
   	
	      List<TShip.Turret> turrets = new List<Turret>(MAX_TURRET_COUNT) { new Turret(0, TurretPosition.cockpit) }; //cockpit turret
   	
	      int numDockingSlots;
	      WareSize maxCargoSize;
         XStringResource  race;
	      int hullStrength;
	      int explosionDefinition;
	      int explosionBodyDefinition;
	      int particleEmitter;
	      int rotationAcceleration;
	      /// <summary>Ship class such as M1, M3. Not the same as <see cref="TObject.subtype"/>.</summary>
	      wstring shipClass;
         VariationLookup variation;
      };

      /// <summary></summary>
      class TShipReader : public TFileReader<TShip>
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
	      void  ReadObject(TShip& obj, GameVersion ver)
         {
            
         }

         // -------------------- REPRESENTATION ---------------------

      public:
      };
   
   }
}

using namespace Logic::Types;
