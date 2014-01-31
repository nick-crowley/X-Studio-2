#pragma once
#include "Common.h"
#include "XmlReader.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary>Reader for MSCI scripts</summary>
      class ScriptFileReader : private XmlReader
      {
      private:
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
            wstring         ReadStringNode(const wchar* help) { return Reader.ReadString(ReadNode(), help);   }
            int             ReadIntNode(const wchar* help)    { return Reader.ReadInt(ReadNode(), help);      }
            ParameterValue  ReadValueNode(const wchar* help)  { return Reader.ReadValue(ReadNode(), help);    }
            DataType        ReadTypeNode(const wchar* help)   { return (DataType)LOWORD(ReadIntNode(help));   }

            ScriptParameter ReadParameter(ParameterSyntax s, const wchar* help) 
            { 
               DataType dt = ReadTypeNode(help);
               return ScriptParameter(s, dt, ReadValueNode(help)); 
            }

         private:
            XmlNodePtr     ReadNode()       { return Command->childNodes->item[NodeIndex++]; }

            // -------------------- REPRESENTATION ---------------------
         protected:
            ScriptFileReader&     Reader;
            ScriptFile&           Script;
            XmlNodePtr&           Command;

         private:
            int                   NodeIndex;
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



      private:
         /// <summary>Unique command reader pointer</summary>
         typedef unique_ptr<ScriptCommandReader>  ReaderPtr;

         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptFileReader(StreamPtr in);
         virtual ~ScriptFileReader();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------

      public:
         ScriptFile     ReadFile(Path path, bool justProperties);

      private:
         ReaderPtr      GetCommandReader(ScriptFile& script, CommandType type, XmlNodePtr& cmdBranch);

         int            ReadArray(XmlNodePtr& node, const WCHAR* help);
         GameVersion    ReadEngineVersion(XmlNodePtr& node);
         ScriptFile     ReadExternalScript(const wstring& name);
         int            ReadInt(XmlNodePtr& node, const WCHAR* help);
         wstring        ReadString(XmlNodePtr& node, const WCHAR* help);
         ParameterValue ReadValue(XmlNodePtr& node, const WCHAR* help);

         void  ReadCommands(ScriptFile&  script, XmlNodePtr& stdBranch, XmlNodePtr& auxBranch);
         void  ReadVariables(ScriptFile&  script, XmlNodePtr& varBranch, XmlNodePtr& argBranch);

		   // -------------------- REPRESENTATION ---------------------
      private:
         Path  Folder;
      };

      
   }
}

using namespace Logic::IO;
