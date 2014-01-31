#include "stdafx.h"
#include "TShip.h"

namespace Logic
{
   namespace Types
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Reads the properties unique to this type</summary>
      /// <param name="obj">Object to read into</param>
      /// <param name="ver">File version</param>
      /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
	   void  TShipReader::ReadObject(TShip& obj, GameVersion ver)
      {
         obj.speed              = ReadInt(L"speed");
	      obj.acceleration       = ReadInt(L"acceleration");
	      obj.engineSound        = ReadInt(L"engineSound");
	      obj.averageReactionDelay = ReadInt(L"averageReactionDelay");
	      obj.engineEffect       = ReadInt(L"engineEffect");
	      obj.engineGlowEffect   = ReadInt(L"engineGlowEffect");
	      obj.power              = ReadInt(L"power");
	      obj.soundVolumeMin     = ReadInt(L"soundVolumeMin");
	      obj.soundVolumeMax     = ReadInt(L"soundVolumeMax");
	      obj.modelScene         = ReadString(L"modelScene");
	      obj.cockpitScene       = ReadString(L"cockpitScene");
	      obj.laserMask          = ReadInt(L"laserMask");

	      UINT gunCount = ReadInt(L"gunCount");

	      obj.laserEnergy        = ReadInt(L"laserEnergy");
	      obj.laserRechargeRate  = ReadFloat(L"laserRechargeRate");
	      obj.shieldType         = ReadInt(L"shieldType");
	      obj.maxNumOfShileds    = ReadInt(L"maxNumOfShileds");
	      obj.missileMask        = ReadInt(L"missileMask");
	      obj.maxMissileCountNPC = ReadInt(L"maxMissileCountNPC");
	      obj.extensionsSpeed    = ReadInt(L"extensionsSpeed");
	      obj.extensionsSteering = ReadInt(L"extensionsSteering");
	      obj.cargoMin           = ReadInt(L"cargoMin");
	      obj.cargoMax           = ReadInt(L"cargoMax");
	      obj.wareListID         = ReadInt(L"wareListID");

	      for(int i = 0; i < 6; i++) 
         {
            int index = ReadInt(L"cockpit index");
            auto pos = (TShip::TurretPosition)ReadInt(L"turret position");

		      obj.turrets.push_back(TShip::Turret(index, pos));
         }

	      obj.numDockingSlots       = ReadInt(L"numDockingSlots");
	      obj.maxCargoSize          = (WareSize)ReadInt(L"maxCargoSize");
	      obj.race.ID               = ReadInt(L"race");
	      obj.hullStrength          = ReadInt(L"hullStrength");
	      obj.explosionDefinition   = ReadInt(L"explosionDefinition");
	      obj.explosionBodyDefinition = ReadInt(L"explosionBodyDefinition");
	      obj.particleEmitter       = ReadInt(L"particleEmitter");
	      obj.variation.ID          = ReadInt(L"variationIndex");
	      obj.rotationAcceleration  = ReadInt(L"rotationAcceleration");
	      obj.shipClass             = ReadString(L"shipType");

	      UINT turretCount = ReadInt(L"turretCount");

	      while(obj.turrets.size() > turretCount) 
		      obj.turrets.pop_back();

	      LoadTurrets(obj);

	      LoadGunGroups(obj);
      }

      

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Loads the gun groups.</summary>
      /// <param name="ship">The ship.</param>
      void  TShipReader::LoadGunGroups(TShip ship)
	   {
		   UINT numGunGroups = ReadInt(L"numGG");

         // Ensure cockpit present
		   if(numGunGroups > 0 && ship.turrets.size() == 0)
			   ship.turrets.push_back(TShip::Turret(0, TShip::TurretPosition::cockpit));

		   UINT i = 0;
		   try 
         {
			   for(i = 0; i < numGunGroups; i++) 
            {
				   int tmp = ReadInt(L"first index");
				   tmp = ReadInt(L"num lasers");
				   tmp = ReadInt(L"index");
				   int numWeapons = ReadInt(L"numWeapons");

				   // if there's no turret for current gg, put weapons to the first (cockpit)
				   // - case of some old corrupted ship entries
				   LoadWeapons(ship.turrets[i >= ship.turrets.size() ? 0 : i], numWeapons);
			   }
		   }
         catch(ExceptionBase& e) { 
            throw FileFormatException(HERE, GuiString(L"Error loading weapon group %d : %s", i, e.Message.c_str())); 
         }
	   }


      /// <summary>Loads the turrets.</summary>
      /// <param name="ship">The ship.</param>
      void  TShipReader::LoadTurrets(TShip ship)
	   {
		   UINT i = 0;
		   try 
         {
			   for(i = 0; i < ship.turrets.size(); i++) 
            {
				   int tmp = ReadInt(L"index");
				   tmp = ReadInt(L"index2");
				   ship.turrets[i].special = tmp == -1;
				   ship.turrets[i].modelFile = ReadString(L"modelFile");
				   ship.turrets[i].sceneNodeIndex = ReadInt(L"sceneNodeIndex");
			   }
		   }
         catch(ExceptionBase& e) { 
            throw FileFormatException(HERE, GuiString(L"Error loading turret %d : %s", i, e.Message.c_str())); 
         }
	   }

      
      /// <summary>Loads the turret weapons.</summary>
      /// <param name="turret">The turret.</param>
      /// <param name="count">The count.</param>
      void  TShipReader::LoadWeapons(TShip::Turret turret, UINT count)
	   {
		   UINT i = 0;
		   try 
         {
			   for(i = 0; i < count; i++) 
            {
				   TShip::Weapon w;
				   int idx = ReadInt(L"index");
				   w.numLasers = ReadInt(L"numLasers");
				   w.modelName1 = ReadString(L"modelName1");
				   w.sceneNode1 = ReadInt(L"sceneNode1");
				   w.modelName2 = ReadString(L"modelName2");
				   w.sceneNode2 = ReadInt(L"sceneNode2");

				   turret.weapons.push_back(w);
			   }
		   }
		   catch(ExceptionBase& e) { 
            throw FileFormatException(HERE, GuiString(L"Error loading Weapon %d : %s", i, e.Message.c_str())); 
         }
	   }
   }
}
