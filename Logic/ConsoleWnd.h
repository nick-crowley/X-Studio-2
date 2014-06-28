#pragma once
#include "Mutex.h"
#include "CriticalSection.h"

namespace Logic
{
   namespace Utils
   {
      class ConsoleWnd;

      /// <summary>Console manipulators</summary>
      enum class Cons 
      { 
         UserAction, // CRLF + Bold + Cyan
         Heading,    // CRLF + Cyan
         Normal,     // Remove bold
         Error,      // Bold + Red
         Exception,  // Bold + Purple
         Success,    // Bold + Green + "Success!"
         Failure,    // Bold + Red + "Failed: "
         Warning,    // CRLF + Bold + Yellow + "WARNING: "
         Bold,       // Add bold
         Endl,       // Reset + CRLF 
         Reset,      // White + Normal
         Push,       // Save current attributes
         Pop,        // Restore previously saved attributes 
         //Lock,       // Locks the console for the calling thread
         //Unlock,     // Release the lock upon the console
                     
         White,
         Blue,
         Cyan,
         Green,
         Red,
         Yellow,
         Purple
      };

      /// <summary>Shorthand for console end-of-line manipulator</summary>
      const static Cons ENDL = Cons::Endl;

      /// <summary>Defines convenient combinations of console colours</summary>
      const static UINT FOREGROUND_CYAN = FOREGROUND_BLUE|FOREGROUND_GREEN,
                        FOREGROUND_PURPLE = FOREGROUND_RED|FOREGROUND_BLUE,
                        FOREGROUND_WHITE = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
                        FOREGROUND_YELLOW = FOREGROUND_GREEN|FOREGROUND_RED;

   
      /// <summary>Write rectangle to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc);

      /// <summary>Write size to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz);

      /// <summary>Write basic window details to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CWnd& wnd);

      /// <summary>Provides a debugging console</summary>
      class LogicExport ConsoleWnd
      {
		   // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      private:
         ConsoleWnd();
      public:
         virtual ~ConsoleWnd();
		 
		   NO_COPY(ConsoleWnd);	// Cannot be copied
		   NO_MOVE(ConsoleWnd);	// Cannot be moved

         // ------------------------ STATIC -------------------------
   
         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET_SET(bool,Visible,IsVisible,Show);

      protected:
         PROPERTY_GET_SET(WORD,Attributes,GetAttributes,SetAttributes);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Determines whether console is visible</summary>
         /// <returns></returns>
         bool IsVisible() const;

      protected:
         /// <summary>Gets the attributes</summary>
         /// <returns></returns>
         WORD GetAttributes();

         /// <summary>Sets the attributes</summary>
         /// <param name="attr">The attribute</param>
         void SetAttributes(WORD attr);

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Logs an exception to the console.</summary>
         /// <param name="src">Handler location</param>
         /// <param name="e">error</param>
         /// <param name="msg">Handler error message</param>
         void  Log(const GuiString& src, const ExceptionBase&e, const GuiString& msg);

         /// <summary>Logs an exception to the console.</summary>
         /// <param name="src">Handler location</param>
         /// <param name="e">error</param>
         void  Log(const GuiString& src, const ExceptionBase&e);

         /// <summary>Logs an STL exception to the console.</summary>
         /// <param name="src">Handler location</param>
         /// <param name="e">error</param>
         void  Log(const GuiString& src, const exception&e);

         /// <summary>Locks the console for the calling thread</summary>
         void  Lock();

         /// <summary>Release the lock upon the console</summary>
         void  Release();

         /// <summary>Shows/Hides the console.</summary>
         /// <param name="show">The show.</param>
         void  Show(bool show);

         /// <summary>Inserts associated text colour manipulator, if any</summary>
         /// <param name="c">Colour</param>
         ConsoleWnd& operator<<(Colour c);

         /// <summary>Inserts text manipulator</summary>
         /// <param name="cl">manipulator</param>
         ConsoleWnd& operator<<(Cons c);

         /// <summary>Writes text to the console</summary>
         /// <param name="txt">Text</param>
         ConsoleWnd& operator<<(const WCHAR* txt);

         /// <summary>Writes ansi text to the console</summary>
         /// <param name="txt">Text</param>
         ConsoleWnd& operator<<(const char* txt);

         /// <summary>Writes a character to the console</summary>
         /// <param name="ch">Character</param>
         ConsoleWnd& operator<<(wchar ch);

         /// <summary>Writes signed integer to the console</summary>
         /// <param name="i">Number</param>
         ConsoleWnd& operator<<(int i);

         /// <summary>Writes unsigned integer to the console</summary>
         /// <param name="i">number</param>
         ConsoleWnd& operator<<(UINT i);

         /// <summary>Writes text to the console</summary>
         /// <param name="txt">Text</param>
         ConsoleWnd& operator<<(const wstring& txt);

         /// <summary>Writes ansi text to the console</summary>
         /// <param name="txt">Text</param>
         ConsoleWnd& operator<<(const string& txt);

         /// <summary>Writes pointer to the console</summary>
         /// <param name="p">Pointer</param>
         ConsoleWnd& operator<<(const void* p);

         /// <summary>Writes a path to the console in yellow</summary>
         /// <param name="path">path</param>
         ConsoleWnd& operator<<(const Path& path);

         /// <summary>Writes a game version string to the console in yellow</summary>
         /// <param name="str">game version string</param>
         ConsoleWnd& operator<<(const VersionString& str);

      private:
         /// <summary>Writes the formatted text to the console</summary>
         /// <param name="format">Formatting string</param>
         /// <param name="...">Arguments</param>
         void Writef(const WCHAR* format, ...);

         /// <summary>Writes text to the output.</summary>
         /// <param name="txt">The text.</param>
         void WriteText(const wstring& txt);

         // -------------------- REPRESENTATION ---------------------
      public:
         static ConsoleWnd  Instance;        // Singleton instance

      private:
         deque<WORD>      AttributeStack;    // Attributes stack
         HANDLE           Handle;            // Console Handle
         CriticalSection  Owner;             // Provides thread safety
      };

      /// <summary>Provides access to the console singleton</summary>
      #define Console  ConsoleWnd::Instance



      /// <summary>Automatically acquires/releases the console lock</summary>
      class ConsoleLock
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Blocks until the console lock is accquired</summary>
         ConsoleLock()  { Console.Lock(); }

         /// <summary>Releases the lock upon the console</summary>
         ~ConsoleLock() { Console.Release(); }
      };
	  
      /// <summary>Provides thread safe access to the console singleton by locking entire statement</summary>
      /// <param name="exp">Console I/O statement (without reference to 'Console' singleton)</param>
      #define SyncConsole(exp)  { ConsoleLock lock; Console << exp; }
   }
}

using namespace Logic::Utils;
