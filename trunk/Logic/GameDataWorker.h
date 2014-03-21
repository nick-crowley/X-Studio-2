#pragma once
#include "BackgroundWorker.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Threads
   {

      /// <summary>Worker thread for loading game data</summary>
      class LogicExport GameDataWorker : public BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Data for game data loading worker thread</summary>
         class LogicExport GameDataWorkerData : public WorkerData
         {
         public:
            GameDataWorkerData() : WorkerData(Operation::LoadGameData), Version(GameVersion::Threat), Language(GameLanguage::English)
            {}

            /// <summary>Resets data + update values from preferences.</summary>
            virtual void  Reset()
            {
               // Update values
               GameFolder = PrefsLib.GameDataFolder;
               Version = PrefsLib.GameDataVersion;
               Language = PrefsLib.GameDataLanguage;

               // Reset 'aborted' flag
               __super::Reset();
            }

         public:
            Path         GameFolder;
            GameVersion  Version;
            GameLanguage Language;
         };
	  
         // --------------------- CONSTRUCTION ----------------------
      public:
	      GameDataWorker();
	      virtual ~GameDataWorker();
       
         // ------------------------ STATIC -------------------------
      protected:
         static void         Clear();
         static DWORD WINAPI ThreadMain(GameDataWorkerData* data);

         // --------------------- PROPERTIES ------------------------
	  
         // ---------------------- ACCESSORS ------------------------			
      
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Loads game data using the current game data preferences.</summary>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start()
         {
            if (IsRunning())
               throw InvalidOperationException(HERE, L"Thread already running");

            // Clear previous (if any)
            Clear();

            // Update Game folder/version
            Data.Reset();

            // Start thread
            __super::Start(&Data);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
	      GameDataWorkerData  Data;
      };


   }
}

using namespace Logic::Threads;



