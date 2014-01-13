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

      /// <summary>Gets the super script (if any) of the parameter</summary>
      /// <param name="cmd">The command syntax</param>
      /// <param name="p">Parameter syntax</param>
      /// <returns>Index 0 to 5 if any, otherwise -1</returns>
      int SyntaxWriter::GetSuperScript(const CommandSyntax& cmd, const ParameterSyntax& p)
      {
         auto mark = StringResource::Format(L"$%d", p.PhysicalIndex);

         // Find $0 marker
         if (cmd.Text.find(mark) != wstring::npos)
         {
            // Jump to the letter (if any) following the number
            auto it = cmd.Text.begin() + cmd.Text.find(mark) + 2;

            if (it != cmd.Text.end())
               switch (*it)
               {
               /*case L'º': case L'¹': case L'²': case L'³': case L'ª':
               case 'o': case 'x': case 'y': case 'z': case 'a':*/
               case L'º': 
               case 'o':  
                  return 0;

               case L'¹': 
               case 'x':  
                  return 1;

               case L'²': 
               case 'y': 
                  return 2;

               case L'³': 
               case 'z': 
                  return 3;

               case L'ª':
               case 'a':
                  return 4;
               }
         }

         return -1;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Writes the syntax file to the output stream</summary>
      void  SyntaxWriter::Close()
      {
         XmlWriter::Close();
      }

      /// <summary>Writes a syntax file in the new format</summary>
      /// <param name="f">The file</param>
      void  SyntaxWriter::Write(const SyntaxFile& f)
      {
         map<ParameterType, wstring>  paramNames;

         // Header
         WriteInstruction(L"version='1.0' encoding='utf-8'");
         WriteComment(L"Written by X-Studio II");
         auto root = WriteRoot(L"syntax");

         // Write parameters
         auto paramSyntax = WriteElement(root, L"parameters");
         for (const auto& pair : f.Types)
         {
            // Name/value
            auto e = WriteElement(paramSyntax, L"param", pair.first);
            WriteAttribute(e, L"value", (int)pair.second);

            // Store for rev lookup
            paramNames[pair.second] = pair.first;
         }

         // Write commands
         auto commandSyntax = WriteElement(root, L"commands");
         for (const auto& pair : f.Commands)
         {
            const CommandSyntax& cmd = pair.second;
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
            WriteElement(e, L"type", cmd.Type == CommandType::Standard ? L"Standard" 
                                   : cmd.Type == CommandType::Auxiliary ? L"Auxiliary" : L"Macro");
            WriteElement(e, L"text", cmd.Text);
            WriteElement(e, L"group", L"TODO");
            WriteElement(e, L"url", cmd.URL);
            
            // Parameters
            int index = 0;
            auto cmdParams = WriteElement(e, L"params");
            for (auto p : cmd.Parameters)
            {
               // Type/Index
               e = WriteElement(cmdParams, L"param", paramNames[p.Type]);
               WriteAttribute(e, L"displayIndex", p.DisplayIndex);

               // Superscript
               int superscript = GetSuperScript(cmd, p);
               if (superscript != -1)
                  WriteAttribute(e, L"superscript", superscript);

               // Extra properties
               if (p.ScriptName)
                  WriteAttribute(e, L"scriptname", L"true");
               if (p.PageID)
                  WriteAttribute(e, L"pageid", L"true");
               if (p.StringID)
                  WriteAttribute(e, L"stringid", L"true");
               if (p.Optional)
                  WriteAttribute(e, L"optional", L"true");
            }
         }
      }


		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}