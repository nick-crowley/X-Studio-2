#include "stdafx.h"
#include "FileTemplateReader.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an xml reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      FileTemplateReader::FileTemplateReader(StreamPtr in) : XmlReader(in)
      {
      }


      FileTemplateReader::~FileTemplateReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the entire language file</summary>
      /// <param name="path">Full path</param>
      /// <returns>New language file</returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid languageID or pageID</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      list<FileTemplate> FileTemplateReader::ReadFile()
      {
         try
         {
            list<FileTemplate> templates;

            // Parse document
            LoadDocument();

            // Get root (as node)
            XmlNodePtr languageNode(Document->documentElement);

            // Read fileID + language tag
            file.ID = LanguageFilenameReader(path.FileName).FileID;
            file.Language = ReadLanguageTag(languageNode);

            // Read templates
            for (int i = 0; i < languageNode->childNodes->length; i++)
            {
               XmlNodePtr n = languageNode->childNodes->item[i];

               if (n->nodeType == Xml::NODE_ELEMENT)
                  templates.push_back( ReadTemplate(n) );
            }

            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}