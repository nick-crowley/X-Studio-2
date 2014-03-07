#include "stdafx.h"
#include "Common.h"       // Do not include ConsoleWnd.h directly, referenced via common.h

namespace Logic
{
   /// <summary>Singleton instance</summary>
   ConsoleWnd  ConsoleWnd::Instance;

   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Creates the console</summary>
   ConsoleWnd::ConsoleWnd() : Handle(nullptr)
   {
      if (AllocConsole())
      {
         // Get handle
         SetConsoleTitle(L"Debug Console");
         Handle = GetStdHandle(STD_OUTPUT_HANDLE);

         // Increase size
         COORD buf { 200, 6*1000 };
         SetConsoleScreenBufferSize(Handle, buf);

         //SMALL_RECT wnd { 0, 0, buf.X-2, buf.Y-2};
         //SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &wnd);

         // Disable close button
         DeleteMenu(GetSystemMenu(GetConsoleWindow(), 0), SC_CLOSE, MF_BYCOMMAND);
      }
   }


   /// <summary>Frees the console.</summary>
   ConsoleWnd::~ConsoleWnd()
   {
      FreeConsole();
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------
}
