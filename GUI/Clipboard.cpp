#pragma once
#include "stdafx.h"
#include "Clipboard.h"
#include <strsafe.h>

namespace GUI
{
   namespace Utils
   {
      /// <summary>Singleton instance</summary>
      Clipboard  Clipboard::Instance;

      /// <summary>Clipboard data</summary>
      /*Clipboard::LanguageStringPtr  Clipboard::StringData;
      Clipboard::LanguagePagePtr    Clipboard::PageData;*/

      // -------------------------------- CONSTRUCTION --------------------------------

      Clipboard::Clipboard() : Content(Content)
      {}

      Clipboard::~Clipboard() 
      {
         Free();
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Copies a string to the clipboard.</summary>
      /// <param name="txt">The text.</param>
      void  Clipboard::SetString(const wstring& txt)
      {
         wchar* buffer = nullptr;

         try
         {
            // Open clipboard
            if (!OpenClipboard(theApp.m_pMainWnd->m_hWnd))
               throw Win32Exception(HERE, L"Unable to open clipboard");

            // Allocate+Open buffer
            if ( !(Content = GlobalAlloc(GMEM_MOVEABLE, (txt.length()+1)*sizeof(wchar))) || !(buffer = (wchar*)GlobalLock(Content)) )
               throw Win32Exception(HERE, L"Unable to allocate global memory");

            // Copy output to the clipboard buffer
            StringCchCopy(buffer, txt.length()+1, txt.c_str());
            GlobalUnlock(Content);
               
            // Transfer to clipboard
            if (!SetClipboardData(CF_UNICODETEXT, Content))
               throw Win32Exception(HERE, L"Unable to set clipboard data");

            // Close
            CloseClipboard();
         }
         catch (...) 
         {
            // Cleanup before exit
            Free();
            CloseClipboard();
            throw;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------

   }
}
