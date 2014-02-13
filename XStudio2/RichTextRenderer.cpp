#include "stdafx.h"
#include "RichTextRenderer.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      RichTextRenderer::RichTextRenderer()
      {
      }


      RichTextRenderer::~RichTextRenderer()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Draw rich text in a single line</summary>
      /// <param name="dc">dc.</param>
      /// <param name="line">line rect</param>
      /// <param name="str">string.</param>
      void  RichTextRenderer::DrawLine(CDC* dc, CRect line, const RichString& str)
      {
         dc->DrawText(L"quick brown fox", line, DT_LEFT);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
