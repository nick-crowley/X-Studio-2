#include "stdafx.h"
#include "RtfStringWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      RtfStringWriter::RtfStringWriter(string& out) : RtfWriter(out)
      {
         // Define colours
         list<COLORREF> col;
         col.push_back(Black);
         col.push_back(White);
         col.push_back(Red);
         col.push_back(Green);
         col.push_back(Blue);
         col.push_back(Yellow);
         col.push_back(Purple);
         col.push_back(Cyan);
         col.push_back(Grey);

         // Init writer
         RtfWriter::Open(L"Arial", 10, col);

         // Set properties
         RtfWriter::SetForeColour(White);
         RtfWriter::SetBackColour(Black);
      }

      RtfStringWriter::~RtfStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      void  RtfStringWriter::Write(const RichString& str)
      {
         RtfWriter::SetAlignment(Alignment::Left);
         RtfWriter::SetLeftMargin(500);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
