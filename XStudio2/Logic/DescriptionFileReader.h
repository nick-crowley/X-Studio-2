#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "DescriptionFile.h"

namespace Logic
{
   namespace IO
   {
      
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
