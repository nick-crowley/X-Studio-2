#pragma once
#include "Common.h"
#include "LanguageString.h"

using namespace Logic::Language;

namespace Logic
{
   namespace Language
   {
      /// <summary>Represents a collection of language strings</summary>
      class LanguagePage
      {
      public:
         /// <summary>Collection of language strings, sorted by ID</summary>
         class StringCollection : public map<UINT, LanguageString>
         {
         public:
            /// <summary>Adds a string to the collection, overwriting any with a lower version</summary>
            /// <param name="s">The string</param>
            /// <returns>True if successful, false otherwise</returns>
            bool  Add(LanguageString& s)
            {
               auto res = insert(value_type(s.ID, s));

               if (res.second)
                  return true;

               // Overwrite if version is greater
               if (!res.second && (UINT)s.Version > (UINT)(res.first->second).Version)
               {
                  res.first->second = s;
                  return true;
               }

               return false;
            }
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

         UINT             ID;
         wstring          Title,
                          Description;
         bool             Voiced;
         
         StringCollection Strings;

      };
   }
}