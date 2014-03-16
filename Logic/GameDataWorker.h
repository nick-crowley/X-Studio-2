#pragma once
#include "BackgroundWorker.h"

namespace Logic
{
   namespace Threads
   {

      /// <summary>Data for game data loading worker thread</summary>
      class LogicExport GameDataWorkerData : public WorkerData
      {
      public:
         GameDataWorkerData(Path folder, GameVersion ver) : WorkerData(Operation::LoadGameData), GameFolder(folder), Version(ver)
         {}

         Path         GameFolder;
         GameVersion  Version;
      };

      /// <summary>Worker thread for loading game data</summary>
      class LogicExport GameDataWorker : public BackgroundWorker
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
         /// <summary>Starts the thread.</summary>
         /// <param name="param">operation data.</param>
         /// <exception cref="Logic::ArgumentNullException">param is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(GameDataWorkerData* param)
         {
            BackgroundWorker::Start(param);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
	   
      };


   }
}

using namespace Logic::Threads;



