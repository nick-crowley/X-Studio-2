#pragma once
#include "GuiString.h"
#include "Path.h"

namespace Logic
{
   /// <summary>Base class UtilExport for all exceptions</summary>
   class UtilExport ExceptionBase : public std::exception
   {
   protected:
      /// <summary>Create a exception without an error code but with a message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, wstring msg);

      /// <summary>Create a exception with an error code and message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">Error id</param>
      /// <param name="msg">Error message</param>
      ExceptionBase(wstring  src, UINT  err, wstring msg);

   public:
      virtual ~ExceptionBase();

      UINT      ErrorID;
      GuiString Message,
                Source;
   };
   
   /// <summary>Occurs when an algorithm fails or an invariant is violated</summary>
   class UtilExport AlgorithmException : public ExceptionBase
   {
   public:
      /// <summary>Create an AlgorithmException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      AlgorithmException(wstring  src, wstring  msg);
   };
   
   /// <summary>Occurs when an error should be displayed to the user verbatim</summary>
   class UtilExport ApplicationException : public ExceptionBase
   {
   public:
      /// <summary>Create an ApplicationException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      ApplicationException(wstring  src, wstring  msg);

      /// <summary>Convert existing exception into an ApplicationException</summary>
      /// <param name="r">Exception</param>
      ApplicationException(const ExceptionBase& r) ;
   };
   
   /// <summary>Occurs when a missing argument is detected</summary>
   class UtilExport ArgumentNullException : public ExceptionBase
   {
   public:
      /// <summary>Create an ArgumentNullException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      ArgumentNullException(wstring  src, const WCHAR* arg);
   };

   /// <summary>Occurs when an argument is invalid</summary>
   class UtilExport ArgumentException : public ExceptionBase
   {
   public:
      /// <summary>Create an ArgumentException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="arg">Name of argument</param>
      /// <param name="msg">Message</param>
      ArgumentException(wstring  src, wstring  arg, wstring  msg);
   };
   
   /// <summary>Occurs when a COM object is used incorrectly</summary>
   class UtilExport ComException : public ExceptionBase
   {
   public:
      /// <summary>Create a ComException from a _com_error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, _com_error&  err);

      /// <summary>Create a ComException from a _com_error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="err">COM error</param>
      ComException(wstring  src, HRESULT hr);
   };

   /// <summary>Occurs when a file is not found</summary>
   class UtilExport DirectoryNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a DirectoryNotFoundException from a folder path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">Folder path</param>
      DirectoryNotFoundException(wstring  src, Path  folder);
   };


   /// <summary>Occurs when a game data file is not in the expected format</summary>
   class UtilExport FileFormatException : public ExceptionBase
   {
   public:
      /// <summary>Create an FileFormatException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      FileFormatException(wstring  src, wstring  msg);

      /// <summary>Create an FileFormatException for a specific line</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="line">One based line number</param>
      /// <param name="msg">Message</param>
      FileFormatException(wstring  src, UINT line, wstring  msg);
   };

   /// <summary>Occurs when a file is not found</summary>
   class UtilExport FileNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a FileNotFoundException for a file path</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="file">File path</param>
      FileNotFoundException(wstring  src, Path  file);
   };
   
   /// <summary>Occurs when zlib encounters an error</summary>
   class UtilExport GZipException : public ExceptionBase
   {
   public:
      /// <summary>Create a GZipException from a custom message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Custom message</param>
      GZipException(wstring  src, const wstring&  msg);

      /// <summary>Create a GZipException from a zlib message</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">ZLib message</param>
      GZipException(wstring  src, const char*  msg);
   };
   
   /// <summary>Occurs when an index is out of range</summary>
   class UtilExport IndexOutOfRangeException : public ExceptionBase
   {
   public:
      /// <summary>Create an IndexOutOfRangeException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="index">Desired index</param>
      /// <param name="count">Item count</param>
      IndexOutOfRangeException(wstring  src, UINT index, UINT count);

      /// <summary>Create an IndexOutOfRangeException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="index">Desired index</param>
      IndexOutOfRangeException(wstring  src, UINT index);
   };

   /// <summary>Occurs when an invalid operation is attempted</summary>
   class UtilExport InvalidOperationException : public ExceptionBase
   {
   public:
      /// <summary>Create an InvalidOperationException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      InvalidOperationException(wstring  src, wstring  msg);
   };

   /// <summary>Occurs when an invalid value is detected within game data</summary>
   class UtilExport InvalidValueException : public ExceptionBase
   {
   public:
      /// <summary>Create an InvalidValueException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      InvalidValueException(wstring  src, wstring  msg);
   };

   /// <summary>Occurs when an I/O operator fails</summary>
   class UtilExport IOException : public ExceptionBase
   {
   public:
      /// <summary>Create an IOException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      IOException(wstring  src, wstring  msg);
   };

   /// <summary>Occurs when an operation that has not been implemented is attempted</summary>
   class UtilExport NotImplementedException : public ExceptionBase
   {
   public:
      /// <summary>Create a NotImplementedException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      NotImplementedException(wstring  src, wstring  msg);
   };

   /// <summary>Occurs when an unsupported operation is attempted</summary>
   class UtilExport NotSupportedException : public ExceptionBase
   {
   public:
      /// <summary>Create an NotSupportedException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      NotSupportedException(wstring  src, wstring  msg) ;
   };


   /// <summary>Occurs when a win32 API call fails</summary>
   class UtilExport Win32Exception : public ExceptionBase
   {
   public:
      /// <summary>Create an Win32Exception from the last system error</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">message</param>
      Win32Exception(wstring  src, wstring  msg);
   };

   
   // ------------------------ USER TYPES --------------------------


   /// <summary>General exception for any circumstance</summary>
   class UtilExport GenericException : public ExceptionBase
   {
   public:
      /// <summary>Create a GenericException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="msg">Message</param>
      GenericException(wstring  src, wstring msg);
   };


   /// <summary>Occurs when a language string is missing</summary>
   class UtilExport PageNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a PageNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      PageNotFoundException(wstring  src, UINT page);
   };

   /// <summary>Occurs when a language string is missing</summary>
   class UtilExport StringNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create a StringNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      /// <param name="id">string id</param>
      StringNotFoundException(wstring  src, UINT page, UINT id);
   };

   /// <summary>Occurs when a command syntax is missing</summary>
   class UtilExport SyntaxNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create an SyntaxNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="id">command id</param>
      /// <param name="ver">game version</param>
      SyntaxNotFoundException(wstring  src, UINT id, GameVersion v);
   };

   
}