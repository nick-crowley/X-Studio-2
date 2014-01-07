#pragma once
#include "Common.h"
#include "LanguageString.h"

namespace Library
{

   /// <summary>Represents a collection of language strings</summary>
   class LanguagePage
   {
   private:
      /// <summary>Sorts language strings by ID (no duplicates)</summary>
      struct UniqueID : public binary_function<LanguageString, LanguageString, bool>
	   {	
         // Sort by ID (ascending)
	      bool operator()(const LanguageString& a, const LanguageString& b) const
		   {	
		      return a.ID < b.ID;
		   }
	   };

   public:
      /// <summary>Collection of language strings, sorted by ID (lowest to highest, no duplicates)</summary>
      class StringCollection : public set<LanguageString, UniqueID>
      {
      };

      // --------------------- CONSTRUCTION ----------------------

   public:
      LanguagePage() : ID(0) {};
      LanguagePage(UINT id, wstring title, wstring desc, bool voice) : ID(id), Title(title), Description(desc) {};
      ~LanguagePage() {};

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

      UINT     ID;
      wstring  Title,
               Description;

      StringCollection Strings;

   };

}