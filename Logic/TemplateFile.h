#pragma once

namespace Logic
{
   /// <summary>Defines the properties of a document template</summary>
   class LogicExport TemplateFile
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create template file.</summary>
      /// <param name="name">Template name.</param>
      /// <param name="type">File type.</param>
      /// <param name="desc">Template description.</param>
      /// <param name="subpath">subpath into application folder of actual template.</param>
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

   /// <summary>List of file templates</summary>
   typedef list<TemplateFile>  TemplateList;
}

using namespace Logic;
