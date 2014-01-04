#pragma once
#include "stl.h"
#include "Utils.h"
#include <Windows.h>

namespace Library
{
   #define  HERE   L"TODO"

   class ExceptionBase : public std::exception
   {
   protected:
      /// <summary>Create a exception without a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      ExceptionBase(wstring  src, UINT  err) : ErrorID(err), Source(src)
      {
      }

      /// <summary>Create a exception with a custom error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, UINT  err, wstring msg) : ErrorID(err), Message(msg), Source(src)
      {}

   public:
      virtual ~ExceptionBase()  {}

   protected:
      /// <summary>Lookup the message from the system</summary>
      void  LookupMessage()
      {
         WCHAR* szBuffer;
         if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ErrorID, NULL, (WCHAR*)&szBuffer, 512, NULL))
         {
            Message = szBuffer;
            LocalFree(szBuffer);
         }
      }

   public:
      UINT     ErrorID;
      wstring  Message,
               Source;
   };
   

   class ComException : public ExceptionBase
   {
   public:
      /// <summary>Create a ComException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, _com_error&  err) : ExceptionBase(src, err.Error(), err.ErrorMessage())
      {}
   };


   /// <summary>Occurs when a game data file is not in the expected format</summary>
   class FileFormatException : public ExceptionBase
   {
   public:

      /// <summary>Create a FileFormatException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      FileFormatException(wstring  src, UINT err, wstring msg, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, msg)); 
      }

   };


   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class InvalidValueException : public ExceptionBase
   {
   public:

      /// <summary>Create an InvalidValueException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      InvalidValueException(wstring  src, UINT err, wstring msg, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, msg)); 
      }
   };

   /// <summary>Occurs when there an error accessing a file</summary>
   class IOException : public ExceptionBase
   {
   public:

      /// <summary>Create an IOException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      IOException(wstring  src, UINT err, wstring msg, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, msg)); 
      }

      /// <summary>Create an IOException from a system error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">System error id</param>
      IOException(wstring  src, UINT err) : ExceptionBase(src, err)
      {
         LookupMessage();
      }
   };
   

}
