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
      /// <param name="title">file title</param>
      /// <param name="version">file version</param>
      void  SyntaxWriter::Write(const SyntaxFile& f, const wstring& title, const wstring& version)
      {
         map<ParameterType, wstring>  paramNames;
         map<CommandGroup, wstring>   groupNames;

         // Header
         WriteInstruction(L"version='1.0' encoding='utf-8'");
         WriteComment(L"Written by X-Studio II");

         // Root
         auto root = WriteRoot(L"syntax");
         WriteAttribute(root, L"title", title);
         WriteAttribute(root, L"version", version);

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

            // Id + Type 
            WriteAttribute(e, L"id", (int)cmd.ID);
            WriteAttribute(e, L"type", GetString(cmd.Type));
            
            // Version: Build comma delimited string of acronyms
            wstring vers;
            for (GameVersion v : {GameVersion::Threat, GameVersion::Reunion, GameVersion::TerranConflict, GameVersion::AlbionPrelude})
               if (cmd.Versions & (UINT)v)
                  vers.append( VString(vers.empty()?L"%s":L",%s", VersionString(v, true).c_str()) );

            // Version+Group
            WriteAttribute(e, L"version", vers);
            WriteAttribute(e, L"group", groupNames[cmd.Group]);

            // Text/URL
            WriteElement(e, L"text", cmd.Text);
            WriteElement(e, L"url", cmd.URL);

            // Execution
            auto ee = WriteElement(e, L"execution", GetString(cmd.Execution));
            WriteAttribute(ee, L"yield", cmd.IsInterrupt() ? L"true" : L"false");
            
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

            // Vargs
            auto ve = WriteElement(e, L"vargs", GetString(cmd.VArgument));
            WriteAttribute(ve, L"count", cmd.VArgCount);
            WriteAttribute(ve, L"encoding", GetString(cmd.VArgParams));
         }
      }


		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
