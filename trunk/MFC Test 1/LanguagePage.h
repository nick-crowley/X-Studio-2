#pragma once
#include "Common.h"
#include "LanguageString.h"

namespace Library
{

   class LanguagePage
   {
   private:
	   struct UniqueID : public binary_function<LanguageString, LanguageString, bool>
	   {	
         // Sort by ID (ascending)
	      bool operator()(const LanguageString& a, const LanguageString& b) const
		   {	
		      return a.ID < b.ID;
		   }
	   };

   public:
      class StringCollection : public set<LanguageString, UniqueID>
      {
      };

   public:
      LanguagePage() : ID(0) {};
      LanguagePage(UINT id, wstring title, wstring desc, bool voice) : ID(id), Title(title), Description(desc) {};
      ~LanguagePage() {};

      UINT     ID;
      wstring  Title,
               Description;

      StringCollection Strings;

   };

}