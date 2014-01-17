#pragma once

namespace Logic
{
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
         OutputDebugString(txt.c_str());
         WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), txt.c_str(), txt.length(), &written, NULL);
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
