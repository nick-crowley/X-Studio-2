#pragma once

namespace Logic
{
   
   /// <summary>Console manipulators</summary>
   enum class Cons { UserAction,
                     Heading, 
                     Normal, 
                     Error,
                     Exception,
                     Success,
                     Failure,
                     Bold, 
                     Endl,
                     Reset,
                     Push,
                     Pop,
                     
                     White,
                     Blue,
                     Cyan,
                     Green,
                     Red,
                     Yellow,
                     Purple
   };

   /// <summary>Shorthand for console end-of-line manipulator</summary>
   const Cons ENDL = Cons::Endl;

   /// <summary>Provides a debugging console</summary>
   class EXPORTED ConsoleWnd
   {
		// ------------------------ TYPES --------------------------
   private:
	  
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
      /// <summary>Determines whether console is visible.</summary>
      /// <returns></returns>
      bool IsVisible() const;

   protected:
      /// <summary>Gets the attributes.</summary>
      /// <returns></returns>
      WORD GetAttributes();

      /// <summary>Sets the attributes.</summary>
      /// <param name="attr">The attribute.</param>
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

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  Write(const wstring& txt);

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  Writef(const WCHAR* format, ...);

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  Writef(const wstring& format, ...);

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  WriteLn(const wstring& txt = L"");

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  WriteLnf(const WCHAR* format = L"", ...);

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  WriteLnf(const wstring& format, ...);

   private:
      /// <summary>Writes text to the output.</summary>
      /// <param name="txt">The text.</param>
      void WriteText(const wstring& txt);

      // -------------------- REPRESENTATION ---------------------
   public:
      static ConsoleWnd  Instance;

   private:
      deque<WORD> AttributeStack;
      HANDLE      Handle;
   };

   // Provide singleton access
   #define Console      ConsoleWnd::Instance
}

using namespace Logic;
