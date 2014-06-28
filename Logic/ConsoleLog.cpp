#include "stdafx.h"
#include "ConsoleLog.h"
#include "FileStream.h"

namespace Logic
{
   
   /// <summary>Log file singleton</summary>
   ConsoleLog  ConsoleLog::Instance;

   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Do nothing</summary>
   ConsoleLog::ConsoleLog() : Writer(nullptr)
   {
   }


   /// <summary>SafeCloses the writer and stream</summary>
   ConsoleLog::~ConsoleLog()
   {
      if (Writer)
         delete Writer;
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Closes and flushes the writer and the stream.</summary>
   /// <exception cref="Logic::InvalidOperationException">Writer has already been closed</exception>
   /// <exception cref="Logic::IOException">I/O error occurred</exception>
   void  ConsoleLog::Close()
   {
      // Validate state
      if (!Writer)
         throw InvalidOperationException(HERE, L"Writer is already closed");

      // Close + destroy
      Writer->Close();
      delete Writer;
      Writer = nullptr;
   }

   /// <summary>Flushes the writer and the stream.</summary>
   /// <exception cref="Logic::InvalidOperationException">Writer has already been closed</exception>
   /// <exception cref="Logic::IOException">I/O error occurred</exception>
   void  ConsoleLog::Flush()
   {
      // Validate state
      if (!Writer)
         throw InvalidOperationException(HERE, L"Writer is closed");

      // Flush
      Writer->Flush();
   }

   /// <summary>Opens the writer and the stream.</summary>
   /// <exception cref="Logic::InvalidOperationException">Writer has already been opened</exception>
   /// <exception cref="Logic::IOException">I/O error occurred</exception>
   void  ConsoleLog::Open()
   {
      // Validate state
      if (Writer)
         throw InvalidOperationException(HERE, L"Writer is already open");

      // Create logfile using flags to reduce caching
      auto file = new FileStream(AppPath(L"Console.rtf"), 
                                 FileMode::CreateAlways, 
                                 FileAccess::ReadWrite, 
                                 FileAttribute::Normal, /* FileAttribute::WriteThrough */
                                 FileShare::AllowRead);

      // Create writer
      Writer = new LogFileWriter(StreamPtr(file));
   }

   /// <summary>Writes the specified text.</summary>
   /// <param name="txt">The text.</param>
   /// <param name="attr">Console attributes.</param>
   /// <exception cref="Logic::IOException">I/O error occurred</exception>
   void  ConsoleLog::Write(const wstring& txt, WORD attr)
   {
      // Validate state
      if (!Writer)
         throw InvalidOperationException(HERE, L"Writer is closed");

      // Write
      Writer->Write(txt, attr);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------
}

