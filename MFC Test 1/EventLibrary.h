#pragma once

#include "Logic/Event.h"

namespace GUI
{
   // -------------------------- EVENTS --------------------------

   typedef Event<CDocument*>  DocumentActivatedEvent;
   typedef Event<CDocument*>  DocumentUpdatedEvent;

   // ------------------------- DELEGATES ------------------------

   typedef DocumentActivatedEvent::DelegatePtr  DocumentActivatedHandler;
   typedef DocumentUpdatedEvent::DelegatePtr    DocumentUpdatedHandler;

   // -------------------------- CLASSES -------------------------

   /// <summary></summary>
   class EventLibrary
   {
		// ------------------------ TYPES --------------------------
   
      // --------------------- CONSTRUCTION ----------------------

   private:
      EventLibrary();
   public:
      virtual ~EventLibrary();
		 
		NO_COPY(EventLibrary);	// No copy semantics
		NO_MOVE(EventLibrary);	// No move semantics

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
	   
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------

   public:
      static EventLibrary Instance;

   public:
      DocumentActivatedEvent  DocumentActivated;
      DocumentUpdatedEvent    DocumentUpdated;
   };
   
   // Macro for accessing event library singleton
   #define EventLib  EventLibrary::Instance
}

using namespace GUI;
