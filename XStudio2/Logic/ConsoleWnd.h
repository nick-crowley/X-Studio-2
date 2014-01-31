#pragma once

namespace Logic
{
   /// <summary>RGB Colours</summary>
   enum class Colour : COLORREF { Black = RGB(0,0,0), Green = RGB(0,255,0), Blue = RGB(0,0,255), White = RGB(255,255,255), Cyan = RGB(150,220,220),
                                  Red = RGB(255,0,0), Yellow = RGB(255,255,0), Grey = RGB(128,128,128), Purple = RGB(255,0,255) };


   /// <summary>Console manipulators</summary>
   enum class Cons { Normal, Bold, Endl, Error };

   /// <summary>Provides a debugging console</summary>
   class ConsoleWnd
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
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

   public:
      /// <summary>Logs an exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      /// <param name="msg">Handler error message</param>
      void  Log(const GuiString& src, const ExceptionBase&e, const GuiString& msg)
      {
         *this << ENDL 
               << Colour::Purple << L"ERROR: " 
               << Colour::Red << msg 
               << Colour::White << L"...     Source: " 
               << Colour::Yellow << src << ENDL;
         *this << Colour::Purple << L"CAUSE: " 
               << Colour::Red << e.Message.TrimRight(L"\r\n")
               << Colour::White << L"...     Source: " 
               << Colour::Yellow << e.Source << ENDL;
      }

      /// <summary>Logs an exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  Log(const GuiString& src, const ExceptionBase&e)
      {
         *this << ENDL 
               << Colour::Purple << L"EXCEPTION: " 
               << Colour::Red << e.Message.TrimRight(L"\r\n") 
               << Colour::White << L"...    Source: " 
               << Colour::Yellow << src << ENDL;
         *this << Colour::Purple << L"SOURCE: " 
               << Colour::Yellow << e.Source << ENDL;
      }

      /// <summary>Logs an STL exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  Log(const GuiString& src, const exception&e)
      {
         *this << ENDL 
               << Colour::Purple << L"STL EXCEPTION: " 
               << Colour::Red << e.what()
               << Colour::White << L"...    Source: " 
               << Colour::Yellow << src << ENDL;
      }

      /// <summary>Sets the text colour</summary>
      /// <param name="cl">The colour</param>
      ConsoleWnd& operator<<(Colour cl)
      {
         switch (cl)
         {
         case Colour::Blue:  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);                  break;
         case Colour::Cyan:  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE|FOREGROUND_GREEN); break;
         case Colour::Green: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);                 break;
         case Colour::Red:   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);                   break;
         case Colour::Yellow: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN|FOREGROUND_RED); break;
         case Colour::Purple: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE|FOREGROUND_RED);  break;
         case Colour::White: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); break;
         }
         return *this;
      }

      /// <summary>Inserts text manipulator</summary>
      /// <param name="cl">The colour</param>
      ConsoleWnd& operator<<(Cons c)
      {
         switch (c)
         {
         case Cons::Bold:   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); break;
         case Cons::Normal: return *this << Colour::White;
         case Cons::Endl:   return *this << Colour::White << L"\n";
         case Cons::Error:  return *this << Colour::Red << L"ERROR : " << Colour::Yellow;
         }
         return *this;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& operator<<(const WCHAR* txt)
      {
         WriteText(txt);
         return *this;
      }

      /// <summary>Writes ansi text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& operator<<(const char* txt)
      {
         return *this << StringResource::Convert(txt, CP_ACP);
      }

      /// <summary>Writes a character to the console</summary>
      /// <param name="ch">Character</param>
      ConsoleWnd& operator<<(wchar ch)
      {
         wchar buf[2] = {ch, NULL};
         return *this << buf;
      }

      /// <summary>Writes signed integer to the console</summary>
      /// <param name="i">Number</param>
      ConsoleWnd& operator<<(int i)
      {
         Writef(L"%d", i);
         return *this;
      }

      /// <summary>Writes unsigned integer to the console</summary>
      /// <param name="i">number</param>
      ConsoleWnd& operator<<(UINT i)
      {
         Writef(L"%u", i);
         return *this;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& operator<<(const wstring& txt)
      {
         WriteText(txt);
         return *this;
      }

      /// <summary>Writes ansi text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& operator<<(const string& txt)
      {
         return *this << StringResource::Convert(txt, CP_ACP);
      }

      /// <summary>Writes pointer to the console</summary>
      /// <param name="p">Pointer</param>
      ConsoleWnd& operator<<(const void* p)
      {
         Writef(L"0x%x", p);
         return *this;
      }


      /// <summary>Writes an STL exception to the console</summary>
      /// <param name="e">Exception</param>
      ConsoleWnd& operator<<(const exception&  e)
      {
         *this << Colour::Red << L"STL EXCEPTION: " << Colour::Yellow << e.what() << ENDL;
         return *this;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  Write(const wstring& txt)
      {
         WriteText(txt);
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  Writef(const WCHAR* format, ...)
      {
         va_list args;
         WriteText( StringResource::FormatV(format, va_start(args, format)) );
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  Writef(const wstring& format, ...)
      {
         va_list args;
         WriteText( StringResource::FormatV(format.c_str(), va_start(args, format)) );
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  WriteLn(const wstring& txt = L"")
      {
         WriteText(txt);
         WriteText(L"\n");
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  WriteLnf(const WCHAR* format = L"", ...)
      {
         va_list args;
         WriteText( StringResource::FormatV(format, va_start(args, format)) );
         WriteText(L"\n");
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  WriteLnf(const wstring& format, ...)
      {
         va_list args;
         WriteText( StringResource::FormatV(format.c_str(), va_start(args, format)) );
         WriteText(L"\n");
      }

   private:
      void WriteText(const wstring& txt)
      {
         DWORD written=0;
         WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), txt.c_str(), txt.length(), &written, NULL);

#ifdef _DEBUG
         OutputDebugString(txt.c_str());
#endif
      }

      // -------------------- REPRESENTATION ---------------------

   public:
      static ConsoleWnd  Instance;

   private:
   };

   // Provide singleton access
   #define Console      ConsoleWnd::Instance
}

using namespace Logic;
