#pragma once

namespace Logic
{
   namespace Scripts
   {
   
      /// <summary>Helper object for initializing COM functions for a thread function</summary>
      class ComThreadHelper
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Initializes COM as single threaded.</summary>
         /// <exception cref="Logic::ComException">Unable to initialize COM</exception>
         ComThreadHelper()
         {
            HRESULT  hr;

            // Initialize COM
            if (FAILED(hr = ::CoInitialize(NULL)))
               throw ComException(HERE, hr);
         }

         /// <summary>Cleans up COM.</summary>
         virtual ~ComThreadHelper()
         {
            ::CoUninitialize();
         }
		 
		   NO_COPY(ComThreadHelper);	// Uncopyable
		   NO_MOVE(ComThreadHelper);	// Unmoveable

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      protected:
      };
   
   }
}

using namespace Logic::Scripts;
