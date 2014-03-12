#include "stdafx.h"
#include "BackupFileReader.h"
#include "LanguageFileReader.h"

namespace Logic
{
   namespace IO
   {
   
      

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a description xml file reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      BackupFileReader::BackupFileReader(StreamPtr in) : XmlReader(in)
      {
         
      }


      BackupFileReader::~BackupFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire descriptions file</summary>
      /// <returns>New descriptions file</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Invalid date / Corrupt XML / Missing attributes/elements</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid language ID -or- invalid command version</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      BackupFile  BackupFileReader::ReadFile()
      {
         try
         {
            BackupFile file;

            // Read file
            LoadDocument();

            // Get root 
            Root = Document->documentElement;
            ReadElement(Root, L"backups");

            // Read file properties
            if (ReadAttribute(Root, L"type") == L"msci")
               file.Type = BackupType::MSCI;
            else
               throw FileFormatException(HERE, L"Unrecognised backup type");

            // Read revisions
            ReadScriptRevisions(file);

            // Return file
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Reads a single command description</summary>
      /// <param name="n">command node</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Invalid date / Corrupt XML / Missing attributes/elements</exception>
      /// <exception cref="Logic::InvalidValueException">invalid game version</exception>
      ScriptRevision  BackupFileReader::ReadScriptRevision(XmlNodePtr n)
      {
         ScriptRevision r;

         try
         {
            // Verify tag
            ReadElement(n, L"revision");

            // Properties
            r.Title = ReadAttribute(n, L"title");
            r.FullPath = ReadAttribute(n, L"path");

            // Read date  (UK Format)
            if (!r.Date.ParseDateTime(ReadAttribute(n, L"date").c_str(), 0, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK)))
               throw FileFormatException(HERE, L"Unable to parse revision date");

            // Read script properties
            r.ScriptName  = GetChild(n, 0, L"script name")->text;
            r.Version     = GuiString(GetChild(n, 1, L"script version")->text).ToInt();
            r.Description = GetChild(n, 3, L"script description")->text;
            r.Content     = GetChild(n, 4, L"script text")->text;

            // Read game version
            wstring ver = GetChild(n, 2, L"script engine version")->text;
            if (ver == L"X2")
               r.Game = GameVersion::Threat;
            else if (ver == L"X3R")
               r.Game = GameVersion::Reunion;
            else if (ver == L"X3TC")
               r.Game = GameVersion::TerranConflict;
            else if (ver == L"X3AP")
               r.Game = GameVersion::AlbionPrelude;
            else
               throw InvalidValueException(HERE, GuiString(L"Unrecognised engine version '%s'", ver.c_str()));

            // Done
            return r;  
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }
      

      /// <summary>Reads all the revisions</summary>
      /// <param name="file">backup file</param>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">invalid command version</exception>
      void  BackupFileReader::ReadScriptRevisions(BackupFile& file)
      {
         try
         {
            // Read revisions
            for (int i = 0, count = Root->childNodes->length; i < count; i++)
            {
               XmlNodePtr n = GetChild(Root, i, L"script revision");

               // Read all elements
               if (n->nodeType == Xml::NODE_ELEMENT)
                  file.Revisions.Add( ReadScriptRevision(n) );
            }
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }



   }
}
