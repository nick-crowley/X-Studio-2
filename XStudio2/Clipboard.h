#pragma once
#include "Logic/Common.h"
#include "Logic/LanguagePage.h"

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
         Clipboard() {}
         ~Clipboard() {}
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
         }
         
      protected:
         /// <summary>Gets data from the clipboard.</summary>
         /// <param name="format">format.</param>
         /// <returns>Data</returns>
         template<typename T>
         T  GetData(UINT format) const
         {
            HANDLE handle = nullptr;
            void*  buffer = nullptr;

            try
            {
               // Open clipboard
               if (!OpenClipboard(theApp.m_pMainWnd))
                  throw Win32Exception(HERE, L"Unable to open clipboard");
            
               // Allocate+Open buffer
               if ((handle = GetClipboardData(format)) == nullptr)
                  throw Win32Exception(HERE, L"No clipboard data in the desired format");

               // Extract data
               if ((buffer = GlobalLock(handle)) == nullptr)
                  throw Win32Exception(HERE, L"Unable to allocate global memory");

               // Copy
               BYTE object[sizeof(T)];
               memcpy(object, buffer, sizeof(T));
               GlobalUnlock(handle);

               // Close
               CloseClipboard();
               return *reinterpret_cast<T*>(object);
            }
            catch (...) 
            {
               // Cleanup before exit
               if (handle)
                  GlobalUnlock(handle);
               CloseClipboard();
               throw;
            }
         }

         /// <summary>Sets clipboard data.</summary>
         /// <param name="format">format.</param>
         /// <param name="obj">object.</param>
         template<typename T>
         void  SetData(UINT format, T obj)
         {
            HANDLE handle = nullptr;
            void*  buffer = nullptr;

            try
            {
               // Open clipboard
               if (!OpenClipboard(theApp.m_pMainWnd))
                  throw Win32Exception(HERE, L"Unable to open clipboard");

               // Clear
               EmptyClipboard();
            
               // Allocate+Open buffer
               if ( !(handle = GlobalAlloc(GMEM_MOVEABLE, sizeof(T))) || !(buffer = GlobalLock(handle)) )
                  throw Win32Exception(HERE, L"Unable to allocate global memory");

               // Copy output to the clipboard buffer
               memcpy(buffer, &obj, sizeof(T));
               GlobalUnlock(handle);
               
               // Transfer to clipboard
               if (!SetClipboardData(format, handle))
                  throw Win32Exception(HERE, L"Unable to set clipboard data");

               // Close
               CloseClipboard();
            }
            catch (...) 
            {
               // Cleanup before exit
               if (handle)
                  GlobalUnlock(handle);
               CloseClipboard();
               throw;
            }
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         /// <summary>Singleton instance</summary>
         static Clipboard  Instance;

      protected:
         /// <summary>Private clipboard formats</summary>
         const UINT  CF_LANGUAGE_STRING = CF_GDIOBJFIRST;

         /// <summary>Clipboard data</summary>
         static LanguageStringPtr  StringData;
         static LanguagePagePtr    PageData;
      };

   }
}

using namespace GUI::Utils;
