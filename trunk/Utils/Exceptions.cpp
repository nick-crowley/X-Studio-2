#pragma once
#include "stdafx.h"
#include "Exceptions.h"

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------
   
   /// <summary>Create a exception without an error code but with a message</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Error message</param>
   ExceptionBase::ExceptionBase(wstring  src, wstring msg) : ErrorID(0), Message(msg), Source(src)
   {}

   /// <summary>Create a exception with an error code and message</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="err">Error id</param>
   /// <param name="msg">Error message</param>
   ExceptionBase::ExceptionBase(wstring  src, UINT  err, wstring msg) : ErrorID(err), Message(msg), Source(src)
   {}

   
   ExceptionBase::~ExceptionBase()
   {}
   
   /// <summary>Create an AlgorithmException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   AlgorithmException::AlgorithmException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"Invariant violated: ") + msg)
   {}
   
   /// <summary>Create an ApplicationException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   ApplicationException::ApplicationException(wstring  src, wstring  msg) 
      : ExceptionBase(src, msg)
   {}

   /// <summary>Convert existing exception into an ApplicationException</summary>
   /// <param name="r">Exception</param>
   ApplicationException::ApplicationException(const ExceptionBase& r) 
      : ExceptionBase(r.Source, r.Message)
   {}
   
   /// <summary>Create an ArgumentNullException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="arg">Name of argument</param>
   ArgumentNullException::ArgumentNullException(wstring  src, const WCHAR* arg) 
      // "Missing '%s' argument"
      : ExceptionBase(src, VString(ERR_ARGUMENT_NULL, arg))
   {}

   /// <summary>Create an ArgumentException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="arg">Name of argument</param>
   /// <param name="msg">Message</param>
   ArgumentException::ArgumentException(wstring  src, wstring  arg, wstring  msg) 
      // "The '%s' argument is invalid : %s"
      : ExceptionBase(src, VString(L"The '%s' argument is invalid : %s", arg.c_str(), msg.c_str()) )
   {}
   
   /// <summary>Create a ComException from a _com_error</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="err">COM error</param>
   ComException::ComException(wstring  src, _com_error&  err) 
      // "COM object error: ...."
      : ExceptionBase(src, err.Error(), wstring(L"COM operation failed: ") + err.ErrorMessage())
   {}

   /// <summary>Create a ComException from a result code</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="err">COM error</param>
   ComException::ComException(wstring  src, HRESULT hr) 
      // "COM object error: ...."
      : ExceptionBase(src, hr, wstring(L"COM operation failed: ") + SysErrorString(hr))
   {}

   /// <summary>Create a ComException from a message and result code</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   /// <param name="err">COM error</param>
   ComException::ComException(wstring  src, wstring msg, HRESULT hr)
      : ExceptionBase(src, hr, msg + L"COM operation failed: " + SysErrorString(hr))
   {}

   /// <summary>Create a DirectoryNotFoundException from a folder path</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="file">Folder path</param>
   DirectoryNotFoundException::DirectoryNotFoundException(wstring  src, Path  folder) 
      // "The folder '%s' does not exist"
      : ExceptionBase(src, ERROR_PATH_NOT_FOUND, VString(L"The folder '%s' does not exist", folder.c_str()) )
   {}


   /// <summary>Create an FileFormatException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   FileFormatException::FileFormatException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"The file format is invalid: ") + msg)
   {}

   /// <summary>Create an FileFormatException for a specific line</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="line">One based line number</param>
   /// <param name="msg">Message</param>
   FileFormatException::FileFormatException(wstring  src, UINT line, wstring  msg) 
      : ExceptionBase(src, VString(L"The file format is invalid on line %u: ", line) + msg)
   {}

   /// <summary>Create a FileNotFoundException for a file path</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="file">File path</param>
   FileNotFoundException::FileNotFoundException(wstring  src, Path  file) 
      : ExceptionBase(src, ERROR_FILE_NOT_FOUND, VString(L"The file '%s' does not exist", file.c_str()))
   {}
   
   /// <summary>Create a GZipException from a custom message</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Custom message</param>
   GZipException::GZipException(wstring  src, const wstring&  msg) 
      : ExceptionBase(src, L"Error in gzip handling: " + msg)
   {}

   /// <summary>Create a GZipException from a zlib message</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">ZLib message</param>
   GZipException::GZipException(wstring  src, const char*  msg) 
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
   
   /// <summary>Create an IndexOutOfRangeException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="index">Desired index</param>
   /// <param name="count">Item count</param>
   IndexOutOfRangeException::IndexOutOfRangeException(wstring  src, UINT index, UINT count) 
      : ExceptionBase(src, VString(L"The index %d is out of range, only %d objects in collection", index, count))
   {}

   /// <summary>Create an IndexOutOfRangeException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="index">Desired index</param>
   IndexOutOfRangeException::IndexOutOfRangeException(wstring  src, UINT index) 
      : ExceptionBase(src, VString(L"The index %d is out of range", index))
   {}

   /// <summary>Create an InvalidOperationException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   InvalidOperationException::InvalidOperationException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"The operation is invalid: ") + msg)
   {}

   /// <summary>Create an InvalidValueException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   InvalidValueException::InvalidValueException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"Invalid game data value: ") + msg)
   {}

   /// <summary>Create an IOException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   IOException::IOException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"The I/O operation failed: ") + msg)
   {}

   /// <summary>Create a NotImplementedException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">message</param>
   NotImplementedException::NotImplementedException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"The operation has not been implemented: ") + msg)
   {}

   /// <summary>Create an NotSupportedException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">message</param>
   NotSupportedException::NotSupportedException(wstring  src, wstring  msg) 
      : ExceptionBase(src, wstring(L"The operation is invalid: ") + msg)
   {}


   /// <summary>Create an Win32Exception from the last system error</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">message</param>
   Win32Exception::Win32Exception(wstring  src, wstring  msg) 
      : ExceptionBase(src, msg + L" : " + (GetLastError() ? (wstring)SysErrorString() : L"<Subsystem provided no further details>"))
   {}

   
   // ------------------------ USER TYPES --------------------------


   /// <summary>Create a GenericException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="msg">Message</param>
   GenericException::GenericException(wstring  src, wstring msg) 
      : ExceptionBase(src, msg)
   {}


   /// <summary>Create a PageNotFoundException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="page">page id</param>
   PageNotFoundException::PageNotFoundException(wstring  src, UINT page) 
      : ExceptionBase(src, VString(L"Cannot find page %d", page))
   {}

   /// <summary>Create a StringNotFoundException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="page">page id</param>
   /// <param name="id">string id</param>
   StringNotFoundException::StringNotFoundException(wstring  src, UINT page, UINT id) 
      : ExceptionBase(src, VString(L"Cannot find string %d in page %d", id, page))
   {}

   /// <summary>Create an SyntaxNotFoundException</summary>
   /// <param name="src">Location of throw</param>
   /// <param name="id">command id</param>
   /// <param name="ver">game version</param>
   SyntaxNotFoundException::SyntaxNotFoundException(wstring  src, UINT id, GameVersion v) 
      : ExceptionBase(src, VString(L"Cannot find %s syntax for command with id %d", VersionString(v).c_str(), id))
   {}

   
   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------  
}