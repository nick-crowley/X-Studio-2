#pragma once
#include "Logic/Common.h"
#include "Logic/LanguagePage.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class Clipboard
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      private:
         Clipboard();
         virtual ~Clipboard();
         
         NO_COPY(Clipboard);	// No copy semantics
         NO_MOVE(Clipboard);	// No move semantics

         // ------------------------ STATIC -------------------------
      public:
         /// <summary>Copies a language string to the clipboard.</summary>
         /// <param name="str">The string.</param>
         static void  CopyString(const LanguageString* str)
         {
            REQUIRED(str);
            SetData<const LanguageString*>(CF_LANGUAGE_STRING, str);
         }
         
         /// <summary>Copies a language string to the clipboard.</summary>
         /// <param name="str">The string.</param>
         static LanguageString*  PasteString()
         {
            return GetData<LanguageString*>(CF_LANGUAGE_STRING);
         }
         

      protected:
         /// <summary>Private clipboard formats</summary>
         static const UINT CF_LANGUAGE_STRING = CF_GDIOBJFIRST + 1;     // Tells System to delete handles

         /// <summary>Gets data from the clipboard.</summary>
         /// <param name="format">format.</param>
         /// <returns>Data</returns>
         template<typename T>
         static T  GetData(UINT format)
         {
            HANDLE handle = nullptr;
            void*  buffer = nullptr;
            T      object;

            try
            {
               // Open clipboard
               if (!OpenClipboard(AfxGetMainWnd()))
                  throw Win32Exception(HERE, L"Unable to open clipboard");
            
               // Allocate+Open buffer
               if ((handle = GetClipboardData(format)) == nullptr)
                  throw Win32Exception(HERE, L"No clipboard data in the desired format");

               // Extract data
               if ((buffer = GlobalLock(handle)) == nullptr)
                  throw Win32Exception(HERE, L"Unable to allocate global memory");

               // Copy
               memcpy(object, buffer, sizeof(T));
               GlobalUnlock(handle);

               // Close
               CloseClipboard();
               return object;
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
         static void  SetData(UINT format, T obj)
         {
            HANDLE handle = nullptr;
            void*  buffer = nullptr;

            try
            {
               // Open clipboard
               if (!OpenClipboard(AfxGetMainWnd()))
                  throw Win32Exception(HERE, L"Unable to open clipboard");

               // Clear
               EmptyClipboard();
            
               // Allocate+Open buffer
               if ( !(handle = GlobalAlloc(GMEM_MOVEABLE, sizeof(T))) || !(buffer = GlobalLock(handle)) )
               if (!handle || !buffer)
                  throw Win32Exception(HERE, L"Unable to allocate global memory");

               // Copy output to the clipboard buffer
               memcpy(buf, &obj, sizeof(T));
      
               // Transfer to clipboard
               if (!GlobalUnlock(handle) || !SetClipboardData(format, handle))
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

         

         //static list<LanguageStringPtr>  Data;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace GUI::Utils;
