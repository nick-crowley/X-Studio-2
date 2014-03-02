#pragma once
#include "D:/My Projects/XStudio2/Resources/Resources.h"

#include "StringResource.h"
#include "Path.h"

namespace Logic
{
   /// <summary>Base class for all exceptions</summary>
   class ExceptionBase : public std::exception
   {
   protected:
      
      /// <summary>Create a exception without an error code but with a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, wstring msg) : ErrorID(0), Message(msg), Source(src)
      {}

      /// <summary>Create a exception with an error code and message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, UINT  err, wstring msg) : ErrorID(err), Message(msg), Source(src)
      {}

   public:
      virtual ~ExceptionBase()  {}

      UINT      ErrorID;
      GuiString Message,
                Source;
   };
   
   /// <summary>Occurs when an algorithm fails or an invariant is violated</summary>
   class AlgorithmException : public ExceptionBase
   {
   public:
      /// <summary>Create an AlgorithmException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      AlgorithmException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"Invariant violated: ") + msg)
      {}
   };
   
   /// <summary>Occurs when a message should be displayed to the user verbatim</summary>
   class ApplicationException : public ExceptionBase
   {
   public:
      /// <summary>Create an ApplicationException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      ApplicationException(wstring  src, wstring  msg) 
         : ExceptionBase(src, msg)
      {}
   };

   /// <summary>Occurs when a missing argument is detected</summary>
   class ArgumentNullException : public ExceptionBase
   {
   public:
      /// <summary>Create an ArgumentNullException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      ArgumentNullException(wstring  src, const WCHAR* arg) 
         // "Missing '%s' argument"
         : ExceptionBase(src, StringResource::Load(ERR_ARGUMENT_NULL, arg))
      {}
   };

   /// <summary>Occurs when an argument is invalid</summary>
   class ArgumentException : public ExceptionBase
   {
   public:
      /// <summary>Create an ArgumentException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      /// <param name="msg">Message</param>
      ArgumentException(wstring  src, wstring  arg, wstring  msg) 
         // "The '%s' argument is invalid : %s"
         : ExceptionBase(src, StringResource::Format(L"The '%s' argument is invalid : %s", arg.c_str(), msg.c_str()) )
      {}
   };
   
   /// <summary>Occurs when a COM object is used incorrectly</summary>
   class ComException : public ExceptionBase
   {
   public:
      /// <summary>Create a ComException from a _com_error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, _com_error&  err) 
         // "COM object error: ...."
         : ExceptionBase(src, err.Error(), wstring(L"COM object error: ") + err.ErrorMessage())
      {}

      /// <summary>Create a ComException from a _com_error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, HRESULT hr) 
         // "COM object error: ...."
         : ExceptionBase(src, hr, wstring(L"COM object error: ") + SysErrorString(hr))
      {}
   };

   /// <summary>Occurs when a file is not found</summary>
   class DirectoryNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a DirectoryNotFoundException from a folder path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">Folder path</param>
      DirectoryNotFoundException(wstring  src, IO::Path  folder) 
         // "The folder '%s' does not exist"
         : ExceptionBase(src, ERROR_PATH_NOT_FOUND, StringResource::Format(L"The folder '%s' does not exist", folder.c_str()) )
      {}
   };


   /// <summary>Occurs when a game data file is not in the expected format</summary>
   class FileFormatException : public ExceptionBase
   {
   public:
      /// <summary>Create an FileFormatException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      FileFormatException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"The file format is invalid: ") + msg)
      {}

      /// <summary>Create an FileFormatException for a specific line</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="line">One based line number</param>
      /// <param name="msg">Message</param>
      FileFormatException(wstring  src, UINT line, wstring  msg) 
         : ExceptionBase(src, GuiString(L"The file format is invalid on line %u: ", line) + msg)
      {}
   };

   /// <summary>Occurs when a file is not found</summary>
   class FileNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a FileNotFoundException for a file path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">File path</param>
      FileNotFoundException(wstring  src, IO::Path  file) 
         : ExceptionBase(src, ERROR_FILE_NOT_FOUND, StringResource::Format(L"The file '%s' does not exist", file.c_str()))
      {}
   };
   
   /// <summary>Occurs when zlib encounters an error</summary>
   class GZipException : public ExceptionBase
   {
   public:
      /// <summary>Create a GZipException from a zlib message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">ZLib message</param>
      GZipException(wstring  src, const char*  msg) 
         : ExceptionBase(src, L"Error in zlib library: ")
      {
         if (msg)
         {
            string sz(msg);
            Message += wstring(sz.begin(), sz.end());
         }
         else
            Message += L"Unspecified";
      }
   };
   
   /// <summary>Occurs when an index is out of range</summary>
   class IndexOutOfRangeException : public ExceptionBase
   {
   public:
      /// <summary>Create an IndexOutOfRangeException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="index">Desired index</param>
      /// <param name="count">Item count</param>
      IndexOutOfRangeException(wstring  src, UINT index, UINT count) 
         : ExceptionBase(src, GuiString(L"The index %d is out of range, only %d objects in collection", index, count))
      {}

      /// <summary>Create an IndexOutOfRangeException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="index">Desired index</param>
      IndexOutOfRangeException(wstring  src, UINT index) 
         : ExceptionBase(src, GuiString(L"The index %d is out of range", index))
      {}
   };

   /// <summary>Occurs when an invalid operation is attempted</summary>
   class InvalidOperationException : public ExceptionBase
   {
   public:
      /// <summary>Create an InvalidOperationException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      InvalidOperationException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"The operation is invalid: ") + msg)
      {}
   };

   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class InvalidValueException : public ExceptionBase
   {
   public:
      /// <summary>Create an InvalidValueException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      InvalidValueException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"Invalid game data value: ") + msg)
      {}
   };

   /// <summary>Occurs when an I/O operator fails</summary>
   class IOException : public ExceptionBase
   {
   public:
      /// <summary>Create an IOException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      IOException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"The I/O operation failed: ") + msg)
      {}
   };

   /// <summary>Occurs when an operation that has not been implemented is attempted</summary>
   class NotImplementedException : public ExceptionBase
   {
   public:
      /// <summary>Create a NotImplementedException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      NotImplementedException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"The operation has not been implemented: ") + msg)
      {}
   };

   /// <summary>Occurs when an unsupported operation is attempted</summary>
   class NotSupportedException : public ExceptionBase
   {
   public:
      /// <summary>Create an NotSupportedException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      NotSupportedException(wstring  src, wstring  msg) 
         : ExceptionBase(src, wstring(L"The operation is invalid: ") + msg)
      {}
   };


   /// <summary>Occurs when a win32 API call fails</summary>
   class Win32Exception : public ExceptionBase
   {
   public:
      /// <summary>Create an Win32Exception from the last system error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      Win32Exception(wstring  src, wstring  msg) 
         : ExceptionBase(src, msg + L" : " + (GetLastError() ? (wstring)SysErrorString() : L"<Subsystem provided no further details>"))
      {}
   };

   
   // ------------------------ USER TYPES --------------------------


   /// <summary>General exception for any circumstance</summary>
   class GenericException : public ExceptionBase
   {
   public:
      /// <summary>Create a GenericException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      GenericException(wstring  src, wstring msg) 
         : ExceptionBase(src, msg)
      {}
   };


   /// <summary>Occurs when a language string is missing</summary>
   class PageNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a PageNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      PageNotFoundException(wstring  src, UINT page) 
         : ExceptionBase(src, GuiString(L"Cannot find page %d", page))
      {}
   };

   /// <summary>Occurs when a language string is missing</summary>
   class StringNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a StringNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      /// <param name="id">string id</param>
      StringNotFoundException(wstring  src, UINT page, UINT id) 
         : ExceptionBase(src, GuiString(L"Cannot find string %d in page %d", id, page))
      {}
   };

   /// <summary>Occurs when a command syntax is missing</summary>
   class SyntaxNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create an SyntaxNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="id">command id</param>
      /// <param name="ver">game version</param>
      SyntaxNotFoundException(wstring  src, UINT id, GameVersion v) 
         : ExceptionBase(src, GuiString(L"Cannot find %s syntax for command with id %d", VersionString(v).c_str(), id))
      {}
   };

   
}