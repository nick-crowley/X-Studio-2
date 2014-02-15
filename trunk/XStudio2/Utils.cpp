#include "stdafx.h"
#include "Utils.h"

namespace GUI
{

   // -------------------------------- CONSTRUCTION --------------------------------


   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Write rectangle to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc)
   {
      return c << "{CRect left=" << rc.left << " top=" << rc.top << " right=" << rc.right << " bottom=" << rc.bottom << "}";
   }

   /// <summary>Write size to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz)
   {
      return c << "{CSize width=" << sz.cx << " height=" << sz.cy << "}";
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

}

