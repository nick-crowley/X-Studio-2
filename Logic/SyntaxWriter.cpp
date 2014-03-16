#include "stdafx.h"
#include "SyntaxWriter.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a syntax writer for an output stream</summary>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      SyntaxWriter::SyntaxWriter(StreamPtr out) : XmlWriter(out)
      {
      }

      SyntaxWriter::~SyntaxWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes and flushes the output stream</summary>
      void  SyntaxWriter::Close()
      {
         XmlWriter::Close();
      }

      /// <summary>Writes a syntax file in the new format</summary>
      /// <param name="f">The file</param>
      void  SyntaxWriter::Write(const SyntaxFile& f)
      {
         map<ParameterType, wstring>  paramNames;
         map<CommandGroup, wstring>   groupNames;

         // Header
         WriteInstruction(L"version='1.0' encoding='utf-8'");
         WriteComment(L"Written by X-Studio II");
         auto root = WriteRoot(L"syntax");


         // reverse group names lookup
         for (const auto& pair : f.Groups)
            groupNames[pair.second] = pair.first;

         // Write groups
         auto groupSyntax = WriteElement(root, L"groups");
         for (const auto& pair : groupNames)
         {  
            auto e = WriteElement(groupSyntax, L"group", pair.second);  // Name
            WriteAttribute(e, L"id", (int)pair.first);   // Value
         }


         // reverse param syntax lookup
         for (const auto& pair : f.Types)
            paramNames[pair.second] = pair.first;

         // Write parameters, ordered by type
         auto paramSyntax = WriteElement(root, L"types");
         for (const auto& pair : paramNames)
         {  
            auto e = WriteElement(paramSyntax, L"type", pair.second);  // Name
            WriteAttribute(e, L"value", (int)pair.first);   // Value
         }

         
         // Write commands
         auto commandSyntax = WriteElement(root, L"commands");
         for (const auto& pair : f.Commands)
         {
            CommandSyntaxRef cmd = pair.second;
            auto e = WriteElement(commandSyntax, L"command");

            // Id
            WriteAttribute(e, L"id", (int)cmd.ID);

            // Version: Build string of acronyms
            wstring ver;
            if (cmd.Versions & (UINT)GameVersion::Threat)
               ver = L"X2";
            if (cmd.Versions & (UINT)GameVersion::Reunion)
               ver.append(ver.empty() ? L"X3R" : L",X3R");
            if (cmd.Versions & (UINT)GameVersion::TerranConflict)
               ver.append(ver.empty() ? L"X3TC" : L",X3TC");
            if (cmd.Versions & (UINT)GameVersion::AlbionPrelude)
               ver.append(ver.empty() ? L"X3AP" : L",X3AP");

            WriteAttribute(e, L"version", ver);

            // Properties
            WriteElement(e, L"text", cmd.Text);
            WriteElement(e, L"group", groupNames[cmd.Group]);
            WriteElement(e, L"type", cmd.Type == CommandType::Standard ? L"Standard" 
                                   : cmd.Type == CommandType::Auxiliary ? L"Auxiliary" : L"Macro");
            WriteElement(e, L"url", cmd.URL);
            
            // Parameters
            int index = 0;
            auto cmdParams = WriteElement(e, L"params");
            for (auto p : cmd.Parameters)
            {
               // Type
               e = WriteElement(cmdParams, L"param", paramNames[p.Type]);

               // Display index/ordinal
               WriteAttribute(e, L"displayIndex", p.DisplayIndex);
               if (p.Ordinal != 0)
                  WriteAttribute(e, L"ordinal", p.Ordinal);

               // Usage
               switch (p.Usage) 
               {
               case ParameterUsage::ScriptName:  WriteAttribute(e, L"usage", L"scriptname");  break;
               case ParameterUsage::PageID:      WriteAttribute(e, L"usage", L"pageid");  break;
               case ParameterUsage::StringID:    WriteAttribute(e, L"usage", L"stringid");  break;
               }
            }
         }
      }


		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
