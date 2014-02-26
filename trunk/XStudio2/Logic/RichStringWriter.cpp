#include "stdafx.h"
#include "RichStringWriter.h"
#include "RtfStringWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a rich text source code writer for a TOM text document</summary>
      /// <param name="doc">The document.</param>
      /// <param name="tags">Type of colour tags to use</param>
      /// <exception cref="Logic::ArgumentNullException">Document is null</exception>
      RichStringWriter::RichStringWriter(TextDocumentPtr& doc, ColourTag tags) : Input(doc), ColourTags(tags)
      {
         REQUIRED(doc);
      }


      RichStringWriter::~RichStringWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get tag name from enum</summary>
      /// <param name="t">tag type.</param>
      /// <returns></returns>
      wstring  RichStringWriter::GetTagString(TagType t)
      {
         const wchar* str[] = { L"b", L"u", L"i", L"left", L"right", L"center", L"justify", L"text", L"select", L"author", 
                                L"title", L"rank",L"black", L"blue", L"cyan", L"green", L"magenta", L"orange", L"red", L"silver", L"yellow", 
                                L"white", L"unrecognised"  };

         return str[(UINT)t];
      }

      TagType  RichStringWriter::GetColourTag(COLORREF c)
      {
         switch (RtfStringWriter::FromRGB(c))
         {
         case Colour::Black:   return TagType::Black;
         case Colour::Default: return TagType::Default;
         case Colour::White:   return TagType::White;  
         case Colour::Silver:  return TagType::Silver;

         case Colour::Grey:    return TagType::Grey;
         case Colour::Blue:    return TagType::Blue;
         case Colour::Cyan:    return TagType::Cyan;
         case Colour::Green:   return TagType::Green;
         case Colour::Orange:  return TagType::Orange;
         case Colour::Purple:  return TagType::Magenta;
         case Colour::Red:     return TagType::Red;
         case Colour::Yellow:  return TagType::Yellow;
         }
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Generates richText source code for the input document</summary>
      /// <returns></returns>
      wstring  RichStringWriter::Write()
      {
         try
         {
            if (!InputLength)
            {
               TextRangePtr thisChar = Input->Range(0, 1),
                            prevChar(thisChar->Duplicate);
               CharState    thisState(thisChar->Font, thisChar->Para),
                            prevState(thisState);

               // Open paragraph
               OnParagraphOpened(thisState);
               WriteChar(thisChar);

               // Examine each character
               for (int i = 1, end = InputLength; i < end; ++i)
               {
                  // Advance this
                  thisChar->Move(TOM::tomCharacter, 1);
                  thisState = CharState(thisChar->Font, thisChar->Para);

                  // Paragraph change: Close open tags, Open new paragraph
                  if (thisState.Alignment != prevState.Alignment)
                  {
                     OnParagraphClosed(prevState.Alignment);
                     OnParagraphOpened(thisState);
                  }
                  else
                  {
                     // Formatting lost: Close tags
                     if (thisState < prevState)
                        OnFormattingLost(thisState - prevState);
                     
                     // Formatting gained: Open tags
                     if (thisState > prevState)
                        OnFormattingGained(prevState - thisState);
                  }
                  
                  // Write character
                  WriteChar(thisChar);

                  // Re-sync prev 
                  prevChar->Move(TOM::tomCharacter, 1);
                  prevState = thisState;
               }

               // Close paragraph
               OnParagraphClosed(thisState.Alignment);
            }

            return Output;
         }
         catch (_com_error& e) {
            throw ComException(HERE, e);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      
      void  RichStringWriter::OnFormattingLost(TagList& lost)
      {
         TagList unchanged;
         
         // Close all open tags until all lost formatting has been closed
         while (!lost.empty())
         {
            // Close tag
            auto tag = Formatting.Pop();
            WriteTag(tag, false);

            // Remove/preserve tag
            if (tag == lost.front())
               lost.pop_front();
            else
               unchanged.push_front(tag);
         }

         // Re-open tags whoose formatting was unchanged
         for (TagType t : unchanged)
            WriteTag(t, true);
      }

      void  RichStringWriter::OnFormattingGained(const TagList& gained)
      {
         // Open tags
         for (TagType t : gained)
         {
            WriteTag(t, true);
            Formatting.Push(t);
         }
      }

      void  RichStringWriter::OnParagraphOpened(CharState s)
      {
         // Open paragraph
         WriteTag(s.Alignment, true);

         // Open tags (if any)
         for (TagType t : s.FormatTags)
         {
            WriteTag(t, true);
            Formatting.Push(t);
         }
      }

      void  RichStringWriter::OnParagraphClosed(TagType para)
      {
         // Close all tags
         while (!Formatting.empty())
            WriteTag(Formatting.Pop(), false);
         
         // Close paragraph
         WriteTag(para, false);

         // Clear stack
         Formatting.clear();
      }

      /// <summary>Writes the character.</summary>
      /// <param name="chr">range</param>
      void  RichStringWriter::WriteChar(TextRangePtr chr)
      {
         switch (wchar ch = (wchar)chr->Char)
         {
         case '\r':  break;
         case '\v':  Output += '\n';  break;
         default:    Output += ch;    break;
         }
      }

      /// <summary>Writes the tag.</summary>
      /// <param name="t">The t.</param>
      /// <param name="open">The open.</param>
      void  RichStringWriter::WriteTag(TagType t, bool open)
      {
         Output += GuiString(open ? L"[%s]" : L"[/%s]", GetTagString(t).c_str());
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
