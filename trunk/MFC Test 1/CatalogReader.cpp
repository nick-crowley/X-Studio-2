#include "stdafx.h"
#include "CatalogReader.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace IO
   {

      CatalogReader::CatalogReader(StreamPtr src) : StringReader(src)
      {
         // Ignore header
         string line;
         StringReader::ReadLine(line);
      }


      CatalogReader::~CatalogReader()
      {
      }


      bool  CatalogReader::ReadDeclaration(wstring&  path, DWORD&  size)
      {
         string  line;

         // Check for EOF
         if (!StringReader::ReadLine(line))
            return false;

         // Parse declaration
         string::size_type gap = line.find_last_of(' ');
         if (gap == string::npos)
            throw FileFormatException(HERE, L"Invalid file declaration");
         
         // Parse declaration
         path = wstring(line.begin(), line.begin()+gap);
         size = atoi( string(line.begin()+gap+1, line.end()).c_str() );
         return true;
      }

   }
}