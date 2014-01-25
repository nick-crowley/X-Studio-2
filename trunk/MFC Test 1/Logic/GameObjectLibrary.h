#pragma once

#include "Common.h"
#include "TFile.hpp"

namespace Logic
{
   namespace Scripts
   {
      
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
