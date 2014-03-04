#include "stdafx.h"
#include "LanguageFileWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a syntax writer for an output stream</summary>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      LanguageFileWriter::LanguageFileWriter(StreamPtr out) : XmlWriter(out)
      {
      }


      LanguageFileWriter::~LanguageFileWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes and flushes the output stream</summary>
      void  LanguageFileWriter::Close()
      {
         __super::Close();
      }

      /// <summary>Writes a language file</summary>
      /// <param name="f">The file</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  LanguageFileWriter::Write(const LanguageFile& f)
      {
         // Header
         WriteInstruction(L"version='1.0' encoding='UTF-8'");
         WriteComment(L"Written by X-Studio II");

         // Root: LanguageID
         auto root = WriteRoot(L"language");
         WriteAttribute(root, L"id", (int)f.Language);

         // Pages/Strings
         for (const LanguagePage& page : f)
         {
            // Page
            auto pageNode = WriteElement(root, L"page");
            WriteAttribute(pageNode, L"id", page.ID);
            WriteAttribute(pageNode, L"title", page.Title);
            WriteAttribute(pageNode, L"descr", page.Description);
            WriteAttribute(pageNode, L"voice", page.Voiced ? L"yes" : L"no");

            // Strings
            for (const LanguageString& str : page)
            {
               auto strNode = WriteElement(pageNode, L"t");
               WriteAttribute(strNode, L"id", str.ID);
               WriteText(strNode, str.Text);
            }
         }

      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
