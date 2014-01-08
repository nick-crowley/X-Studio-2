
#include "stdafx.h"
#include "Common.h"
#include "LegacySyntaxReader.h"

namespace Library
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      LegacySyntaxReader::LegacySyntaxReader(StreamPtr src) : StringReader(src)
      {
      }

      LegacySyntaxReader::LegacySyntaxReader(LegacySyntaxReader&& r) : StringReader(std::move(r)) 
      {
      }

      LegacySyntaxReader::~LegacySyntaxReader()
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      SyntaxFile   LegacySyntaxReader::ReadFile()
      {
         SyntaxFile  f;
         wstring       group, url, syntax;
         UINT          id, versions;
         list<wstring> params;

         // Read syntax blocks
         while (ReadSyntax(group, versions, id, url, syntax, params))
            f.Add(CommandSyntax(group, versions, id, url, syntax, params));

         return f;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Reads the next syntax block</summary>
      /// <param name="group">The command group name</param>
      /// <param name="versions">The game version compatibility flags</param>
      /// <param name="id">The command ID</param>
      /// <param name="url">The MSCI reference help URL</param>
      /// <param name="syntax">The command syntax text</param>
      /// <param name="params">Collection of parameter syntax</param>
      /// <returns>true if successful, false on EOF</returns>
      /// <exception cref="Library::FileFormatException">Missing syntax component</exception>
      bool  LegacySyntaxReader::ReadSyntax(wstring& group, UINT& versions, UINT& id, wstring& url, wstring& syntax, list<wstring>& params)
      {
         wstring line;

         // Init
         versions = 0;
         params.clear();

         // Group
         if (!TryReadLine(group))
            return false;  // EOF Reached

         // Game versions
         RequireLine(line, L"compatibility flags");
         
         if (line.find(L"X2") != wstring::npos)
            versions |= (UINT)GameVersion::Threat;
         if (line.find(L"X3R") != wstring::npos)
            versions |= (UINT)GameVersion::Reunion;
         if (line.find(L"X3TC") != wstring::npos)
            versions |= (UINT)GameVersion::TerranConflict;
         if (line.find(L"X3AP") != wstring::npos)
            versions |= (UINT)GameVersion::AlbionPrelude;

         // ID
         RequireLine(line, L"ID");
         id = _wtoi(line.c_str());

         // URL
         RequireLine(url, L"help URL");
         // Syntax
         RequireLine(syntax, L"syntax text");

         // Params
         while (RequireLine(line, L"parameter") && line != END_BLOCK)
            params.push_back(line);
      }

      /// <summary>Reads the next line</summary>
      /// <param name="line">The line text</param>
      /// <param name="content">Meaning of line</param>
      /// <returns>true</returns>
      /// <exception cref="Library::FileFormatException">Line Missing</exception>
      bool  LegacySyntaxReader::RequireLine(wstring& line, const WCHAR* content)
      {
         string ansi;

         // Ensure line exists
         if (!ReadLine(ansi))
            throw FileFormatException(HERE, StringResource::Format(L"Missing %s on line %d", content, LineNumber));

         // Convert ansi->wide
         line = wstring(ansi.begin(), ansi.end());
         return true;
      
      }

      /// <summary>Tries to read the next line</summary>
      /// <param name="line">The line text</param>
      /// <returns>true if found, false otherwise</returns>
      bool  LegacySyntaxReader::TryReadLine(wstring& line)
      {
         string ansi;

         // Ensure line exists
         if (!ReadLine(ansi))
            return false;

         // Convert ansi->wide
         line = wstring(ansi.begin(), ansi.end());
         return true;
      }
   }
}