#pragma once

#include "Common.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Language
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

   }
}

using namespace Logic::Language;
