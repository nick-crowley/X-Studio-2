#pragma once

#include "MapIterator.hpp"
#include "RichString.h"

namespace Logic
{
   namespace Language
   {
      class LanguageString;
      class LanguagePage;

      /// <summary>Read-only language string reference</summary>
      typedef const LanguageString&  LanguageStringRef;

      /// <summary>Read-only language page reference</summary>
      typedef const LanguagePage&    LanguagePageRef;

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

      /// <summary>Defines colour tag types used in language strings</summary>
      enum class ColourTag { Undetermined, Unix, Message };

      /// <summary>Represents a string in a language file</summary>
      class LanguageString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         LanguageString(UINT  id, UINT page, wstring  txt, GameVersion v);
         LanguageString(LanguageString&& r);
         ~LanguageString();

         DEFAULT_COPY(LanguageString);	// Default copy semantics

         // ----------------------- STATIC --------------------------
      protected:
         static const wregex  MatchMessageTag;

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,ResolvedText,GetResolvedText);
         PROPERTY_GET(RichString,RichText,GetRichText);

		   // ---------------------- ACCESSORS ------------------------
      public:
         wstring    GetResolvedText() const;
         RichString GetRichText() const;
         bool       IsScriptObject() const;

		   // ----------------------- MUTATORS ------------------------
      public:
         ColourTag  IdentifyColourTags();

		   // -------------------- REPRESENTATION ---------------------
      public:
         UINT         ID,
                      Page;
         wstring      Text;
         GameVersion  Version;
         ColourTag    TagType;
      };

      /// <summary>Write string to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const LanguageString& str);

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

            /// <summary>Read-only access to strings by ID.</summary>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            LanguageStringRef  Find(UINT  id) const
            {
               const_iterator it;

               if ((it=find(id)) == end())
                  throw StringNotFoundException(HERE, PageID, id);

               return it->second;
            }
            
            /// <summary>Read-only access to strings by ID.</summary>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            LanguageStringRef  operator[](UINT  id) const
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

            /// <summary>Read-write access to strings by index</summary>
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

            /// <summary>Inserts a string</summary>
            /// <param name="s">string.</param>
            /// <returns>Zero-based index if successful, otherwise -1</returns>
            int  Insert(LanguageStringRef s)
            {
               // Attempt to insert
               auto pos = insert(value_type(s.ID, s));
               
               // Return index
               return pos.second ? distance(begin(), pos.first) : -1;
            }

            /// <summary>Remove a string by ID</summary>
            /// <param name="id">The string id</param>
            /// <returns>Index of removed string</returns>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            int  Remove(UINT  id) 
            { 
               const_iterator pos;

               // Lookup string
               if ((pos=find(id)) == end())
                  throw StringNotFoundException(HERE, PageID, id);
               int index = distance(cbegin(), pos);

               // Remove + return index
               erase(pos);
               return index;
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

         DEFAULT_COPY(LanguagePage);	// Default copy semantics

         // ----------------------- STATIC ------------------------

         /// <summary>Get category of a page id</summary>
         static PageGroup  IdentifyGroup(UINT page);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

         /// <summary>Get read-only string iterator</summary>
         /// <returns></returns>
         const_iterator begin() const 
         { 
            return const_iterator(Strings, Strings.begin()); 
         }
         
         /// <summary>Get read-only string iterator</summary>
         /// <returns></returns>
         const_iterator end() const 
         { 
            return const_iterator(Strings, Strings.end()); 
         }

         /// <summary>Adds a string to the collection. If a lower version string exists, it is overwritten</summary>
         /// <param name="s">The string</param>
         /// <returns>True if inserted, otherwise false</returns>
         bool  Add(LanguageString& s)
         {
            return Strings.Add(s);
         }
         
         /// <summary>Query whether a string is present</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         bool Contains(UINT  id) const
         { 
            return Strings.Contains(id); 
         }

         /// <summary>Read-only access to string by ID</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         LanguageStringRef  Find(UINT  id) const
         { 
            return Strings.Find(id);     
         }

         /// <summary>Get display category of this page when used in the string library viewer</summary>
         PageGroup  GetGroup() const;
         
         /// <summary>Finds an available ID for a new string</summary>
         /// <param name="start">ID of insertion point, or -1 to append after last string.</param>
         /// <returns>ID preceeding input, if available, otherwise the first ID following the input.</returns>
         int  GetAvailableID(int start) const
         {
            // Empty/Append: Return LastID+1
            if (start == -1 || Strings.empty())
               return !Strings.empty() ? (--Strings.end())->first+1 : 1;

            // Prev Available: Use previous ID
            if (Strings.find(start-1) == Strings.end())
               return start-1;

            // Find next ID
            auto pos = Strings.find(start);
            for (int id = start; pos != Strings.end(); id++)
            {
               // Gap Detected: Use first available ID
               if ((UINT)id < (pos++)->first)
                  return id;
            }

            // Contiguous: Use LastID+1
            return (--Strings.end())->first+1;
         }

         /// <summary>Determines whether a string ID is available.</summary>
         /// <param name="id">The identifier.</param>
         /// <returns></returns>
         bool IsAvailable(UINT id) const
         {
            return !Contains(id);
         }

         /// <summary>Read-only access to string by ID.</summary>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         LanguageStringRef  operator[](UINT  id) const
         { 
            return Strings.Find(id);     
         }

		   // ----------------------- MUTATORS ------------------------
         
         /// <summary>Read-write access to strings by index</summary>
         /// <param name="index">The index</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
         LanguageString&  FindByIndex(UINT index) 
         {
            return Strings.FindByIndex(index);
         }
         
         /// <summary>Inserts a string</summary>
         /// <param name="s">string.</param>
         /// <returns>Zero-based index if successful, otherwise -1</returns>
         int  Insert(LanguageStringRef s)
         {
            return Strings.Insert(s);
         }

         /// <summary>Remove a string by ID</summary>
         /// <param name="id">The string id</param>
         /// <returns>Zero-based index of removed string</returns>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         int  Remove(UINT  id) 
         { 
            return Strings.Remove(id);     
         }

		   // -------------------- REPRESENTATION ---------------------
      public:
         UINT             ID;
         wstring          Title,
                          Description;
         bool             Voiced;
      
         StringCollection Strings;
      };

      /// <summary>Write page to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, LanguagePageRef page);
   }
}

using namespace Logic::Language;
