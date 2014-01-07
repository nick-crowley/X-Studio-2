#pragma once
#include "Utils.h"

namespace Library
{

   #define  HERE   __WFUNCTION__

   class ExceptionBase : public std::exception
   {
   protected:
      /// <summary>Create a exception with an error code but no message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      ExceptionBase(wstring  src, UINT  err) : ErrorID(err), Source(src)
      {
      }

      /// <summary>Create a exception without an error code but with a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, wstring msg) : ErrorID(0), Message(msg), Source(src)
      {
      }

      /// <summary>Create a exception with an error code and message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, UINT  err, wstring msg) : ErrorID(err), Message(msg), Source(src)
      {}

   public:
      virtual ~ExceptionBase()  {}

      UINT     ErrorID;
      wstring  Message,
               Source;
   };
   

   #define REQUIRED(arg)  { if ((arg) == NULL) throw ArgumentNullException(HERE, WIDEN(#arg)); }

   /// <summary>Occurs when a missing argument is detected</summary>
   class ArgumentNullException : public ExceptionBase
   {
   public:

      /// <summary>Create an ArgumentNullException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      ArgumentNullException(wstring  src, const WCHAR* arg) : ExceptionBase(src, (UINT)0)
      {
         // "Missing '%s' argument"
         Message = StringResource::Load(ERR_ARGUMENT_NULL, arg); 
      }
   };

   /// <summary>Occurs when an argument is invalid</summary>
   class ArgumentException : public ExceptionBase
   {
   public:

      /// <summary>Create an ArgumentException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      /// <param name="msg">Debugging message</param>
      ArgumentException(wstring  src, wstring  arg, wstring  msg) : ExceptionBase(src, (UINT)0)
      {
         // "The '%s' argument is invalid : %s"
         Message = StringResource::Format(L"The '%s' argument is invalid : %s", arg.c_str(), msg.c_str()); 
      }

      /// <summary>Create an ArgumentException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      /// <param name="err">Error ID</param>
      ArgumentException(wstring  src, wstring  arg, UINT  err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         
         // "The '%s' argument is invalid : %s"
         wstring msg = StringResource::LoadV(err, va_start(pArgs, err));
         Message = StringResource::Format(L"The '%s' argument is invalid : %s", arg.c_str(), msg.c_str()); 
      }
   };
   
   /// <summary>Occurs when a COM object is used incorrectly</summary>
   class ComException : public ExceptionBase
   {
   public:
      /// <summary>Create a ComException from a _com_error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, _com_error&  err) 
         : ExceptionBase(src, err.Error(), wstring(L"COM object error: ") + err.ErrorMessage())
      {}
   };

   /// <summary>Occurs when zlib encounters an error</summary>
   class GZipException : public ExceptionBase
   {
   public:
      /// <summary>Create a GZipException from a zlib message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">ZLib message</param>
      GZipException(wstring  src, const char*  msg) : ExceptionBase(src, (UINT)0)
      {
         if (msg)
         {
            string sz(msg);
            Message = wstring(L"Error in zlib library: ") + wstring(sz.begin(), sz.end());
         }
      }
   };

   

   /// <summary>Occurs when a file is not found</summary>
   class DirectoryNotFoundException : public ExceptionBase
   {
   public:

      /// <summary>Create a DirectoryNotFoundException for a file path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">Folder path</param>
      DirectoryNotFoundException(wstring  src, Path  folder) : ExceptionBase(src, (UINT)0)
      {
         Message = StringResource::Format(L"The folder '%s' does not exist", folder.c_str()); 
      }

   };


   /// <summary>Occurs when a game data file is not in the expected format</summary>
   class FileFormatException : public ExceptionBase
   {
   public:

      /// <summary>Create a FileFormatException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      FileFormatException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = wstring(L"The file format is invalid: ") + StringResource::LoadV(err, va_start(pArgs, err)); 
      }

      /// <summary>Create an FileFormatException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      FileFormatException(wstring  src, wstring  debug) 
         : ExceptionBase(src, wstring(L"The file format is invalid: ") + debug)
      {
      }
   };


   /// <summary>Occurs when a file is not found</summary>
   class FileNotFoundException : public ExceptionBase
   {
   public:

      /// <summary>Create a FileNotFoundException for a file path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">File path</param>
      FileNotFoundException(wstring  src, Path  file) : ExceptionBase(src, (UINT)0)
      {
         Message = StringResource::Format(L"The file '%s' does not exist", file.c_str()); 
      }

   };

   /// <summary>Occurs when an invalid operation is attempted</summary>
   class InvalidOperationException : public ExceptionBase
   {
   public:

      /// <summary>Create an InvalidOperationException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      InvalidOperationException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = wstring(L"The operation is invalid: ") + StringResource::LoadV(err, va_start(pArgs, err)); 
      }

      /// <summary>Create an InvalidOperationException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      InvalidOperationException(wstring  src, wstring  debug, ...) 
         : ExceptionBase(src, wstring(L"The operation is invalid: ") + debug)
      {
      }
   };

   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class InvalidValueException : public ExceptionBase
   {
   public:

      /// <summary>Create an InvalidValueException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      InvalidValueException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = wstring(L"Invalid game data value: ") + StringResource::LoadV(err, va_start(pArgs, err)); 
      }
   };


   /// <summary>Occurs when an operation that has not been implemented is attempted</summary>
   class NotImplementedException : public ExceptionBase
   {
   public:

      /// <summary>Create an NotImplementedException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      NotImplementedException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = wstring(L"The operation has not been implemented: ") + StringResource::LoadV(err, va_start(pArgs, err)); 
      }

      /// <summary>Create an NotImplementedException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      NotImplementedException(wstring  src, wstring  debug, ...) 
         : ExceptionBase(src, wstring(L"The operation has not been implemented: ") + debug)
      {
      }
   };

   /// <summary>Occurs when an unsupported operation is attempted</summary>
   class NotSupportedException : public ExceptionBase
   {
   public:

      /// <summary>Create an NotSupportedException from a resource ID</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      NotSupportedException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = wstring(L"The operation is not supported: ") + StringResource::LoadV(err, va_start(pArgs, err)); 
      }

      /// <summary>Create an NotSupportedException from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      NotSupportedException(wstring  src, wstring  debug, ...) 
         : ExceptionBase(src, wstring(L"The operation is invalid: ") + debug)
      {
      }
   };


   /// <summary>Occurs when a win32 API call fails</summary>
   class Win32Exception : public ExceptionBase
   {
   public:
      /// <summary>Create an Win32Exception from a debugging message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      Win32Exception(wstring  src, wstring  debug) 
         : ExceptionBase(src, wstring(L"Win32API method failed: ") + debug)
      {
      }

      /// <summary>Create an Win32Exception from a system error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">System error id</param>
      Win32Exception(wstring  src, UINT err) : ExceptionBase(src, err)
      {
         Message = StringResource::FormatMessage(err);
      }

      /// <summary>Create an Win32Exception from the last system error</summary>
      /// <param name="src">Location of throw</param>
      Win32Exception(wstring  src) : ExceptionBase(src, GetLastError())
      {
         Message = StringResource::FormatMessage(ErrorID);
      }
   };
}