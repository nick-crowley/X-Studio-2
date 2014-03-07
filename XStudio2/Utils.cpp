#include "stdafx.h"
#include "Utils.h"

namespace GUI
{

   // -------------------------------- CONSTRUCTION --------------------------------


   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Write rectangle to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CRect& rc)
   {
      return c << "{CRect left=" << rc.left << " top=" << rc.top << " right=" << rc.right << " bottom=" << rc.bottom << "}";
   }

   /// <summary>Write size to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const CSize& sz)
   {
      return c << "{CSize width=" << sz.cx << " height=" << sz.cy << "}";
   }

   /// <summary>Dialog data exchange for std::wstring</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, wstring& value)
   {
	   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
      CString str;

	   if (pDX->m_bSaveAndValidate)
	   {
		   int nLen = ::GetWindowTextLength(hWndCtrl);
		   ::GetWindowText(hWndCtrl, str.GetBufferSetLength(nLen), nLen+1);
		   str.ReleaseBuffer();
         value = (LPCWSTR)str;
	   }
	   else
	   {
		   AfxSetWindowText(hWndCtrl, value.c_str());
	   }
   }

   /// <summary>Dialog data exchange for paths</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, IO::Path& path)
   {
	   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);

	   if (pDX->m_bSaveAndValidate)
	      ::GetWindowText(hWndCtrl, (wchar*)path, MAX_PATH);
	   else
	      AfxSetWindowText(hWndCtrl, path.c_str());
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

}

