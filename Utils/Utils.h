#pragma once

/// <summary>User interface</summary>
namespace GUI
{
   /// <summary>Client rectangle</summary>
   class ClientRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ClientRect(CWnd* wnd) : CRect(0,0,0,0)
      {
         wnd->GetClientRect(this);
      }

      DEFAULT_COPY(ClientRect);	// Default copy semantics
      DEFAULT_MOVE(ClientRect);	// Default move semantics

      // -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Control rectangle in parent client co-ordinates</summary>
   class CtrlRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create control rectangle in parent client co-ordinates</summary>
      /// <param name="parent">The parent.</param>
      /// <param name="ctrl">The control.</param>
      CtrlRect(CWnd* parent, CWnd* ctrl) : CRect(0, 0, 0, 0)
      {
         ctrl->GetWindowRect(this);
         parent->ScreenToClient(this);
      }

      DEFAULT_COPY(CtrlRect);	// Default copy semantics
      DEFAULT_MOVE(CtrlRect);	// Default move semantics

   };

   /// <summary>Position of mouse cursor</summary>
   class CursorPoint : public CPoint
   {
   public:
      /// <summary>Create cursor position.</summary>
      /// <param name="wnd">Window for client co=ordinates, or nullptr for screen co-ords</param>
      CursorPoint(const CWnd* wnd = nullptr) : CPoint(0, 0)
      {
         // Get position in screen co-ords
         if (GetCursorPos(this) && wnd)
            // Window: Convert to client co-ords
            ScreenToClient(*wnd, this);
      }
   };

   /// <summary>Window rectangle</summary>
   class WindowRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      WindowRect(CWnd* wnd) : CRect(0,0,0,0)
      {
         wnd->GetWindowRect(this);
      }

      DEFAULT_COPY(WindowRect);	// Default copy semantics
      DEFAULT_MOVE(WindowRect);	// Default move semantics

      // -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Write rectangle to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc);

   /// <summary>Write size to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz);

   /// <summary>Dialog data exchange for std::wstring</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, wstring& value);

   /// <summary>Dialog data exchange for paths</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, IO::Path& path);
}


