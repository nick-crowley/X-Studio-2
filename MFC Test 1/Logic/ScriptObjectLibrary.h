#pragma once

#include "Common.h"
#include "WorkerFeedback.h"
#include "LanguageFile.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
   
      /// <summary></summary>
      class ScriptObjectLibrary
      {
		 // ------------------------ TYPES --------------------------
      private:
         /// <summary></summary>
         class ObjectID
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ObjectID(KnownPage p, UINT id) : Page(p), ID(id)
            {}

            // ---------------------- ACCESSORS ------------------------	
         public:
            bool operator==(const ObjectID& r) const {
               return Page == r.Page && ID == r.ID;
            }
            bool operator<(const ObjectID& r) const {
               return Page < r.Page || (Page == r.Page && ID < r.ID);
            }

            // -------------------- REPRESENTATION ---------------------
         public:
            const KnownPage Page;
            const UINT      ID;
         };

         /// <summary></summary>
         class ScriptObject : public LanguageString
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ScriptObject(KnownPage p, const LanguageString& s) : Page(p), LanguageString(s)
            {}

            // -------------------- REPRESENTATION ---------------------
         public:
            const KnownPage Page;
         };

         /// <summary></summary>
         class ObjectCollection : public map<ObjectID, ScriptObject, less<ObjectID>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            ScriptObject  Find(KnownPage page, UINT id)
            {
               iterator it;
               // Lookup and return string
               if ((it = find(ObjectID(page, id))) != end())
                  return it->second;

               // Error: Not found
               throw StringNotFoundException(HERE, (UINT)page, id);
            }

            // ----------------------- MUTATORS ------------------------
         public:
            bool  Add(KnownPage page, const LanguageString& str)
            {
               switch (page)
               {
               // Exclude all
               default:
                  return false;

               // Include all
               case KnownPage::DATA_TYPES:
               case KnownPage::FLIGHT_RETURNS:
               case KnownPage::OBJECT_CLASSES:
               case KnownPage::OBJECT_COMMANDS:
               case KnownPage::PARAMETER_TYPES:
               case KnownPage::WING_COMMANDS:
                  break;

               // Exclude 'old' [THIS] from lookup tree
               case KnownPage::CONSTANTS:
                  if (str.ID == 0)
                     return false;
                  break;

               // Skip 6 digit sector names with IDs 20xxx and 30xxx
               case KnownPage::SECTORS:
                  if (str.ID < 1020000)
                     return false;
                  break;

               // Include names, exclude initials
               case KnownPage::RACES:
                  if (str.ID >= 200)
                     return false;
                  break;

               // Exclude the S,M,L,XL,XXL ship/station name modifiers
               case KnownPage::STATION_SERIALS:
                  if (str.ID >= 500)
                     return false;
                  break;

               // Only include the abbreviated versions
               case KnownPage::TRANSPORT_CLASSES:
                  if (str.ID >= 10)
                     return false;
                  break;

               // FRIEND/FOE/NEUTRAL
               case KnownPage::RELATIONS:
                  switch (str.ID)
                  {
                  case 1102422:
                  case 1102423:
                  case 1102424: 
                     break;
                  default:  
                     return false;
                  }
               }

               // Insert
               return insert(value_type(ObjectID(page,str.ID), ScriptObject(page, str))).second;
            }
         };

         /// <summary></summary>
         class LookupCollection : public map<wstring, ScriptObject, less<wstring>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            // ---------------------- ACCESSORS ------------------------			

            ScriptObject  Find(const wstring& sz)
            {
               iterator it;
               // Lookup and return string
               if ((it = find(sz)) != end())
                  return it->second;

               // Error: Not found
               throw ScriptObjectNotFoundException(HERE, sz);
            }

            // ----------------------- MUTATORS ------------------------
         public:
            bool  Add(KnownPage page, const LanguageString& str)
            {
               return insert(value_type(str.Text, ScriptObject(page, str))).second;
            }
         };
	  
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptObjectLibrary();
         virtual ~ScriptObjectLibrary();
		 
		   DEFAULT_COPY(ScriptObjectLibrary);	// Default copy semantics
		   DEFAULT_MOVE(ScriptObjectLibrary);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);

         protected:
         UINT  GenerateLookup(WorkerData* data);
         UINT  GenerateObjects(WorkerData* data);

         // -------------------- REPRESENTATION ---------------------
      public:
         static ScriptObjectLibrary  Instance;

      private:
         ObjectCollection  Objects;
         LookupCollection  Lookup;
      };
   }

   /// <summary>Access to the script object library singleton</summary>
   #define ScriptObjectLib  ScriptObjectLibrary::Instance
}

using namespace Logic::Scripts;
