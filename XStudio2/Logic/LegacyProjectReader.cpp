#include "stdafx.h"
#include "LegacyProjectReader.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates source value read from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      LegacyProjectReader::LegacyProjectReader(StreamPtr in) : XmlReader(in)
      {
      }


      LegacyProjectReader::~LegacyProjectReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the entire project file</summary>
      /// <param name="path">Full file path</param>
      /// <returns>New project file</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      ProjectFile  LegacyProjectReader::ReadFile(Path path)
      {
         try
         {
            ProjectFile file;

            // Read file
            LoadDocument();

            // Create folders
            file.Items.Add(new ProjectFolderItem(L"Language Files", true));
            file.Items.Add(new ProjectFolderItem(L"MD Scripts", true));
            file.Items.Add(new ProjectFolderItem(L"MSCI Scripts", true));
            file.Items.Add(new ProjectFolderItem(L"Variables", true));

            // Read documents/variables
            for (int i = 0; i < Root->childNodes->length; i++)
            {
               XmlNodePtr n = GetChild(Root, i, L"project item");

               // Comment: Skip
               if (n->nodeType != Xml::NODE_ELEMENT)
                  continue;

               // Document
               else if (n->nodeName == _bstr_t(L"document"))
               {
                  auto doc = ReadDocument(n);
                  // Script:
                  if (doc->FileType == FileType::Script)
                     file.Items[2]->Add(doc);
                  
                  // LanguageFile:
                  else if (doc->FileType == FileType::Language)
                     file.Items[0]->Add(doc);
               }
               // Variable
               else if (n->nodeName == _bstr_t(L"variable"))
                  file.Items[3]->Add(ReadVariable(n));
               else  
                  // Unrecognised
                  throw FileFormatException(HERE, L"Unrecognised element");;
            }

            // Return file
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      ProjectFileItem*  LegacyProjectReader::ReadDocument(XmlNodePtr node)
      {
         // Verify node
         ReadElement(node, L"document");

         // Verify path exists
         if (!node->text.length())
            throw FileFormatException(HERE, L"Missing document path");

         // Script:
         if (ReadAttribute(node, L"type") == L"script")
            return new ProjectFileItem((const wchar*)node->text, FileType::Script);

         else if (ReadAttribute(node, L"type") == L"language")
            return new ProjectFileItem((const wchar*)node->text, FileType::Language);

         // Unrecognised
         throw FileFormatException(HERE, L"Unrecognised file type");
      }

      ProjectVariableItem*  LegacyProjectReader::ReadVariable(XmlNodePtr node)
      {
         // Verify node
         ReadElement(node, L"variable");

         // Verify name exists
         if (!node->text.length())
            throw FileFormatException(HERE, L"Missing variable name");

         // Read value + name
         int val =  _ttoi(ReadAttribute(node, L"value").c_str());
         return new ProjectVariableItem((const wchar*)node->text, val);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Reads the source file and locates the CodeArray node</summary>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">File format is corrupt</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  LegacyProjectReader::LoadDocument() 
      {
         // Parse document
         XmlReader::LoadDocument();

         // Get project node
         Root = Document->documentElement;
         ReadElement(Root, L"project");

         // Verify version
         if (ReadAttribute(Root, L"version") != L"1")
            throw FileFormatException(HERE, L"Invalid legacy project file version");
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
