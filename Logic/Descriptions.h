#pragma once


#include "ScriptObject.h"

namespace Logic
{
   namespace Language
   {
      /// <summary></summary>
      class LogicExport Description
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
      class LogicExport CommandDescription : public Description
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
      class LogicExport ConstantDescription : public Description
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ConstantDescription(ScriptObjectGroup grp, UINT id, const wstring& txt) : Description(txt), Group(grp), ID(id)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         UINT               ID;
         ScriptObjectGroup  Group;
      };


      /// <summary></summary>
      class LogicExport DescriptionMacro
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         DescriptionMacro(const wstring& nm, const wstring& txt, UINT params, bool recurse) 
            : Name(nm), Text(txt), ParamCount(params), Recursive(recurse)
         {}

         // -------------------- REPRESENTATION ---------------------

         wstring  Name,
                  Text;
         UINT     ParamCount;
         bool     Recursive;
      };
   }
}

using namespace Logic::Language;
