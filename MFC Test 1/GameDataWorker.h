#pragma once
#include "BackgroundWorker.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Threads)

   using namespace Logic::IO;

   /// <summary>Data for game data loading worker thread</summary>
   class GameDataWorkerData : public WorkerData
   {
   public:
      GameDataWorkerData(Path folder, GameVersion ver) : GameFolder(folder), Version(ver)
      {}

      Path         GameFolder;
      GameVersion  Version;
   };

   /// <summary>Worker thread for loading game data</summary>
   class GameDataWorker : public BackgroundWorker
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
	   GameDataWorker();
	   virtual ~GameDataWorker();
       
      // ------------------------ STATIC -------------------------
      
   protected:
      static DWORD WINAPI ThreadMain(GameDataWorkerData* data);

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------

   public:
      bool  Start(GameDataWorkerData* param)
      {
         return BackgroundWorker::Start(param);
      }

      // -------------------- REPRESENTATION ---------------------
   protected:
	   
   };



NAMESPACE_END2(GUI,Threads)



