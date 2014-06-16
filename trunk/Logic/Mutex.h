#pragma once
#include "SynchronizationObject.h"

namespace Logic
{
   namespace Threads
   {
      
      /// <summary>Wrapper for a Win32 mutex</summary>
      class Mutex : public SynchronizationObject
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create an unnamed event</summary>
         /// <param name="state">Initially signalled or non-signalled</param>
         /// <param name="manual">Whether manual reset</param>
         /// <exception cref="Logic::Win32Exception">Unable to create event</exception>
         Mutex(bool locked)
            : SynchronizationObject( CreateMutex(nullptr, locked?TRUE:FALSE, nullptr) )
         {
            if (!Handle)
               throw Win32Exception(HERE, L"Unable to create mutex");
         }

         /// <summary>Duplicates an existing mutex.</summary>
         /// <param name="r">another mutex.</param>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate mutex</exception>
         Mutex(const Mutex& r) : SynchronizationObject(r)
         {
         }

		   NO_MOVE(Mutex);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Acquire the mutex lock</summary>
         /// <exception cref="Logic::Win32Exception">Unable to acquire lock</exception>
         void Lock()
         {
            if (WaitForSingleObject(Handle, INFINITE) == WAIT_FAILED)
               throw Win32Exception(HERE, L"Unable to lock mutex");
         }

         /// <summary>Releases the mutex lock.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to release mutex</exception>
         void Release()
         {
            if (!ReleaseMutex(Handle))
               throw Win32Exception(HERE, L"Unable to release mutex");
         }

         /// <summary>Assignment operator</summary>
         /// <param name="r">another.</param>
         /// <returns></returns>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate mutex</exception>
         Mutex& operator=(const Mutex& r)
         {
            return (Mutex&)__super::operator=(r);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

   }
}

using namespace Logic::Threads;
