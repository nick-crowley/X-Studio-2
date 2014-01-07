#include "stdafx.h"
#include "XFileSystem.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {

      CatalogReader::CatalogReader(XFileSystem&  vfs, XCatalog&  cat, Stream*  ps, bool  owner) 
         : IO::StringReader(ps, owner), FileSystem(vfs), Catalog(cat)
      {
         // Ignore header
         string line;
         StringReader::ReadLine(line);
      }


      CatalogReader::~CatalogReader()
      {
      }


      XFileInfo*  CatalogReader::ReadDeclaration()
      {
         string  line;

         // Check for EOF
         if (!StringReader::ReadLine(line))
            return nullptr;

         // Parse declaration
         string::size_type gap = line.find_last_of(' ');
         if (gap == string::npos)
            throw FileFormatException(HERE, L"Invalid file declaration");
         
         // Parse declaration
         string  size(line.begin()+gap+1, line.end());
         wstring path(line.begin(), line.begin()+gap);

         // Set info
         return new FileSystem::XFileInfo(FileSystem, Catalog, path, atoi(size.c_str()));
      }

   }
}