#pragma once

namespace Logic
{
   namespace Threads
   {
      /// <summary>Base class for synchronization classes</summary>
      class SynchronizationObject
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create object from a handle</summary>
         /// <param name="h">Object handle</param>
         SynchronizationObject(HANDLE h) : Handle(h)
         {
         }

         /// <summary>Duplicates an existing object.</summary>
         /// <param name="r">another object</param>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate handle</exception>
         SynchronizationObject(const SynchronizationObject& r) : Handle(r.Handle ? Duplicate(r.Handle) : nullptr)
         {
         }

         /// <summary>Frees event handle</summary>
         virtual ~SynchronizationObject()
         {
            Close();
         }

		   NO_MOVE(SynchronizationObject);	// No move semantics

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Duplicates the object handle.</summary>
         /// <param name="h">Handle to duplicate.</param>
         /// <returns>Copy of input handle</returns>
         /// <exception cref="Logic::ArgumentNullException">Handle is nullptr</exception>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate handle</exception>
         static HANDLE Duplicate(HANDLE h)
         {
            HANDLE copy(nullptr);
            
            REQUIRED(h);
            
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
         /// <summary>Query whether object is signalled.</summary>
         /// <returns></returns>
         virtual bool IsSignalled() const
         {
            return WaitForSingleObject(Handle, 0) == WAIT_OBJECT_0;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Retrieve underlying HANDLE.</summary>
         /// <returns>The result of the operator.</returns>
         operator HANDLE() const
         {
            return Handle;
         }

         /// <summary>Assignment operator</summary>
         /// <param name="r">another object.</param>
         /// <returns></returns>
         /// <exception cref="Logic::Win32Exception">Unable to duplicate object</exception>
         SynchronizationObject& operator=(const SynchronizationObject& r)
         {
            // Avoid self-assignment
            if (&r == this)
               return *this;

            // Close existing handle
            Close();

            // Duplicate input handle
            if (r.Handle)
               Handle = Duplicate(r.Handle);

            return *this;
         }

      protected:
         /// <summary>Closes the object handle.</summary>
         virtual void Close()
         {
            if (Handle)
            {
               ::CloseHandle(Handle);
               Handle = nullptr;
            }
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         HANDLE Handle;       // Object handle
      };
      
   }
}

using namespace Logic::Threads;
