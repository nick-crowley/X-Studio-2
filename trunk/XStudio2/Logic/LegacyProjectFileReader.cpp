#include "stdafx.h"
#include "LegacyProjectFileReader.h"
#include "ProjectFileReader.h"

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
      LegacyProjectFileReader::LegacyProjectFileReader(StreamPtr in) : XmlReader(in)
      {
      }


      LegacyProjectFileReader::~LegacyProjectFileReader()
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
      ProjectFile  LegacyProjectFileReader::ReadFile(Path path)
      {
         try
         {
            ProjectFile file(path.FileName);

            // Read file
            LoadDocument();

            // Create base folders
            auto language = file.Add(ProjectItem(L"Language Files", true));
            auto scripts = file.Add(ProjectItem(L"MD Scripts", true));
            auto missions = file.Add(ProjectItem(L"MSCI Scripts", true));
            auto variables = file.Add(ProjectItem(L"Variables", true));

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
                  if (doc.FileType == FileType::Script)
                     scripts.Add(doc);
                  
                  // LanguageFile:
                  else if (doc.FileType == FileType::Language)
                     language.Add(doc);
               }
               // Variable
               else if (n->nodeName == _bstr_t(L"variable"))
                  variables.Add(ReadVariable(n));
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

      /// <summary>Reads a single file/document node</summary>
      /// <param name="node">node</param>
      /// <returns>new file item</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Unrecognised file type -or- Missing elements/attributes</exception>
      ProjectItem  LegacyProjectFileReader::ReadDocument(XmlNodePtr node)
      {
         try
         {
            // Verify node
            ReadElement(node, L"document");

            // Require path
            if (!node->text.length())
               throw FileFormatException(HERE, L"Missing document path");

            // Read type
            FileType type = ProjectFileReader::ParseFileType(ReadAttribute(node, L"type"));

            // Require Script/Language:
            if (type != FileType::Language && type != FileType::Script)
               throw FileFormatException(HERE, L"Unrecognised file type");

            // Create file (without backup)
            return ProjectItem(type, (const wchar*)node->text, L"");
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Reads a single variable node</summary>
      /// <param name="node">node</param>
      /// <returns>new variable item</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Missing name -or- Missing elements/attributes</exception>
      ProjectItem  LegacyProjectFileReader::ReadVariable(XmlNodePtr node)
      {
         try
         {
            // Verify node
            ReadElement(node, L"variable");

            // Verify name exists
            if (!node->text.length())
               throw FileFormatException(HERE, L"Missing variable name");

            // Read value + name
            int val =  GuiString(ReadAttribute(node, L"value")).ToInt();
            return ProjectItem((const wchar*)node->text, val);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Reads the source file and locates the root node</summary>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">File format is corrupt</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  LegacyProjectFileReader::LoadDocument() 
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
