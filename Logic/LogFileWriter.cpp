#include "stdafx.h"
#include "LogFileWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create an Rtf script writer</summary>
      /// <param name="out">Output stream</param>
      /// <param name="font">Font name</param>
      /// <param name="size">Size in points</param>
      /// <exception cref="Logic::ArgumentNullException">Output is nullptr</exception>
      LogFileWriter::LogFileWriter(StreamPtr out) : RtfWriter(out)
      {
         // Define unique colours
         list<COLORREF> colours
         {
            White,
            Red,
            Green,
            Blue,
            Magenta,
            Yellow,
            Cyan,
            Black,
         };

         // Init writer
         RtfWriter::Open(L"FixedSys", 10, colours);

         // Set colours
         RtfWriter::SetBackColour(White);
         RtfWriter::SetForeColour(Black);

         // Set properties
         RtfWriter::SetAlignment(Alignment::Left);
      }

      /// <summary>Safely Closes the writer, if open</summary>
      LogFileWriter::~LogFileWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer and the stream.</summary>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void LogFileWriter::Close()
      {
         __super::Close();
      }

      /// <summary>Flushes the output stream</summary>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void LogFileWriter::Flush()
      {
         __super::Flush();
      }

      /// <summary>Writes the specified text.</summary>
      /// <param name="txt">The text.</param>
      /// <param name="attr">Console attributes.</param>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void  LogFileWriter::Write(const wstring& txt, WORD attr)
      {
         // Set colour
         switch (attr & FOREGROUND_WHITE)
         {
         case FOREGROUND_RED:    SetForeColour(Red);     break;
         case FOREGROUND_GREEN:  SetForeColour(Green);   break;
         case FOREGROUND_BLUE:   SetForeColour(Blue);    break;
         case FOREGROUND_YELLOW: SetForeColour(Yellow);  break;
         case FOREGROUND_CYAN:   SetForeColour(Cyan);    break;
         case FOREGROUND_PURPLE: SetForeColour(Magenta); break;
         case FOREGROUND_WHITE:  SetForeColour(Black);   break;
         }

         // Bold/Normal
         __super::SetBold((attr & FOREGROUND_INTENSITY) != 0);

         // Write
         __super::Write(txt);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
