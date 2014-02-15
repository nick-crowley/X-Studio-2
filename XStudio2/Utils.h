#pragma once

/// <summary>User interface</summary>
namespace GUI
{
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

   /// <summary>Write rectangle to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc);

   /// <summary>Write size to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz);

}


