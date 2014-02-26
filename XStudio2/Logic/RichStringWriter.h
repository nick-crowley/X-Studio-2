#pragma once
#include "Common.h"
#include "LanguagePage.h"
#include "RichStringParser.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class RichStringWriter
      {
         // ------------------------ TYPES --------------------------
      protected:
         typedef list<TagType>  TagList;

         class TagStack : public list<TagType>
         {
         public:
            TagStack() {}

            void Push(TagType t)
            {
               push_front(t);
            }

            TagType Pop()
            {
               auto t = front();
               pop_front();
               return t;
            }

            /*TagStack& operator+=(TagType t)
            {
               Push(t);
               return *this;
            }*/
         };

         class CharState
         {
         public:
            CharState() {}

            bool operator==(const CharState& r)
            {
               return Bold==r.Bold && Italic==r.Italic && Underline==r.Underline && Colour==r.Colour && Alignment==r.Alignment;
            }
            bool operator!=(const CharState& r)
            {
               return !operator==(r);
            }

         public:
            bool       Bold, 
                       Italic, 
                       Underline;
            Colour     Colour;
            Alignment  Alignment;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         RichStringWriter(TextDocumentPtr& doc, ColourTag tags);
         virtual ~RichStringWriter();

         DEFAULT_COPY(RichStringWriter);	// Default copy semantics
         DEFAULT_MOVE(RichStringWriter);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         wstring  Write();

      protected:
         void   OnFormattingLost(CharState prev, CharState chr);
         void   OnFormattingGained(CharState prev, CharState chr);
         void   OnParagraphOpened(CharState s);
         void   OnParagraphClosed(CharState s);

         Colour GetColour(TextFontPtr& f);
         void   WriteChar(wchar ch);
         void   WriteChar(TextRangePtr chr, TextRangePtr prev);
         void   WriteState(TextRangePtr chr, bool open);

         // -------------------- REPRESENTATION ---------------------

      protected:
         TextDocumentPtr Input;
         wstring         Output;
         ColourTag       ColourTags;
         TagStack        Stack;
      };

   }
}

using namespace Logic::IO;
