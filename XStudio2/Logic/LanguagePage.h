#pragma once
#include "Common.h"
#include "MapIterator.hpp"

namespace Logic
{
   namespace Language
   {
      /// <summary>Categorises language page ID</summary>
      enum class PageGroup : UINT { DATA, DIALOGUE, MISC, QUEST, BONUS, MENU, EDITOR, NEWS, PLOT, NPC, USER };

      /// <summary>Defines the game pages containing the definitions for the various game constants</summary>
      enum KnownPage : UINT
      {  
         SECTORS              = 7,
         STATION_SERIALS      = 12,
         NAMES_DESCRIPTIONS   = 17,
         RELATIONS            = 35,
         RACES                = 1266,
         TRANSPORT_CLASSES    = 1999,
         PARAMETER_TYPES      = 2000,
         OPERATORS            = 2001,
         CONSTANTS            = 2002,
         COMMAND_SYNTAX       = 2003,
         OBJECT_CLASSES       = 2006,
         CONDITIONALS         = 2007,
         OBJECT_COMMANDS      = 2008,
         FLIGHT_RETURNS       = 2009,
         DATA_TYPES           = 2013,
         WING_COMMANDS        = 2028 
      };

      /// <summary>Language string with immutable ID</summary>
      class LanguageString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         LanguageString(UINT  id, UINT page, wstring  txt, GameVersion v);
         LanguageString(LanguageString&& r);
         ~LanguageString();

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(wstring,ResolvedText,GetResolvedText);

		   // ---------------------- ACCESSORS ------------------------
      public:
         wstring GetResolvedText() const;
         bool    IsScriptObject() const;

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         const UINT   ID,
                      Page;
         wstring      Text;
         GameVersion  Version;
      };

      /// <summary>Represents a collection of language strings</summary>
      class LanguagePage
      {
      public:
         /// <summary>Collection of language strings, sorted by ID</summary>
         class StringCollection : public map<UINT, LanguageString, less<UINT>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            StringCollection(UINT page) : PageID(page) 
            {}
            StringCollection(StringCollection&& r) : map<UINT, LanguageString, less<UINT>>(move(r)), PageID(r.PageID)
            {}

            // ---------------------- ACCESSORS ------------------------
            
            /// <summary>Query whether a string is present</summary>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            bool  Contains(UINT  id) const
            {
               return find(id) != end();
            }

            /// <summary>Finds the specified string.</summary>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            const LanguageString&  Find(UINT  id) const
            {
               const_iterator it;

               if ((it=find(id)) == end())
                  throw StringNotFoundException(HERE, PageID, id);

               return it->second;
            }
            
            /// <summary>Finds the specified string.</summary>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            const LanguageString&  operator[](UINT  id) const
            {
               return Find(id);
            }

            // ----------------------- MUTATORS ------------------------
            
            /// <summary>Adds a string to the collection. If a lower version string exists, it is overwritten</summary>
            /// <param name="s">The string</param>
            /// <returns>True if inserted, otherwise false</returns>
            bool  Add(LanguageString& s)
            {
               // Attempt to insert
               auto it = insert(value_type(s.ID, s));
               if (it.second)
                  return true;

               // Exists: Overwrite if version is greater
               if ((UINT)s.Version > (UINT)(it.first->second).Version)
               {
                  erase(it.first);
                  insert(value_type(s.ID, s));
                  return true;
               }

               return false;
            }

            /// <summary>Finds a string by index</summary>
            /// <param name="index">The index</param>
            /// <returns></returns>
            /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
            LanguageString&  FindByIndex(UINT index) 
            {
               UINT i = 0;

               // Linear search
               for (iterator it = begin(); it != end(); ++it)
                  if (index == i++)
                     return it->second;

               // Not found
               throw IndexOutOfRangeException(HERE, index, size());
            }

		      // -------------------- REPRESENTATION ---------------------
         private:
            UINT  PageID;
         };

         /// <summary>Provides constant access to the strings within a language page</summary>
         typedef MapIterator<const LanguageString, StringCollection, StringCollection::const_iterator> const_iterator;

         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguagePage(UINT id, wstring title, wstring desc, bool voice);
         LanguagePage(LanguagePage&& r);
         ~LanguagePage();

         // ----------------------- STATIC ------------------------

         /// <summary>Get category of a page id</summary>
         static PageGroup  IdentifyGroup(UINT page);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

         /// <summary>Get constant string iterator</summary>
         /// <returns></returns>
         const_iterator begin() const 
         { 
            return const_iterator(Strings, Strings.begin()); 
         }

         /// <summary>Query whether a string is present</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         bool Contains(UINT  id) const
         { 
            return Strings.Contains(id); 
         }

         /// <summary>Get constant string iterator</summary>
         /// <returns></returns>
         const_iterator end() const 
         { 
            return const_iterator(Strings, Strings.end()); 
         }

         /// <summary>Finds the specified string.</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         const LanguageString&  Find(UINT  id) const
         { 
            return Strings.Find(id);     
         }

         /// <summary>Get display category of this page when used in the string library viewer</summary>
         PageGroup  GetGroup() const;

         /// <summary>Finds the specified string.</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         const LanguageString&  operator[](UINT  id) const
         { 
            return Strings.Find(id);     
         }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         const UINT       ID;
         wstring          Title,
                          Description;
         bool             Voiced;
         
         StringCollection Strings;

      };
   }
}

using namespace Logic::Language;
