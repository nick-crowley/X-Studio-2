#pragma once

namespace Logic
{
   /// <summary>RGB Colours</summary>
   enum class Colour : COLORREF 
   { 
      Black = RGB(0,0,0), 
      White = RGB(255,255,255), 
      Default = RGB(200,200,200),
      Silver = RGB(126,126,126),
      Green = RGB(0,255,0), 
      Blue = RGB(0,0,255), 
      Cyan = RGB(150,220,220),
      Red = RGB(255,0,0), 
      Yellow = RGB(255,255,0), 
      Grey = RGB(128,128,128),
      Orange = RGB(200,200,50),
      Purple = RGB(255,0,255) 
   };

   /// <summary>Console manipulators</summary>
   enum class Cons { Heading, 
                     Normal, 
                     Bold, 
                     Endl,
                     Reset,
                     
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
         *this << ENDL << Cons::Bold
               << Cons::Purple << L"ERROR: " 
               << Cons::Red << msg 
               << Cons::White << L"...     Source: " 
               << Cons::Yellow << src << ENDL;
         *this << Cons::Purple << L"CAUSE: " 
               << Cons::Red << e.Message.TrimRight(L"\r\n")
               << Cons::White << L"...     Source: " 
               << Cons::Yellow << e.Source << ENDL;
      }

      /// <summary>Logs an exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  Log(const GuiString& src, const ExceptionBase&e)
      {
         *this << ENDL << Cons::Bold
               << Cons::Purple << L"EXCEPTION: " 
               << Cons::Red << e.Message.TrimRight(L"\r\n") 
               << Cons::White << L"...    Source: " 
               << Cons::Yellow << src << ENDL;
         *this << Cons::Purple << L"SOURCE: " 
               << Cons::Yellow << e.Source << ENDL;
      }

      /// <summary>Logs an STL exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  Log(const GuiString& src, const exception&e)
      {
         *this << ENDL << Cons::Bold 
               << Cons::Purple << L"STL EXCEPTION: " 
               << Cons::Red << e.what()
               << Cons::White << L"...    Source: " 
               << Cons::Yellow << src << ENDL;
      }

      /// <summary>Inserts associated text colour manipulator, if any</summary>
      /// <param name="c">Colour</param>
      ConsoleWnd& operator<<(Colour c)
      {
         switch (c)
         {
         // Supported
         case Colour::Blue:   return *this << Cons::Blue;
         case Colour::Green:  return *this << Cons::Green;
         case Colour::Red:    return *this << Cons::Red;
         case Colour::Cyan:   return *this << Cons::Cyan;
         case Colour::Purple: return *this << Cons::Purple;
         case Colour::Yellow: return *this << Cons::Yellow;
         case Colour::White:  return *this << Cons::White;
         }

         // Unsupported
         return *this;
      }

      /// <summary>Inserts text manipulator</summary>
      /// <param name="cl">manipulator</param>
      ConsoleWnd& operator<<(Cons c)
      {
         CONSOLE_SCREEN_BUFFER_INFO info;
         GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
         WORD bold = (info.wAttributes & FOREGROUND_INTENSITY);

         switch (c)
         {
         // Bold: Add bold
         case Cons::Bold:   
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), info.wAttributes|FOREGROUND_INTENSITY); 
            break;

         // Normal: Remove bold
         case Cons::Normal: 
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), info.wAttributes& ~FOREGROUND_INTENSITY); 
            break;

         // Reset: White + Normal
         case Cons::Reset:
            return *this << Cons::Normal << Cons::White;

         // Endl: Reset + Linebreak 
         case Cons::Endl:   
            return *this << Cons::Reset << L"\n";

         // Heading: Linebreak + Cyan
         case Cons::Heading:  
            return *this << ENDL << Cons::Cyan;

         // Colour
         case Cons::Blue:   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_BLUE);                   break;
         case Cons::Green:  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_GREEN);                  break;
         case Cons::Red:    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_RED);                    break;
         case Cons::Cyan:   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_BLUE|FOREGROUND_GREEN);  break;
         case Cons::Purple: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_BLUE|FOREGROUND_RED);    break;
         case Cons::Yellow: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_GREEN|FOREGROUND_RED);   break;
         case Cons::White:  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bold|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); break;
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

      /// <summary>Writes a path to the console</summary>
      /// <param name="path">path</param>
      ConsoleWnd& operator<<(const IO::Path& path)
      {
         WriteText(path.c_str());
         return *this;
      }

      /// <summary>Writes an STL exception to the console</summary>
      /// <param name="e">Exception</param>
      ConsoleWnd& operator<<(const exception&  e)
      {
         *this << Cons::Red << L"STL EXCEPTION: " << Cons::Yellow << e.what() << ENDL;
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
