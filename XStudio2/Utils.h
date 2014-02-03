#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   /// <summary></summary>
   class ClientRect : public CRect
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------

   public:
      ClientRect(CWnd* wnd)
      {
         wnd->GetClientRect(this);
      }
		 
		DEFAULT_COPY(ClientRect);	// Default copy semantics
		DEFAULT_MOVE(ClientRect);	// Default move semantics

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
 
   };

NAMESPACE_END(GUI)


