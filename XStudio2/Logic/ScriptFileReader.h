#pragma once
#include "Common.h"
#include "XmlReader.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>Converts between game and engine versions</summary>
      class EngineVersionConverter
      {
      public:
         /// <summary>Get game version from engine version</summary>
         /// <param name="engine">engine version</param>
         /// <returns>matching game version</returns>
         static GameVersion  ToGame(int engine)
         {
            return engine <= 24 ? GameVersion::Threat
                 : engine <= 39 ? GameVersion::Reunion
                 : engine <= 44 ? GameVersion::TerranConflict
                                : GameVersion::AlbionPrelude;
         }

         /// <summary>Get engine version from game version</summary>
         /// <param name="ver">game version</param>
         /// <returns>matching engine version</returns>
         static int  ToVersion(GameVersion ver)
         {
            switch (ver)
            {
            case GameVersion::Threat:          return 24;
            case GameVersion::Reunion:         return 39;
            case GameVersion::TerranConflict:  return 44;
            case GameVersion::AlbionPrelude:   return 50;
            default:
               throw ArgumentException(HERE, L"ver", VString(L"Unrecognised game version %d", ver));
            }
         }
      };

      /// <summary>Base class for MSCI script reader</summary>
      class SourceValueReader : public XmlReader
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         SourceValueReader(StreamPtr in);
         virtual ~SourceValueReader();

         // ----------------------- MUTATORS ------------------------
      public:
         int            ReadArray(XmlNodePtr& parent, UINT index, const WCHAR* help);
         int            ReadInt(XmlNodePtr& parent, UINT index, const WCHAR* help);
         wstring        ReadString(XmlNodePtr& parent, UINT index, const WCHAR* help);
         ParameterValue ReadValue(XmlNodePtr& parent, UINT index, const WCHAR* help);

      protected:
         void  LoadDocument() override;

         // -------------------- REPRESENTATION ---------------------
      public:
         XmlNodePtr  CodeArray;
      };


      /// <summary>Reader for MSCI scripts</summary>
      class ScriptFileReader : protected SourceValueReader
      {
      protected:
         /// <summary>Base class for MSCI script command readers</summary>
         class ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         protected:
            ScriptCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : Reader(r), Script(s), Command(cmd), NodeIndex(0) {}
         
            // ----------------------- MUTATORS ------------------------
         public:
            virtual ScriptCommand  ReadCommand() PURE;

         protected:
            wstring         ReadStringNode(const wchar* help);
            int             ReadIntNode(const wchar* help);
            ParameterValue  ReadValueNode(const wchar* help);
            DataType        ReadTypeNode(const wchar* help);

            ScriptParameter ReadParameter(ParameterSyntax s, const wchar* help);

            // -------------------- REPRESENTATION ---------------------
         protected:
            ScriptFileReader&  Reader;
            ScriptFile&        Script;
            XmlNodePtr&        Command;

         private:
            int   NodeIndex;
         };

         /// <summary>Reads typical auxiliary commands</summary>
         class AuxiliaryCommandReader : public ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            AuxiliaryCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : ScriptCommandReader(r, s, cmd) {}

            // ----------------------- MUTATORS ------------------------

            ScriptCommand  ReadCommand();
         };

         /// <summary>Reads command comments</summary>
         class CommentedCommandReader : public ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommentedCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : ScriptCommandReader(r, s, cmd) {}

            // ----------------------- MUTATORS ------------------------

            ScriptCommand  ReadCommand();
         };

         /// <summary>Reads variable argument script calls</summary>
         class ScriptCallCommandReader : public ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ScriptCallCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : ScriptCommandReader(r, s, cmd) {}

            // ----------------------- MUTATORS ------------------------

            ScriptCommand  ReadCommand();
         };

         /// <summary>Reads expression commands</summary>
         class ExpressionCommandReader : public ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ExpressionCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : ScriptCommandReader(r, s, cmd) {}

            // ----------------------- MUTATORS ------------------------

            ScriptCommand  ReadCommand();
         };

         /// <summary>Reads typical standard commands</summary>
         class StandardCommandReader : public ScriptCommandReader
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            StandardCommandReader(ScriptFileReader& r, ScriptFile& s, XmlNodePtr& cmd) : ScriptCommandReader(r, s, cmd) {}

            // ----------------------- MUTATORS ------------------------

            ScriptCommand  ReadCommand();
         };

      protected:
         /// <summary>Unique command reader pointer</summary>
         typedef unique_ptr<ScriptCommandReader>  ReaderPtr;

         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptFileReader(StreamPtr in);
         virtual ~ScriptFileReader();

         // ------------------------ STATIC -------------------------
      public:
         static ScriptFile  ReadExternalScript(Path folder, const wstring& script, bool silent = true);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------
      public:
         ScriptFile  ReadFile(Path path, bool justProperties, bool dropJMPs = true);

      protected:
         ReaderPtr   GetCommandReader(ScriptFile& script, CommandType type, XmlNodePtr& cmdBranch);
         void        ReadCommands(ScriptFile&  script, XmlNodePtr& stdBranch, XmlNodePtr& auxBranch, bool dropJMPs);
         void        ReadVariables(ScriptFile&  script, XmlNodePtr& varBranch, XmlNodePtr& argBranch);

		   // -------------------- REPRESENTATION ---------------------
      protected:
         Path  Folder;
      };

      
   }
}

using namespace Logic::IO;
