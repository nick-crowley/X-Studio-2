#pragma once

namespace GUI
{
   namespace Utils
   {
      
      /// <summary></summary>
      class FileTemplate
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         FileTemplate(wstring name, FileType type, LPCWSTR desc, LPCWSTR subpath) 
            : Name(name), Type(type), Description(desc), SubPath(subpath)
         {}

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,Extension,GetExtension);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get the default file extension.</summary>
         /// <returns></returns>
         wstring  GetExtension() const
         {
            return Type != FileType::Project ? L".xml" : L".xprj";
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         const wstring   Name;            // Template name
         const wstring   Description,     // Template description
                         SubPath;         // File SubPath: 'Templates\...'
         const FileType  Type;            // Document type
      };

      /// <summary></summary>
      typedef list<FileTemplate>  TemplateList;
   }
}

using namespace GUI::Utils;
