#include "stdafx.h"
#include "Common.h"       // Do not include ConsoleWnd.h directly, referenced via common.h

namespace Logic
{
   /// <summary>Singleton instance</summary>
   ConsoleWnd  ConsoleWnd::Instance;

   // -------------------------------- CONSTRUCTION --------------------------------

   ConsoleWnd::ConsoleWnd()
   {
      if (AllocConsole())
      {
         SetConsoleTitle(L"Debug Console");

         COORD buf { 200, 6*1000 };
         SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), buf);

         //SMALL_RECT wnd { 0, 0, buf.X-2, buf.Y-2};
         //SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &wnd);
      }
   }


   ConsoleWnd::~ConsoleWnd()
   {
      FreeConsole();
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------
}
