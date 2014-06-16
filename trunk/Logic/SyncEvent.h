#pragma once

namespace Logic
{
   namespace Threads
   {
      
      /// <summary>Wrapper for a Win32 event synchronization object</summary>
      class SyncEvent
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
            : Handle( CreateEvent(nullptr, manual?TRUE:FALSE, state?TRUE:FALSE, nullptr) )
         {
            if (!Handle)
               throw Win32Exception(HERE, L"Unable to create event");
         }

         /// <summary>Duplicates an existing event.</summary>
         /// <param name="r">other event.</param>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate event</exception>
         SyncEvent(const SyncEvent& r) : Handle(r.Handle ? Duplicate(r.Handle) : nullptr)
         {
         }

         /// <summary>Frees event handle</summary>
         virtual ~SyncEvent()
         {
            Close();
         }

		   NO_MOVE(SyncEvent);	// No move semantics

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Duplicates the specified handle.</summary>
         /// <param name="h">Handle to duplicate.</param>
         /// <returns>Copy of input handle</returns>
         /// <exception cref="Logic::ArgumentNullException">Handle is nullptr</exception>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate handle</exception>
         static HANDLE Duplicate(HANDLE h)
         {
            REQUIRED(h);

            HANDLE copy = nullptr;

            // Duplicate handle
            if (!DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &copy, -1, FALSE, DUPLICATE_SAME_ACCESS))
               throw Win32Exception(HERE, L"Unable to clone event");

            return copy;
         }

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(bool,Signalled,IsSignalled);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether event is signalled.</summary>
         /// <returns></returns>
         bool IsSignalled() const
         {
            return WaitForSingleObject(Handle, 0) == WAIT_OBJECT_0;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Signal this event.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to signal event</exception>
         void Signal()
         {
            if (!SetEvent(Handle))
               throw Win32Exception(HERE, L"Unable to signal event: " + SysErrorString());
         }

         /// <summary>Resets event to unsignalled.</summary>
         /// <exception cref="Logic::Win32Exception">Unable to reset event</exception>
         void Reset()
         {
            if (!ResetEvent(Handle))
               throw Win32Exception(HERE, L"Unable to reset event: " + SysErrorString());
         }

         /// <summary>Retrieve underlying HANDLE.</summary>
         /// <returns>The result of the operator.</returns>
         operator HANDLE() const
         {
            return Handle;
         }

         /// <summary>Assignment operator</summary>
         /// <param name="r">another.</param>
         /// <returns></returns>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate event</exception>
         SyncEvent& operator=(const SyncEvent& r)
         {
            if (&r == this)
               return *this;

            // Close existing
            Close();

            // Duplicate input handle
            if (r.Handle)
               Handle = Duplicate(r.Handle);

            return *this;
         }

      protected:
         /// <summary>Closes the event handle.</summary>
         void Close()
         {
            if (Handle)
            {
               ::CloseHandle(Handle);
               Handle = nullptr;
            }
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         HANDLE Handle;       // Event handle
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
