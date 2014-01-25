#pragma once

#include "Common.h"
#include "TFile.hpp"

namespace Logic
{
   namespace Scripts
   {
      /// <summary></summary>
      class TFilenameReader
      {
      public:
         TFilenameReader(Path p) : Valid(true), Type(MainType::Unknown)
         {
            // Get lowercase filename without extension
            wstring name = GuiString(p.RemoveExtension().FileName).ToLower();

            // Identify file
            if (name == L"tlasers")
               Type = MainType::Laser;
            /*else if (name == L"tmissiles")
               Type = MainType::Missile;*/
            /*else if (name == L"tships")
               Type = MainType::Ship;*/
            else if (name == L"tdocks")
               Type = MainType::Dock;
            else if (name == L"tfactories")
               Type = MainType::Factory;
            else if (name == L"twaret")
               Type = MainType::TechWare;
            else if (name == L"twareb")
               Type = MainType::BioWare;
            else if (name == L"twaree")
               Type = MainType::EnergyWare;
            else if (name == L"twarem")
               Type = MainType::MineralWare;
            else if (name == L"twaren")
               Type = MainType::NaturalWare;
            else if (name == L"twaref")
               Type = MainType::FoodWare;
            else
               Valid = false;
         }

         bool      Valid;
         MainType  Type;
      };
      
      /// <summary></summary>
      class GameObjectLibrary
      {
		   // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------

      public:
         GameObjectLibrary();
         virtual ~GameObjectLibrary();
		 
		   NO_COPY(GameObjectLibrary);	// No copy semantics
		   NO_MOVE(GameObjectLibrary);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         void  Clear();
         UINT  Enumerate(const XFileSystem& vfs, WorkerData* data);
         TObject*  Find(MainType main, UINT subtype);

         // -------------------- REPRESENTATION ---------------------
      public:
         static GameObjectLibrary  Instance;

      private:
         vector<TFilePtr>  Files;
      };
   
      // Access to Game object library singleton
      #define GameObjectLib  GameObjectLibrary::Instance
   }
}

using namespace Logic::Scripts;
