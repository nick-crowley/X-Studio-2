#pragma once

#include "Common.h"
#include "Descriptions.h"

namespace Logic
{
   namespace Language
   {
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
         typedef pair<ScriptObjectGroup,UINT>  ConstID;

         /// <summary></summary>
         class ConstantCollection : public map<ConstID, ConstantDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(ConstantDescription&& d)
            {
               return insert( value_type(ConstID(d.Group,d.ID), d) ).second;
            }
         };

         /// <summary></summary>
         class MacroCollection : public map<wstring, DescriptionMacro>  
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
            bool  Add(DescriptionMacro&& m)
            {
               return insert(value_type(m.Name, m)).second;
            }

         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         DescriptionFile()
         {}
         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(GuiString,Ident,GetIdent);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets ident string</summary>
         /// <returns></returns>
         GuiString GetIdent() const
         {
            return GuiString(L"%s (%s) %s", Title.c_str(), GetString(Language).c_str(), Version.c_str());
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         CommandCollection  Commands;
         ConstantCollection Constants;
         MacroCollection    Macros;

         GameLanguage  Language;
         wstring       Title,
                       Version;
      };
      
   }
}

using namespace Logic::Language;
