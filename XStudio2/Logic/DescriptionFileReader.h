#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace IO
   {
      /// <summary></summary>
      class Description
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         Description(const wstring& txt) : Text(txt)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  Text;
      };



      /// <summary></summary>
      class CommandDescription : public Description
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         CommandDescription(UINT id, GameVersion ver, const wstring& txt) : Description(txt), ID(id), Version(ver)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         UINT         ID;
         GameVersion  Version;
      };



      /// <summary></summary>
      class ConstantDescription : public Description
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ConstantDescription(KnownPage page, UINT id, const wstring& txt) : Description(txt), Page(page), ID(id)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         UINT       ID;
         KnownPage  Page;
      };



      /// <summary></summary>
      class DescriptionFile
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary></summary>
         typedef pair<UINT,GameVersion>  CmdID;

         /// <summary></summary>
         class CommandCollection : public map<CmdID, CommandDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(CommandDescription&& d)
            {
               return insert( value_type(CmdID(d.ID,d.Version), d) ).second;
            }
         };

         /// <summary></summary>
         typedef pair<KnownPage,UINT>  ConstID;

         /// <summary></summary>
         class ConstantCollection : public map<ConstID, ConstantDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(ConstantDescription&& d)
            {
               return insert( value_type(ConstID(d.Page,d.ID), d) ).second;
            }
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         DescriptionFile()
         {}
         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         CommandCollection  Commands;
         ConstantCollection Constants;

         GameLanguage  Language;
         wstring       Title,
                       Version;
      };

      /// <summary></summary>
      class DescriptionFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary></summary>
         class Macro
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            Macro(const wstring& nm, const wstring& txt, UINT params, bool recurse) 
               : Name(nm), Text(txt), ParamCount(params), Recursive(recurse)
            {}

            // -------------------- REPRESENTATION ---------------------

            wstring  Name,
                     Text;
            UINT     ParamCount;
            bool     Recursive;
         };

         /// <summary></summary>
         class MacroCollection : public map<wstring, Macro>  
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            MacroCollection()
            {}

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Add a macro to the collection</summary>
            /// <param name="m">The macro.</param>
            /// <returns></returns>
            bool  Add(Macro&& m)
            {
               return insert(value_type(m.Name, m)).second;
            }

            /// <summary>Tries to find a macro</summary>
            /// <param name="id">macro name</param>
            /// <param name="out">macro if found, otherwise nullptr</param>
            /// <returns></returns>
            bool  TryFind(const wstring& id, const Macro* &out) const
            {
               auto it = find(id);
               out = (it != end() ? &it->second : nullptr);
               return out != nullptr;
            }
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         DescriptionFileReader(StreamPtr in);
         virtual ~DescriptionFileReader();

         NO_COPY(DescriptionFileReader);	// No copy semantics
         NO_MOVE(DescriptionFileReader);	// No move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const wregex  MatchKeyword,
                              MatchMacro,
                              MatchParameters;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      private:         
         wstring  onMatchKeyword(const wsmatch& match, int depth) const;
         wstring  onMatchMacro(const wsmatch& match, int depth) const;
         wstring  Parse(wstring text, int depth = 0) const;

         // ----------------------- MUTATORS ------------------------
      public:
         DescriptionFile  ReadFile();

      private:
         CommandDescription  ReadCommand(XmlNodePtr n);
         ConstantDescription ReadConstant(XmlNodePtr n);
         Macro               ReadMacro(XmlNodePtr n);
         
         void  ReadCommands(DescriptionFile& file);
         void  ReadConstants(DescriptionFile& file);
         void  ReadMacros();

         // -------------------- REPRESENTATION ---------------------

      private:
         const UINT  BUFFER_LENGTH = 32*1024;

         MacroCollection  Macros;
         XmlNodePtr       Root;
         CharArrayPtr     FormatBuffer;
      };

   }
}

using namespace Logic::IO;
