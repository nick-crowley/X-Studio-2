#include "stdafx.h"
#include "TemplateFileReader.h"

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
      TemplateFileReader::TemplateFileReader(StreamPtr in) : XmlReader(in)
      {
      }


      TemplateFileReader::~TemplateFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the entire template file</summary>
      /// <returns>List of templates</returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Missing elements/attributes or invalid template file-type</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      TemplateList TemplateFileReader::ReadFile()
      {
         try
         {
            TemplateList templates;

            // Parse document
            LoadDocument();

            // Get root as node
            XmlNodePtr root(Document->documentElement);

            // Read templates
            for (int i = 0; i < root->childNodes->length; i++)
            {
               XmlNodePtr n = root->childNodes->item[i];

               // Skip comments, read elements
               if (n->nodeType == Xml::NODE_ELEMENT)
                  templates.push_back( ReadTemplate(n) );
            }

            return templates;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Reads a template.</summary>
      /// <param name="node">The node.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Missing element/attribute -or- unrecognised file-type</exception>
      TemplateFile  TemplateFileReader::ReadTemplate(XmlNodePtr node)
      {
         try
         {
            // Verify node
            ReadElement(node, L"template");

            // Attributes
            auto name = ReadAttribute(node, L"name");
            auto type = ParseFileType(ReadAttribute(node, L"type"));
            wstring desc = GetChild(node, 0, L"description")->text,
                    path = GetChild(node, 1, L"path")->text;

            // Generate template
            return TemplateFile(name, type, desc, path);
         }
         // Unrecognised file type
         catch (ArgumentException& e) {
            throw FileFormatException(HERE, e.Message);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}