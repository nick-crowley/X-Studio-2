#pragma once
#include "Utils.h"

namespace Library
{

   #define  HERE   __WFUNCTION__

   class ExceptionBase : public std::exception
   {
   protected:
      /// <summary>Create a exception without a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      ExceptionBase(wstring  src, UINT  err) : ErrorID(err), Source(src)
      {
      }

      /// <summary>Create a exception without a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, wstring msg) : ErrorID(0), Message(msg), Source(src)
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

      UINT     ErrorID;
      wstring  Message,
               Source;
   };
   

   #define REQUIRED(arg)  { if ((arg) == NULL) throw ArgumentNullException(HERE, WIDEN(#arg)); }

   /// <summary>Occurs when a missing argument is detected</summary>
   class ArgumentNullException : public ExceptionBase
   {
   public:

      /// <summary>Create an ArgumentNullException with a custom error.</summary>
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

      /// <summary>Create an ArgumentException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      ArgumentException(wstring  src, wstring  arg) : ExceptionBase(src, (UINT)0)
      {
         // "Missing '%s' argument"
         Message = StringResource::Load(ERR_ARGUMENT_NULL, arg); 
      }
   };
   
   /// <summary>Occurs when a COM object is used incorrectly</summary>
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
      FileFormatException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, err)); 
      }

   };


   /// <summary>Occurs when an invalid operation is attempted</summary>
   class InvalidOperationException : public ExceptionBase
   {
   public:

      /// <summary>Create an InvalidOperationException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      InvalidOperationException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, err)); 
      }

      /// <summary>Create an InvalidOperationException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="debug">Debug message</param>
      InvalidOperationException(wstring  src, wstring  debug, ...) : ExceptionBase(src, debug)
      {
      }
   };

   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class InvalidValueException : public ExceptionBase
   {
   public:

      /// <summary>Create an InvalidValueException with a custom error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      InvalidValueException(wstring  src, UINT err, ...) : ExceptionBase(src, err)
      {
         va_list pArgs;
         Message = StringResource::LoadV(err, va_start(pArgs, err)); 
      }
   };


   /// <summary>Occurs when there an error accessing a file</summary>
   class Win32Exception : public ExceptionBase
   {
   public:
      /// <summary>Create an IOException from a system error.</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">System error id</param>
      Win32Exception(wstring  src, UINT err) : ExceptionBase(src, err)
      {
         Message = StringResource::FormatMessage(err);
      }
   };
}