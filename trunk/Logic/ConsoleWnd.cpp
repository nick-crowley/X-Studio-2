#include "stdafx.h"
#include "ConsoleWnd.h"
#include "ConsoleLog.h"

namespace Logic
{
   namespace Utils
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

            // Maximize
            ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

#ifdef OFFICIAL_RELEASE
            // Initially hide
            Visible = false;
#endif
         }
      }

      /// <summary>Frees the console.</summary>
      ConsoleWnd::~ConsoleWnd()
      {
         // Free console
         FreeConsole();
      }

      // ------------------------------- STATIC METHODS -------------------------------
   
      /// <summary>Write rectangle to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc)
      {
         return c << "{CRect left=" << rc.left << " top=" << rc.top << " right=" << rc.right << " bottom=" << rc.bottom << "}";
      }

      /// <summary>Write size to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz)
      {
         return c << "{CSize width=" << sz.cx << " height=" << sz.cy << "}";
      }
   
      /// <summary>Write basic window details to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const CWnd& wnd)
      {
         CString txt;
         CRect   rc;
      
         // Prepare
         wnd.GetWindowText(txt);
         wnd.GetWindowRect(rc);
      
         //
         return c << "{CWnd title=" << txt 
                  << " visible=" << (wnd.IsWindowVisible() != FALSE)
                  << " enabled=" << (wnd.IsWindowEnabled() != FALSE) 
                  << " position=" << rc
                  << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Determines whether console is visible.</summary>
      /// <returns></returns>
      bool ConsoleWnd::IsVisible() const
      {
         return IsWindowVisible(GetConsoleWindow()) != FALSE;
      }

      /// <summary>Logs an exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      /// <param name="msg">Handler error message</param>
      void  ConsoleWnd::Log(const GuiString& src, const ExceptionBase&e, const GuiString& msg)
      {
         *this << ENDL 
               << Cons::Exception << L"ERROR: " 
               << Cons::Error     << msg.Remove(L"\r\n")
               << Cons::White     << L"...  Source: " 
               << Cons::Yellow    << src << ENDL;
         *this << Cons::Exception << L"CAUSE: " 
               << Cons::Error     << e.Message.Remove(L"\r\n")
               << Cons::White     << L"...  Source: " 
               << Cons::Yellow    << e.Source << ENDL;
      }

      /// <summary>Logs an exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  ConsoleWnd::Log(const GuiString& src, const ExceptionBase&e)
      {
         *this << ENDL 
               << Cons::Exception << L"EXCEPTION: " 
               << Cons::Error     << e.Message.Remove(L"\r\n") 
               << Cons::White     << L"...  Source: " 
               << Cons::Yellow    << src << ENDL;
         *this << Cons::Exception << L"SOURCE: " 
               << Cons::Yellow    << e.Source << ENDL;
      }

      /// <summary>Logs an STL exception to the console.</summary>
      /// <param name="src">Handler location</param>
      /// <param name="e">error</param>
      void  ConsoleWnd::Log(const GuiString& src, const exception&e)
      {
         *this << ENDL 
               << Cons::Exception << L"STL EXCEPTION: " 
               << Cons::Error     << e.what()
               << Cons::White     << L"...  Source: " 
               << Cons::Yellow    << src << ENDL;
      }

      /// <summary>Shows/Hides the console.</summary>
      /// <param name="show">The show.</param>
      void  ConsoleWnd::Show(bool show)
      {
         ShowWindow(GetConsoleWindow(), show ? SW_SHOW : SW_HIDE);
      }

      /// <summary>Inserts associated text colour manipulator, if any</summary>
      /// <param name="c">Colour</param>
      ConsoleWnd& ConsoleWnd::operator<<(Colour c)
      {
         switch (c)
         {
         // Supported
         case Colour::Blue:   return *this << Cons::Blue;
         case Colour::Green:  return *this << Cons::Green;
         case Colour::Red:    return *this << Cons::Red;
         case Colour::Cyan:   return *this << Cons::Cyan;
         case Colour::Purple: return *this << Cons::Purple;
         case Colour::Orange:
         case Colour::Yellow: return *this << Cons::Yellow;
         case Colour::Default:
         case Colour::Silver:
         case Colour::Black:
         case Colour::White:  return *this << Cons::White;
         }

         // Unsupported
         return *this;
      }

      /// <summary>Inserts text manipulator</summary>
      /// <param name="cl">manipulator</param>
      ConsoleWnd& ConsoleWnd::operator<<(Cons c)
      {
         switch (c)
         {
         // Bold: Add bold
         case Cons::Bold:   
            Attributes = Attributes|FOREGROUND_INTENSITY; 
            break;

         // Normal: Remove bold
         case Cons::Normal: 
            Attributes = Attributes & ~FOREGROUND_INTENSITY; 
            break;

         // Reset: White + Normal
         case Cons::Reset:
            return *this << Cons::Normal << Cons::White;

         // Endl: Reset + CRLF 
         case Cons::Endl:   
            return *this << Cons::Reset << L"\n";

         // Heading: CRLF + Cyan
         case Cons::Heading:  
            return *this << ENDL << Cons::Cyan;

         // User Action: CRLF + Bold + Cyan
         case Cons::UserAction:  
            return *this << ENDL << Cons::Bold << Cons::Cyan;

         // Exception: Bold + Purple
         case Cons::Exception:  
            return *this << Cons::Bold << Cons::Purple;

         // Error: Bold + Red
         case Cons::Error:  
            return *this << Cons::Bold << Cons::Red;

         // Success: Bold + Green + "Success!"
         case Cons::Success:  
            return *this << Cons::Bold << Cons::Green << "Success! ";

         // Failure: Bold + Red + "Failed: "
         case Cons::Failure:  
            return *this << Cons::Bold << Cons::Red << "Failed: ";

         // Warning: CRLF + Bold + Yellow + "WARNING: "
         case Cons::Warning:
            return *this << ENDL << Cons::Bold << Cons::Yellow << "WARNING: " << Cons::White;

         // Push attributes: Save current attributes
         case Cons::Push:
            AttributeStack.push_back(Attributes);
            return *this;

         // Pop attributes: Restore previously saved attributes 
         case Cons::Pop:
            if (!AttributeStack.empty())
            {
               Attributes = AttributeStack.back();
               AttributeStack.pop_back();
            }
            return *this;

         // Colour
         default:
            WORD bold = (Attributes & FOREGROUND_INTENSITY);

            switch (c)
            {
            case Cons::Blue:   Attributes = bold|FOREGROUND_BLUE;                   break;
            case Cons::Green:  Attributes = bold|FOREGROUND_GREEN;                  break;
            case Cons::Red:    Attributes = bold|FOREGROUND_RED;                    break;
            case Cons::Cyan:   Attributes = bold|FOREGROUND_BLUE|FOREGROUND_GREEN;  break;
            case Cons::Purple: Attributes = bold|FOREGROUND_BLUE|FOREGROUND_RED;    break;
            case Cons::Yellow: Attributes = bold|FOREGROUND_GREEN|FOREGROUND_RED;   break;
            case Cons::White:  Attributes = bold|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE; break;
            }
         }

         return *this;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& ConsoleWnd::operator<<(const WCHAR* txt)
      {
         WriteText(txt);
         return *this;
      }

      /// <summary>Writes ansi text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& ConsoleWnd::operator<<(const char* txt)
      {
         return *this << GuiString::Convert(txt, CP_ACP);
      }

      /// <summary>Writes a character to the console</summary>
      /// <param name="ch">Character</param>
      ConsoleWnd& ConsoleWnd::operator<<(wchar ch)
      {
         wchar buf[2] = {ch, NULL};
         return *this << buf;
      }

      /// <summary>Writes signed integer to the console</summary>
      /// <param name="i">Number</param>
      ConsoleWnd& ConsoleWnd::operator<<(int i)
      {
         Writef(L"%d", i);
         return *this;
      }

      /// <summary>Writes unsigned integer to the console</summary>
      /// <param name="i">number</param>
      ConsoleWnd& ConsoleWnd::operator<<(UINT i)
      {
         Writef(L"%u", i);
         return *this;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& ConsoleWnd::operator<<(const wstring& txt)
      {
         WriteText(txt);
         return *this;
      }

      /// <summary>Writes ansi text to the console</summary>
      /// <param name="txt">Text</param>
      ConsoleWnd& ConsoleWnd::operator<<(const string& txt)
      {
         return *this << GuiString::Convert(txt, CP_ACP);
      }

      /// <summary>Writes pointer to the console</summary>
      /// <param name="p">Pointer</param>
      ConsoleWnd& ConsoleWnd::operator<<(const void* p)
      {
         Writef(L"0x%x", p);
         return *this;
      }

      /// <summary>Writes a path to the console in yellow</summary>
      /// <param name="path">path</param>
      ConsoleWnd& ConsoleWnd::operator<<(const Path& path)
      {
         return *this << Cons::Push << Cons::Yellow << path.c_str() << Cons::Pop;
      }

      /// <summary>Writes a game version string to the console in yellow</summary>
      /// <param name="str">game version string</param>
      ConsoleWnd& ConsoleWnd::operator<<(const VersionString& str)
      {
         return *this << Cons::Push << Cons::Yellow << str.c_str() << Cons::Pop;
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  ConsoleWnd::Write(const wstring& txt)
      {
         WriteText(txt);
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  ConsoleWnd::Writef(const WCHAR* format, ...)
      {
         va_list args;
         WriteText( GuiString::FormatV(format, va_start(args, format)) );
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  ConsoleWnd::Writef(const wstring& format, ...)
      {
         va_list args;
         WriteText( GuiString::FormatV(format.c_str(), va_start(args, format)) );
      }

      /// <summary>Writes text to the console</summary>
      /// <param name="txt">Text</param>
      void  ConsoleWnd::WriteLn(const wstring& txt /*= L""*/)
      {
         WriteText(txt);
         WriteText(L"\n");
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void ConsoleWnd::WriteLnf(const WCHAR* format /*= L""*/, ...)
      {
         va_list args;
         WriteText( GuiString::FormatV(format, va_start(args, format)) );
         WriteText(L"\n");
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      void  ConsoleWnd::WriteLnf(const wstring& format, ...)
      {
         va_list args;
         WriteText( GuiString::FormatV(format.c_str(), va_start(args, format)) );
         WriteText(L"\n");
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Gets the attributes.</summary>
      /// <returns></returns>
      WORD ConsoleWnd::GetAttributes()
      {
         CONSOLE_SCREEN_BUFFER_INFO info;

         // Ensure connected
         if (Handle == INVALID_HANDLE_VALUE)
            return 0;
         
         // Get attributes
         GetConsoleScreenBufferInfo(Handle, &info);
         return info.wAttributes;
      }

      /// <summary>Sets the attributes.</summary>
      /// <param name="attr">The attribute.</param>
      void ConsoleWnd::SetAttributes(WORD attr)
      {
         if (Handle != INVALID_HANDLE_VALUE)
            SetConsoleTextAttribute(Handle, attr);
      }

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
}
