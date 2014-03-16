#include "stdafx.h"
#include "BackupFileWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a syntax writer for an output stream</summary>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      BackupFileWriter::BackupFileWriter(StreamPtr out) : XmlWriter(out)
      {
      }


      BackupFileWriter::~BackupFileWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes and flushes the output stream</summary>
      void  BackupFileWriter::Close()
      {
         __super::Close();
      }

      /// <summary>Writes a Backup file</summary>
      /// <param name="f">The file</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  BackupFileWriter::WriteFile(const BackupFile& f)
      {
         // Header
         WriteInstruction(L"version='1.0' encoding='UTF-8'");
         WriteComment(L"Written by X-Studio II");

         // Root: File type
         auto root = WriteRoot(L"backups");
         WriteAttribute(root, L"type", GetString(f.Type));

         // Write revision (newest -> oldest)
         for (auto& rev : f.Revisions)
            WriteRevision(rev, root);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Writes a revision</summary>
      /// <param name="r">Revision.</param>
      /// <param name="parent">root node.</param>
      /// <exception cref="Logic::ArgumentNullException">Parent is nullptr</exception>
      void  BackupFileWriter::WriteRevision(const ScriptRevision& r, XmlElementPtr& parent)
      {
         REQUIRED(parent);

         // <revision title="third revision" date="2013-03-12 18:00:00" path="D:\X3 Albion Prelude\scripts\plugin.piracy.lib.logic.xml">  
         auto node = WriteElement(parent, L"revision");
         WriteAttribute(node, L"title", r.Title);
         WriteAttribute(node, L"date", (LPCWSTR)r.Date.Format(L"%Y-%m-%d %H:%M:%S"));  // yyyy-mm-dd hh:mm:ss
         WriteAttribute(node, L"path", r.FullPath.c_str());

         //  <scriptname>plugin.piracy.lib.logic</scriptname>
         //  <version>102</version>
         //  <game>X3TC</game>
         //  <description>Piracy logic library</description>
         //  <command>1019</command>
         //  <text>* Validate Parameters...</text>
         WriteElement(node, L"scriptname", r.ScriptName);
         WriteElement(node, L"version", VString(L"%d", r.Version));
         WriteElement(node, L"game", VersionString(r.Game, true));
         WriteElement(node, L"description", r.Description);
         WriteElement(node, L"command", VString(L"%d", r.CommandID));
         WriteElement(node, L"text", r.Content);
      }


      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
