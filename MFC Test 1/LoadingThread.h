#pragma once
#include "WorkerThread.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   

   /// <summary>Worker thread for loading game data</summary>
   class LoadingThread 
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
	   LoadingThread();
	   virtual ~LoadingThread();
       
      // ------------------------ STATIC -------------------------
      
   public:
      static DWORD WINAPI ThreadMain(WorkerData* data);

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------

   public:
	   

      // -------------------- REPRESENTATION ---------------------
   protected:
	   
   };



NAMESPACE_END(GUI)



