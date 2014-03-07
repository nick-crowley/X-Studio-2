#include "stdafx.h"
#include "Common.h"       // Do not include ConsoleWnd.h directly, referenced via common.h
#include "FileStream.h"
#include "ConsoleLog.h"

namespace Logic
{
   /// <summary>Singleton instance</summary>
   ConsoleWnd  ConsoleWnd::Instance;

   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Creates the console</summary>
   ConsoleWnd::ConsoleWnd() : Handle(nullptr)
   {
      // Create console
      if (AllocConsole())
      {
         // Get handle
         SetConsoleTitle(L"Debug Console");
         Handle = GetStdHandle(STD_OUTPUT_HANDLE);

         // Increase buffer size
         COORD buf { 200, 6*1000 };
         SetConsoleScreenBufferSize(Handle, buf);

         // Increate window size
         //SMALL_RECT wnd { 0, 0, buf.X-2, buf.Y-2};
         //SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &wnd);

         // Disable close button
         DeleteMenu(GetSystemMenu(GetConsoleWindow(), 0), SC_CLOSE, MF_BYCOMMAND);
      }
   }

   /// <summary>Frees the console.</summary>
   ConsoleWnd::~ConsoleWnd()
   {
      // Free console
      FreeConsole();
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   /// <summary>Writes text to the output.</summary>
   /// <param name="txt">The text.</param>
   void  ConsoleWnd::WriteText(const wstring& txt)
   {
      DWORD written=0;

      // Ensure exists
      if (Handle == INVALID_HANDLE_VALUE)
         return;

      // Write to console
      WriteConsole(Handle, txt.c_str(), txt.length(), &written, NULL);
         
      // Write to logfile
      LogFile.Write(txt, Attributes);

#ifdef _DEBUG
      // Write to output window
      OutputDebugString(txt.c_str());
#endif
   }
}
