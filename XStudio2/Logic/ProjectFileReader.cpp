#include "stdafx.h"
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
      ProjectFileReader::ProjectFileReader(StreamPtr in) : XmlReader(in)
      {
      }


      ProjectFileReader::~ProjectFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Get file-type from name</summary>
      /// <exception cref="Logic::ArgumentException">Unrecognised file type</exception>
      FileType  ParseFileType(const wstring& type)
      {
         // Case insensitive comparison
         if (StrCmpI(type.c_str(), L"Unknown") == 0)
	         return FileType::Unknown;

         else if (StrCmpI(type.c_str(), L"Universe") == 0)
	         return FileType::Universe;

         else if (StrCmpI(type.c_str(), L"Project") == 0)
	         return FileType::Project;

         else if (StrCmpI(type.c_str(), L"Mission") == 0)
	         return FileType::Mission;

         else if (StrCmpI(type.c_str(), L"Language") == 0)
	         return FileType::Language;

         else if (StrCmpI(type.c_str(), L"Script") == 0)
	         return FileType::Script;
         
         // Unrecognised:
         throw ArgumentException(HERE, L"t", GuiString(L"Unrecognised fileType '%s'", type.c_str()));
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the entire project file</summary>
      /// <param name="path">Full file path</param>
      /// <returns>New project file</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Unrecognised file type -or- Missing elements/attributes</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      ProjectFile  ProjectFileReader::ReadFile(Path path)
      {
         try
         {
            ProjectFile file;

            // Read file
            LoadDocument();

            // Read items
            for (int i = 0, count = Root->childNodes->length; i < count; i++)
            {
               auto n = GetChild(Root, i, L"project item");

               // Comment: Skip
               if (n->nodeType != Xml::NODE_ELEMENT)
                  continue;

               // Folder
               else if (n->nodeName == _bstr_t(L"folder"))
                  file.Items.Add(ReadFolderItem(n));

               // File
               else if (n->nodeName == _bstr_t(L"file"))
                  file.Items.Add(ReadFileItem(n));

               // Variable
               else if (n->nodeName == _bstr_t(L"variable"))
                  file.Items.Add(ReadVariableItem(n));

               // Unrecognised
               else  
                  throw FileFormatException(HERE, L"Unrecognised project item");;
            }

            // Return file
            return file;
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
      void  ProjectFileReader::LoadDocument() 
      {
         // Parse document
         XmlReader::LoadDocument();

         // Get project node
         Root = Document->documentElement;
         ReadElement(Root, L"project");

         // Verify version
         if (ReadAttribute(Root, L"version") != L"2")
            throw FileFormatException(HERE, L"Invalid project file version");
      }
      
      /// <summary>Reads a single file node</summary>
      /// <param name="node">node</param>
      /// <returns>new file item</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Unrecognised file type -or- Missing elements/attributes</exception>
      ProjectFileItem*  ProjectFileReader::ReadFileItem(XmlNodePtr node)
      {
         try
         {
            FileType type;

            // Verify node
            ReadElement(node, L"file");

            // Verify path exists
            if (!node->text.length())
               throw FileFormatException(HERE, L"Missing document path");

            try
            {
               // Resolve type
               type = ParseFileType(ReadAttribute(node, L"type"));

               // Verify file-type
               if (type == FileType::Unknown)
                  throw FileFormatException(HERE, L"'Unknown' file-type is not supported");
            }
            catch (ArgumentException& e) {
               throw FileFormatException(HERE, e.Message);
            }

            // Create file
            return new ProjectFileItem((const wchar*)node->text, type);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      
      /// <summary>Reads a folder node and its children</summary>
      /// <param name="node">node</param>
      /// <returns>new folder item</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Unrecognised file type -or- Missing elements/attributes</exception>
      ProjectFolderItem*  ProjectFileReader::ReadFolderItem(XmlNodePtr node)
      {
         // Verify node
         ReadElement(node, L"folder");

         // Read folder
         auto* folder = new ProjectFolderItem(ReadAttribute(node, L"name"), ReadAttribute(node, L"fixed") == L"true");

         // Read items
         for (int i = 0, count = node->childNodes->length; i < count; i++)
         {
            auto n = GetChild(node, i, L"project item");

            // Comment: Skip
            if (n->nodeType != Xml::NODE_ELEMENT)
               continue;

            // Folder
            else if (n->nodeName == _bstr_t(L"folder"))
               folder->Add( ReadFolderItem(n) );

            // File
            else if (n->nodeName == _bstr_t(L"file"))
               folder->Add( ReadFileItem(n) );

            // Variable
            else if (n->nodeName == _bstr_t(L"variable"))
               folder->Add( ReadVariableItem(n) );

            // Unrecognised
            else  
               throw FileFormatException(HERE, L"Unrecognised project item");;
         }

         return folder;
      }

      /// <summary>Reads a single variable node</summary>
      /// <param name="node">node</param>
      /// <returns>new variable item</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Missing name -or- Missing elements/attributes</exception>
      ProjectVariableItem*  ProjectFileReader::ReadVariableItem(XmlNodePtr node)
      {
         try
         {
            // Verify node
            ReadElement(node, L"variable");

            // Verify name exists
            if (!node->text.length())
               throw FileFormatException(HERE, L"Missing variable name");

            // Read value + name
            int val = GuiString(ReadAttribute(node, L"value")).ToInt();
            return new ProjectVariableItem((const wchar*)node->text, val);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
