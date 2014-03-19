#pragma once
#include "XZip.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary>XZip exception</summary>
      class LogicExport XZipException : public ExceptionBase
      {
      public:
         /// <summary>Create XZip exception from an XZip error code</summary>
         /// <param name="src">The source.</param>
         /// <param name="code">The code.</param>
         XZipException(const wstring& src, ZRESULT code) : ExceptionBase(src, code, L"")
         {
            char buf[128];
            FormatZipMessageZ(code, buf, 128);
            Message = GuiString::Convert(buf, CP_ACP);
         }

         /// <summary>Create XZip exception from a custom message</summary>
         /// <param name="src">The source.</param>
         /// <param name="msg">message.</param>
         XZipException(const wstring& src, const wstring& msg) : ExceptionBase(src, msg)
         {
         }
      };

      
      /// <summary></summary>
      class LogicExport ZipFile
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------

      public:
         ZipFile(const Path& p);
         virtual ~ZipFile();

         NO_COPY(ZipFile);	// No copy semantics
         NO_MOVE(ZipFile);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void Add(const Path& f, const wstring& name);
         void Close();

         // -------------------- REPRESENTATION ---------------------
      protected:
         HZIP Handle;
      };

   }
}

using namespace Logic::IO;
