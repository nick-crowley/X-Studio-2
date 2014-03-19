#pragma once

#include "../Logic/LanguagePage.h"

namespace GUI
{
   namespace Utils
   {
      /// <summary>Clipboard singleton</summary>
      #define theClipboard  Clipboard::Instance

      /// <summary>
      /// Due to having to maintain and delete global memory allocations when using private clipboard formats,
      /// this class just provides the illusion of clipboard functionality
      /// </summary>
      class Clipboard
      {
         // ------------------------ TYPES --------------------------
      protected:
         typedef unique_ptr<LanguageString>  LanguageStringPtr;
         typedef unique_ptr<LanguagePage>  LanguagePagePtr;

         // --------------------- CONSTRUCTION ----------------------
      private:
         Clipboard();
         ~Clipboard();

         NO_COPY(Clipboard);	// No copy semantics
         NO_MOVE(Clipboard);	// No move semantics

      public:
         //PROPERTY_GET_SET(LanguageString*,String,GetLanguageString,SetLanguageString);

         // ------------------------ STATIC -------------------------
      

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Copies a language Page to the clipboard.</summary>
         /// <param name="str">The Page.</param>
         LanguagePage  GetLanguagePage() const
         {
            return *PageData.get();     //return GetData<LanguagePage*>(CF_LANGUAGE_Page);
         }

         /// <summary>Copies a language string to the clipboard.</summary>
         /// <param name="str">The string.</param>
         LanguageString  GetLanguageString() const
         {
            return *StringData.get();     //return GetData<LanguageString*>(CF_LANGUAGE_STRING);
         }

         /// <summary>Query whether clipboard contains a language Page</summary>
         /// <returns></returns>
         bool  HasLanguagePage() const
         {
            return (bool)PageData;      //return IsClipboardFormatAvailable(CF_LANGUAGE_Page) != FALSE;
         }

         /// <summary>Query whether clipboard contains a language string</summary>
         /// <returns></returns>
         bool  HasLanguageString() const
         {
            return (bool)StringData;      //return IsClipboardFormatAvailable(CF_LANGUAGE_STRING) != FALSE;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Clears the clipboard.</summary>
         void  Clear()
         {
            StringData.reset();
            PageData.reset();
            
            // Clear
            EmptyClipboard();
         }
         
         /// <summary>Copies a language Page to the clipboard.</summary>
         /// <param name="str">The Page.</param>
         void  SetLanguagePage(LanguagePageRef str)
         {
            Clear();
            PageData.reset(new LanguagePage(str));      //SetData<const LanguagePage*>(CF_LANGUAGE_Page, str);
         }

         /// <summary>Copies a language string to the clipboard.</summary>
         /// <param name="str">The string.</param>
         void  SetLanguageString(LanguageStringRef str)
         {
            Clear();
            StringData.reset(new LanguageString(str));      //SetData<const LanguageString*>(CF_LANGUAGE_STRING, str);
            SetString(str.ToXML());
         }
         
      protected:
         /// <summary>Frees global memory handle.</summary>
         void  Free()
         {
            if (Content)
            {
               GlobalFree(Content);
               Content = nullptr;
            }
         }

         /// <summary>Gets data from the clipboard.</summary>
         /// <typeparam name="DATA">Data format</typeparam>
         /// <param name="format">clipboard format.</param>
         /// <returns>Data</returns>
         //template<typename DATA>
         //DATA  GetData(UINT format) const
         //{
         //   HANDLE handle = nullptr;
         //   DATA*  buffer = nullptr;

         //   try
         //   {
         //      // Open clipboard
         //      if (!OpenClipboard(theApp.m_pMainWnd))
         //         throw Win32Exception(HERE, L"Unable to open clipboard");
         //   
         //      // Allocate+Open buffer
         //      if ((handle = GetClipboardData(format)) == nullptr)
         //         throw Win32Exception(HERE, L"No clipboard data in the desired format");

         //      // Copy data
         //      if ((buffer = (DATA*)GlobalLock(handle)) == nullptr)
         //         throw Win32Exception(HERE, L"Unable to allocate global memory");
         //      DATA obj(*buffer);
         //      
         //      // Close
         //      GlobalUnlock(handle);
         //      CloseClipboard();
         //      return obj;
         //   }
         //   catch (...) 
         //   {
         //      // Cleanup before exit
         //      if (handle)
         //         GlobalUnlock(handle);
         //      CloseClipboard();
         //      throw;
         //   }
         //}

         /// <summary>Sets clipboard data.</summary>
         /// <typeparam name="DATA">Data format</typeparam>
         /// <param name="format">format.</param>
         /// <param name="obj">object.</param>
         //template<typename DATA>
         //void  SetData(UINT format, DATA obj)
         //{
         //   DATA*  buffer = nullptr;

         //   try
         //   {
         //      // Clear previous data
         //      if (Content)
         //         Free();

         //      // Open clipboard
         //      if (!OpenClipboard(theApp.m_pMainWnd))
         //         throw Win32Exception(HERE, L"Unable to open clipboard");

         //      // Allocate+Open buffer
         //      if ( !(Content = GlobalAlloc(GMEM_MOVEABLE, sizeof(DATA))) || !(buffer = GlobalLock(Content)) )
         //         throw Win32Exception(HERE, L"Unable to allocate global memory");

         //      // Copy output to the clipboard buffer
         //      *buffer = obj;
         //      GlobalUnlock(Content);
         //      
         //      // Transfer to clipboard
         //      if (!SetClipboardData(format, Content))
         //         throw Win32Exception(HERE, L"Unable to set clipboard data");

         //      // Close
         //      CloseClipboard();
         //   }
         //   catch (...) 
         //   {
         //      // Cleanup before exit
         //      Free();
         //      CloseClipboard();
         //      throw;
         //   }
         //}

         /// <summary>Copies a string to the clipboard.</summary>
         void  SetString(const wstring& txt);

         // -------------------- REPRESENTATION ---------------------
      public:
         /// <summary>Singleton instance</summary>
         static Clipboard  Instance;

         /// <summary>Private clipboard formats</summary>
         const static UINT  CF_LANGUAGE_STRING = CF_GDIOBJFIRST,
                            CF_LANGUAGE_PAGE   = (CF_GDIOBJFIRST+1);

      protected:
         /// <summary>Clipboard data</summary>
         LanguageStringPtr  StringData;
         LanguagePagePtr    PageData;
         HANDLE             Content;
      };

   }
}

using namespace GUI::Utils;
