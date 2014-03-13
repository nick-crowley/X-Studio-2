#include "stdafx.h"
#include "ProjectFileWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a syntax writer for an output stream</summary>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      ProjectFileWriter::ProjectFileWriter(StreamPtr out) : XmlWriter(out)
      {
      }


      ProjectFileWriter::~ProjectFileWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes and flushes the output stream</summary>
      void  ProjectFileWriter::Close()
      {
         __super::Close();
      }

      /// <summary>Writes a Project file</summary>
      /// <param name="f">The file</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  ProjectFileWriter::Write(const ProjectFile& f)
      {
         // Header
         WriteInstruction(L"version='1.0' encoding='UTF-8'");
         WriteComment(L"Written by X-Studio II");

         // Root: File format version
         auto root = WriteRoot(L"project");
         WriteAttribute(root, L"version", 2);

         // Write Items recursively
         for (auto& item : f.Items)
            WriteItem(item, root);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Writes an item and all it's children (if any)</summary>
      /// <param name="item">The item.</param>
      /// <param name="parent">The parent.</param>
      /// <exception cref="Logic::ArgumentNullException">Parent is nullptr or Item type corrupted</exception>
      void  ProjectFileWriter::WriteItem(const ProjectItemPtr& item, XmlElementPtr& parent)
      {
         REQUIRED(parent);

         XmlElementPtr folder;

         switch (item->Type)
         {
         // Folder: Only folders can have children
         case ProjectItemType::Folder:
            folder = WriteFolder(dynamic_cast<ProjectFolderItem*>(item.get()), parent);
            
            // Write Children
            for (auto& c : item->Children)
               WriteItem(c, folder);
            break;

         // File:
         case ProjectItemType::File:
            WriteFile(dynamic_cast<ProjectFileItem*>(item.get()), parent);
            break;

         // Variable:
         case ProjectItemType::Variable:
            WriteVariable(dynamic_cast<ProjectVariableItem*>(item.get()), parent);
            break;
         }
      }

      /// <summary>Writes file item.</summary>
      /// <param name="file">file.</param>
      /// <param name="parent">parent node.</param>
      /// <exception cref="Logic::ArgumentNullException">Missing parameter</exception>
      void  ProjectFileWriter::WriteFile(const ProjectFileItem* file, XmlElementPtr& parent)
      {
         REQUIRED(file);
         REQUIRED(parent);

         // <file type='..'>full_path</file>
         auto node = WriteElement(parent, L"file", file->FullPath.c_str());
         WriteAttribute(node, L"backup", file->BackupPath.c_str());
         WriteAttribute(node, L"type", ::GetString(file->FileType));
      }

      /// <summary>Writes folder item.</summary>
      /// <param name="folder">folder.</param>
      /// <param name="parent">parent node.</param>
      /// <returns>Folder node</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing parameter</exception>
      XmlElementPtr  ProjectFileWriter::WriteFolder(const ProjectFolderItem* folder, XmlElementPtr& parent)
      {
         REQUIRED(folder);
         REQUIRED(parent);

         // <folder name=.. fixed=..>  ... </folder>
         auto node = WriteElement(parent, L"folder");
         WriteAttribute(node, L"name", folder->Name);
         WriteAttribute(node, L"fixed", folder->Fixed ? L"true" : L"false");
         
         return node;
      }

      /// <summary>Writes variable item.</summary>
      /// <param name="var">variable.</param>
      /// <param name="parent">parent node.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentNullException">Missing parameter</exception>
      void  ProjectFileWriter::WriteVariable(const ProjectVariableItem* var, XmlElementPtr& parent)
      {
         REQUIRED(var);
         REQUIRED(parent);

         // <variable value='..'>name</variable>
         auto node = WriteElement(parent, L"variable", var->Name);
         WriteAttribute(node, L"value", var->Value);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
