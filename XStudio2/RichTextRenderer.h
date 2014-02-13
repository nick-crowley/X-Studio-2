
#pragma once
#include "Logic/RichString.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class RichTextRenderer
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary>Block of text with contiguous formatting</summary>   
         class RichPhrase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            RichPhrase(RichCharacter ch) : Format(ch.Format), Colour(ch.Colour)
            {
               Text.push_back(ch.Char);
            }
            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			
         public:
            CFont* GetFont(const LOGFONT& from) const
            {
               CFont* f = new CFont();
               LOGFONT lf = from;

               // Convert formatting into properties
               lf.lfItalic = (Format & CFE_ITALIC ? TRUE : FALSE);
               lf.lfWeight = (Format & CFE_BOLD ? FW_BOLD : FW_NORMAL);
               lf.lfUnderline = (Format & CFE_UNDERLINE ? TRUE : FALSE);

               // Create font
               f->CreateFontIndirectW(&lf);
               return f;
            }

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

            wstring Text;
            UINT    Format;
            Colour  Colour;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         RichTextRenderer();
         virtual ~RichTextRenderer();

         DEFAULT_COPY(RichTextRenderer);	// Default copy semantics
         DEFAULT_MOVE(RichTextRenderer);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static void  DrawLine(CDC* dc, CRect line, const RichString& str);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace GUI::Utils;
