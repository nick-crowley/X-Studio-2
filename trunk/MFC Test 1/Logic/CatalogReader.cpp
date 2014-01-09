#include "stdafx.h"
#include "CatalogReader.h"
#include <algorithm>


namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="CatalogReader"/> class.</summary>
      /// <param name="src">The stream to read from</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      CatalogReader::CatalogReader(StreamPtr src) : StringReader(src)
      {
         // Consume header on first line
         string line;
         StringReader::ReadLine(line);
      }


      CatalogReader::~CatalogReader()
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the next declaration, if any</summary>
      /// <param name="path">The file sub-path</param>
      /// <param name="size">The file offset</param>
      /// <returns>true if found, false if EOF</returns>
      /// <exception cref="Logic::FileFormatException">Declaration is corrupt</exception>
      /// <exception cref="Logic::InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
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
         
         // Parse path: Convert char->wchar and '/'->'\'
         path.clear();
         transform(line.begin(), line.begin()+gap, back_inserter(path), [](char ch) { return (WCHAR)(ch == '/' ? '\\' : ch); } );

         // Parse size
         size = atoi( string(line.begin()+gap+1, line.end()).c_str() );
         return true;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------
   }
}