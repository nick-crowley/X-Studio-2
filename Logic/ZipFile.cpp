#include "stdafx.h"
#include "ZipFile.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create zip file</summary>
      /// <param name="p">Full path</param>
      /// <exception cref="Logic::IO::XZipException">Unable to create file handle</exception>
      ZipFile::ZipFile(const Path& p) : Handle(nullptr)
      {
         // Create handle
         if ((Handle = CreateZip((void*)p.c_str(), 0, ZIP_FILENAME)) == nullptr)
            throw XZipException(HERE, ZR_RECENT);
      }

      ZipFile::~ZipFile()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Add file to the archive.</summary>
      /// <param name="path">Fullpath</param>
      /// <param name="name">Subpath displayed within archive.</param>
      /// <exception cref="Logic::IO::XZipException">Unable to add file -or- file already closed</exception>
      void  ZipFile::Add(const Path& path, const wstring& name)
      {
         // Verify state
         if (!Handle)
            throw XZipException(HERE, L"File has been closed");

         // Add file
         if (ZipAdd(Handle, name.c_str(), (void*)path.c_str(), 0, ZIP_FILENAME) != ZR_OK)
            throw XZipException(HERE, ZR_RECENT);
      }

      /// <summary>Writes the file to disc</summary>
      /// <exception cref="Logic::IO::XZipException">Unable to write file</exception>
      void  ZipFile::Close()
      {
         // Close/Write file
         if (Handle && CloseZip(Handle) != ZR_OK)
            throw XZipException(HERE, ZR_RECENT);
         
         // Cleanup
         Handle = nullptr;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

