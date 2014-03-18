#pragma once
#include "BackgroundWorker.h"

namespace Logic
{
   namespace Threads
   {

      /// <summary>Background worker for importing a legacy project</summary>
      class LogicExport ImportProjectWorker : public BackgroundWorker
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Data for import project worker thread</summary>
         class LogicExport ImportProjectData : public WorkerData
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Default ctor</summary>
            ImportProjectData()
               : WorkerData(Operation::ImportProject)
            {}

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            /// <summary>Resets to initial state.</summary>
            /// <param name="legacy">legacy project path to upgrade</param>
            /// <param name="upgrade">upgraded project path to create.</param>
            void  Reset(Path legacy, Path upgrade)
            {
               LegacyPath = legacy;
               UpgradePath = upgrade;

               __super::Reset();
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            Path   LegacyPath,
                   UpgradePath;
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         ImportProjectWorker();
         virtual ~ImportProjectWorker();

         NO_COPY(ImportProjectWorker);	// No copy semantics
         NO_MOVE(ImportProjectWorker);	// No move semantics

         // ------------------------ STATIC -------------------------
      protected:
         static DWORD WINAPI ThreadMain(ImportProjectData* data);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Imports and upgrades a project file.</summary>
         /// <param name="legacy">legacy project path to upgrade</param>
         /// <param name="upgrade">upgraded project path to create.</param>
         /// <exception cref="Logic::InvalidOperationException">Thread already running</exception>
         /// <exception cref="Logic::Win32Exception">Failed to start Thread</exception>
         void  Start(Path legacy, Path upgrade)
         {
            if (IsRunning())
               throw InvalidOperationException(HERE, L"Thread already running");

            // Reset data
            Data.Reset(legacy, upgrade);

            // Start thread
            __super::Start(&Data);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         ImportProjectData  Data;
      };

   }
}

using namespace Logic::Threads;
