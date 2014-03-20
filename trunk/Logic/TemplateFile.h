#pragma once

namespace Logic
{
   /// <summary>Defines the properties of a document template</summary>
   class TemplateFile
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      TemplateFile(wstring name, FileType type, const wstring& desc, const wstring& subpath) 
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
      const wstring   Name,            // Template name
                      Description,     // Template description
                      SubPath;         // File SubPath: 'Templates\...'
      const FileType  Type;            // Document type
   };

   /// <summary></summary>
   typedef list<TemplateFile>  TemplateList;
}

using namespace Logic;
