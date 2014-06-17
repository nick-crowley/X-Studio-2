#pragma once

namespace Logic
{
   namespace Threads
   {
      
      /// <summary>Critical section synchronization object</summary>
      class CriticalSection 
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates a critical section</summary>
         CriticalSection()
         {
            InitializeCriticalSection(&Section);
         }

         /// <summary>Destroys the critical section</summary>
         ~CriticalSection()
         {
            DeleteCriticalSection(&Section);
         }

         NO_COPY(CriticalSection);  // Uncopyable
		   NO_MOVE(CriticalSection);	// Unmoveable

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Enters the critical section</summary>
         void Enter()
         {
            EnterCriticalSection(&Section);
         }

         /// <summary>Exits the critical section</summary>
         void Leave()
         {
            LeaveCriticalSection(&Section);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         CRITICAL_SECTION Section;
      };

   }
}

using namespace Logic::Threads;
