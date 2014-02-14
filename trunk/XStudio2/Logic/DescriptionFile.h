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
      
   }
}

using namespace Logic::Language;
