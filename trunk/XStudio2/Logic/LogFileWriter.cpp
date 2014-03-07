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
            RGB(0,0,0),
            RGB(255,0,0),
            RGB(0,255,0),
            RGB(0,0,255),
            RGB(255,0,255),
            RGB(255,255,0),
            RGB(0,255,255),
            RGB(255,255,255),
         };

         // Init writer
         RtfWriter::Open(L"FixedSys", 10, colours);

         // Set colours
         RtfWriter::SetForeColour(RGB(255,255,255));
         RtfWriter::SetBackColour(RGB(0,0,0));

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

      /// <summary>Writes the specified text.</summary>
      /// <param name="txt">The text.</param>
      /// <param name="attr">Console attributes.</param>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void  LogFileWriter::Write(const wstring& txt, WORD attr)
      {
         // Set colour
         switch (attr & (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE))
         {
         case FOREGROUND_RED:                                   SetForeColour(RGB(255,0,0));      break;
         case FOREGROUND_GREEN:                                 SetForeColour(RGB(0,255,0));      break;
         case FOREGROUND_BLUE:                                  SetForeColour(RGB(0,0,255));      break;
         case FOREGROUND_RED|FOREGROUND_GREEN:                  SetForeColour(RGB(255,255,0));    break;
         case FOREGROUND_GREEN|FOREGROUND_BLUE:                 SetForeColour(RGB(0,255,255));    break;
         case FOREGROUND_RED|FOREGROUND_BLUE:                   SetForeColour(RGB(255,0,255));    break;
         case FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE:  SetForeColour(RGB(255,255,255));  break;
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
