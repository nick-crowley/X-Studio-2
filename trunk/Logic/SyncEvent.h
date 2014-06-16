#pragma once
#include "SynchronizationObject.h"

namespace Logic
{
   namespace Threads
   {
      
      /// <summary>Wrapper for a Win32 event synchronization object</summary>
      class SyncEvent : public SynchronizationObject
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create an unnamed event</summary>
         /// <param name="state">Initially signalled or non-signalled</param>
         /// <param name="manual">Whether manual reset</param>
         /// <exception cref="Logic::Win32Exception">Unable to create event</exception>
         SyncEvent(bool state, bool manual)
            : SynchronizationObject( CreateEvent(nullptr, manual?TRUE:FALSE, state?TRUE:FALSE, nullptr) )
         {
            if (!Handle)
               throw Win32Exception(HERE, L"Unable to create event");
         }

         /// <summary>Duplicates an existing event.</summary>
         /// <param name="r">other event.</param>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate event</exception>
         SyncEvent(const SyncEvent& r) : SynchronizationObject(r)
         {
         }

		   NO_MOVE(SyncEvent);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Signal this event.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to signal event</exception>
         void Signal()
         {
            if (!SetEvent(Handle))
               throw Win32Exception(HERE, L"Unable to signal event");
         }

         /// <summary>Resets event to unsignalled.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to reset event</exception>
         void Reset()
         {
            if (!ResetEvent(Handle))
               throw Win32Exception(HERE, L"Unable to reset event");
         }

         /// <summary>Assignment operator</summary>
         /// <param name="r">another.</param>
         /// <returns></returns>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate event</exception>
         SyncEvent& operator=(const SyncEvent& r)
         {
            return (SyncEvent&)__super::operator=(r);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Manual reset win32 event synchronization object.</summary>
      class ManualEvent : public SyncEvent
      {
      public:
         /// <summary>Create a manual reset event.</summary>
         /// <param name="state">Whether initially signalled.</param>
         ManualEvent(bool state) : SyncEvent(state, true)
         {}
      };

   }
}

using namespace Logic::Threads;
