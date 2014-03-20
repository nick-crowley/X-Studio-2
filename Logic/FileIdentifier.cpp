#include "stdafx.h"
#include "FileIdentifier.h"
#include "XFileInfo.h"

namespace Logic
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      FileIdentifier::FileIdentifier()
      {
      }


      FileIdentifier::~FileIdentifier()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Identifies whether a file is supported by scanning it's header</summary>
      /// <param name="path">The path.</param>
      /// <returns>Filetype</returns>
      FileType  FileIdentifier::Identify(Path  path)
      {
         try
         {
            StreamPtr    s(XFileInfo(path).OpenRead());
            ByteArrayPtr buf(new BYTE[PEEK_BUFFER]);

            // Peek first 1024 bytes
            DWORD count = s->Read(buf.get(), PEEK_BUFFER);
            buf.get()[min(count,PEEK_BUFFER-1)] = '\0';

            // Check for unique XML document root elements
            if (StrStrIA((char*)buf.get(), "<script>") != nullptr)
               return FileType::Script;
            
            if (StrStrIA((char*)buf.get(), "<language") != nullptr)
               return FileType::Language;

            if (StrStrIA((char*)buf.get(), "<director>") != nullptr)
               return FileType::Mission;

            if (StrStrIA((char*)buf.get(), "<project") != nullptr)
               return FileType::Project;

            if (StrStrIA((char*)buf.get(), "<universe>") != nullptr)
               return FileType::Universe;
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e, VString(L"Unable to identify file '%s'", path.c_str()));
         }

         // Unknown
         return FileType::Unknown;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------

      
   }
}
