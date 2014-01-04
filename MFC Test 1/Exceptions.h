#pragma once
#include "stl.h"
#include "Utils.h"

namespace Library
{

   class ExceptionBase : public std::exception
   {
   public:
      ExceptionBase(UINT  err, wstring msg) : ErrorID(err), Message(msg) 
      {}

      UINT     ErrorID;
      wstring  Message;
   };
   

   class ComException : public ExceptionBase
   {
   public:
      ComException(_com_error&  err) : ExceptionBase(err.Error(), err.ErrorMessage())
      {}

      HRESULT  ErrorID;
   };


   /// <summary>Occurs when a game data file is not in the expected format</summary>
   class FileFormatException : public ExceptionBase
   {
   public:

      /// <summary>Initializes a new instance of the <see cref="FileFormatException"/> class.</summary>
      /// <param name="path">Full file path</param>
      /// <param name="msg">Error id</param>
      /// <param name="msg">Error message</param>
      FileFormatException(Path path, UINT err, wstring msg) : ExceptionBase(err, msg), FullPath(path)
      {}

      /// <summary>Creates an exception with a localised message</summary>
      /// <param name="path">Full file path</param>
      /// <param name="msg">error message id</param>
      /// <param name="">Arguments</param>
      /// <returns></returns>
      static FileFormatException  Build(Path path, UINT msg, ...)
      {
         va_list pArgs;
         return FileFormatException(path, msg, StringResource::LoadV(msg, va_start(pArgs, msg)) );
      }

      Path FullPath;
   };


   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class InvalidValueException : public ExceptionBase
   {
   public:

      /// <summary>Initializes a new instance of the <see cref="InvalidValueException"/> class.</summary>
      /// <param name="msg">Error id</param>
      /// <param name="msg">Error message</param>
      InvalidValueException(UINT err, wstring msg) : ExceptionBase(err, msg)
      {}

      /// <summary>Creates an exception with a localised message</summary>
      /// <param name="msg">error message id</param>
      /// <param name="">Arguments</param>
      /// <returns></returns>
      static InvalidValueException  Build(UINT msg, ...)
      {
         va_list pArgs;
         return InvalidValueException(msg, StringResource::LoadV(msg, va_start(pArgs, msg)) );
      }

   };

   /// <summary>Occurs when there an error accessing a file</summary>
   class IOException : public ExceptionBase
   {
   public:

      /// <summary>Initializes a new instance of the <see cref="IOException"/> class.</summary>
      /// <param name="path">Full file path</param>
      /// <param name="msg">Error id</param>
      /// <param name="msg">Error message</param>
      IOException(Path path, UINT err, wstring msg) : ExceptionBase(err, msg), FullPath(path)
      {}

      /// <summary>Creates an exception with a localised message</summary>
      /// <param name="path">Full file path</param>
      /// <param name="msg">error message id</param>
      /// <param name="">Arguments</param>
      /// <returns></returns>
      static IOException  Build(Path path, UINT msg, ...)
      {
         va_list pArgs;
         return IOException(path, msg, StringResource::LoadV(msg, va_start(pArgs, msg)) );
      }

      Path FullPath;
   };
   

}
