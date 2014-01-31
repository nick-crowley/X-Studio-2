#include "stdafx.h"
#include "ScriptObjectLibrary.h"


namespace Logic
{
   namespace Scripts
   {
      ScriptObjectLibrary  ScriptObjectLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptObjectLibrary::ScriptObjectLibrary()
      {
      }


      ScriptObjectLibrary::~ScriptObjectLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      
      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Get start iterator.</summary>
      /// <returns></returns>
      ScriptObjectLibrary::const_iterator  ScriptObjectLibrary::begin() const
      {
         return const_iterator(Lookup, Lookup.begin());
      }

      /// <summary>Query whether an object is present</summary>
      /// <param name="obj">The object text</param>
      /// <returns></returns>
      bool  ScriptObjectLibrary::Contains(const GuiString& obj) const
      {
         return Lookup.Contains(obj);
      }

      /// <summary>Queries whether a script object is present</summary>
      /// <param name="grp">object group</param>
      /// <param name="id">object id</param>
      /// <returns></returns>
      bool  ScriptObjectLibrary::Contains(ScriptObjectGroup grp, UINT id) const
      {
         return Objects.Contains(grp, id);
      }

      /// <summary>Clears all objects from the library.</summary>
      void  ScriptObjectLibrary::Clear()
      {
         Objects.clear();
         Lookup.clear();
      }

      /// <summary>Get finish iterator.</summary>
      /// <returns></returns>
      ScriptObjectLibrary::const_iterator  ScriptObjectLibrary::end() const
      {
         return const_iterator(Lookup, Lookup.end());
      }

      /// <summary>Populates the library from the string library</summary>
      /// <param name="data">Feedback data</param>
      /// <returns>Number of objects found</returns>
      /// <exception cref="Logic::ArgumentNullException">Worker data is null</exception>
      /// <exception cref="Logic::InvalidOperationException">String library is empty</exception>
      UINT  ScriptObjectLibrary::Enumerate(WorkerData* data)
      {
         REQUIRED(data);

         // Ensure string library exists
         if (StringLib.Files.empty())
            throw InvalidOperationException(HERE, L"String library has not been enumerated");

         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 1, L"Generating script objects from language files");

         // Populate
         Clear();
         Populate(data);

         // DEBUG:
         Console << "Discovered " << (int)Objects.size() << " script objects..." << ENDL;
         
         // Feedback number of conflicts
         if (Objects.size() - Lookup.size() > 1)   // Always 1 less in lookup because old [THIS] intentionally removed
            data->SendFeedback(Colour::Red, ProgressType::Error, 2, GuiString(L"Unable to process %d script objects", Objects.size()-Lookup.size()-1));
         
         return Lookup.size();
      }

      /// <summary>Finds an object by text<summary>
      /// <param name="sz">The text</param>
      /// <returns>Object</returns>
      /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
      ScriptObject  ScriptObjectLibrary::Find(const GuiString& sz) const
      {
         return Lookup.Find(sz);
      }

      /// <summary>Finds a script object by ID</summary>
      /// <param name="group">object group</param>
      /// <param name="id">object id</param>
      /// <returns></returns>
      /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
      ScriptObject  ScriptObjectLibrary::Find(ScriptObjectGroup grp, UINT id) const
      {
         return Objects.Find(grp, id);
      }

      /// <summary>Get number of objects in the library</summary>
      /// <returns></returns>
      UINT  ScriptObjectLibrary::GetCount() const
      {
         return Lookup.size();
      }
      
      /// <summary>Find all script objects containing a substring</summary>
      /// <param name="str">The substring.</param>
      /// <returns></returns>
      ScriptObjectArray  ScriptObjectLibrary::Query(const GuiString& str) const
      {
         ScriptObjectArray Results;
         bool FindAll = (str.length() == 0);

         // Linear search for partial substring
         for (const auto& obj : *this)
            if (!obj.IsHidden() && (FindAll || obj.Text.Contains(str, false)))
               Results.push_back(obj);

         return Results;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Attempts to insert two conflicting object.</summary>
      /// <param name="a">first</param>
      /// <param name="b">second</param>
      /// <returns>true if inserted, false if still conflicting  (Neither are inserted if false)</returns>
      bool  ScriptObjectLibrary::InsertConflicts(ScriptObject a, ScriptObject b)
      {
         // Check whether objects are still conflicting
         if (a.Text == b.Text || Lookup.Contains(a.Text) || Lookup.Contains(b.Text))
            return false;

         // DEBUG: 
         Console << Colour::Green << L"Resolved: " << Colour::Yellow << a.Text.c_str() << Colour::White << L" and " << Colour::Yellow << b.Text.c_str() << ENDL;

         // Insert unique
         Lookup.Add(a);
         Lookup.Add(b);
         return true;
      }

      /// <summary>Performs name mangling on two conflicting objects.</summary>
      /// <param name="a">first</param>
      /// <param name="b">second</param>
      /// <returns>true if mangled and inserted successfully, false if unable to resolve  (Neither are inserted if false)</returns>
      bool  ScriptObjectLibrary::MangleConflicts(ScriptObject a, ScriptObject b)
      {
         // different groups: append group acronym 
         if (a.Group != b.Group || a.Group == ScriptObjectGroup::Sector)   // special case: append sector co-ordinates
            return InsertConflicts(a+a.Group, b+b.Group);

         // different games: append game acronym
         else if (a.Version != b.Version) 
            return InsertConflicts(a+a.Version, b+b.Version);

         // Same group, same game: append ID
         return InsertConflicts(a+a.ID, b+b.ID);
      }

      /// <summary>Generates the ID and reverse lookup collections from the string library</summary>
      /// <param name="data">Feedback data</param>
      /// <returns>Number of objects generated</returns>
      UINT  ScriptObjectLibrary::Populate(WorkerData* data)
      {
         ObjectCollection unmangled(GetSpecialCases());

         // Extract all script objects from library  [this loop correctly enforces string precedence]
         for (auto& file : StringLib.Files)       
            for (auto& page : file)
               for (auto& str : page)
                  if (str.IsScriptObject())
                     unmangled.Add( ScriptObject(str.ID, (KnownPage)page.ID, str.ResolvedText, str.Version) );
         

         // Generate reverse lookup collection
         for (const auto& pair : unmangled) 
         {
            const ScriptObject& obj = pair.second;

            // Attempt to insert
            if (!Lookup.Add(obj))
            {
               // DEBUG: 
               auto& conf = Lookup.Find(obj.Text);
               Console << L"Conflict " << Colour::Yellow << obj.Text << Colour::White << " : " << Colour::Yellow << GuiString(L"{%s:%d}",GetString(obj.Group).c_str(),obj.ID) << Colour::White << L" and " << Colour::Yellow << GuiString(L"{%s:%d}",GetString(conf.Group).c_str(),conf.ID) << Colour::White << "...";

               // Extract conflict
               ScriptObject conflict = Lookup.Find(obj.Text);
               Lookup.Remove(obj.Text);

               // Mangle them
               if (!MangleConflicts(obj, conflict))
               {
                  // Failed: Feedback
                  GuiString err(L"Conflicting script objects '%s' detected: {%s:%d} and {%s:%d}", obj.Text.c_str(), GetString(obj.Group).c_str(), obj.ID, GetString(conflict.Group).c_str(), conflict.ID);
                  data->SendFeedback(ProgressType::Error, 2, err);

                  // DEBUG:
                  Console << Colour::Red << L"Unable to resolve" << ENDL;
               }
            }
         }

         // Populate ID collection from lookup to preserve name mangling
         for (const ScriptObject& obj : *this)
            Objects.Add(obj);

         // SpecialCase: Add old [THIS] to ID collection so older scripts can be parsed
         Objects.Add(ScriptObject(0, KnownPage::CONSTANTS, L"THIS", GameVersion::Threat));

         // Return count
         return Objects.size();   
      }

      
      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Gets collection of special cases</summary>
      /// <returns></returns>
      ScriptObjectLibrary::ObjectCollection  ScriptObjectLibrary::GetSpecialCases() const
      {
         ObjectCollection coll;

         // Add 'Player' race
         coll.Add(ScriptObject(10, KnownPage::RACES, L"Player", GameVersion::Threat));
         
         // Distinguish NPC/Player HQ object class
         coll.Add(ScriptObject(2045, KnownPage::OBJECT_CLASSES, L"Player Headquarters", GameVersion::Reunion));

         // Distinguish TS/PirateTS object class
         coll.Add(ScriptObject(2035, KnownPage::OBJECT_CLASSES, L"Pirate TS", GameVersion::TerranConflict));

         // Fix ambiguous parameter types
         coll.Add(ScriptObject((UINT)ParameterType::INTERRUPT_RETURN_VALUE_IF, KnownPage::PARAMETER_TYPES, L"@RetVar/If", GameVersion::Threat));
         coll.Add(ScriptObject((UINT)ParameterType::LABEL_NAME, KnownPage::PARAMETER_TYPES, L"Label Name", GameVersion::Threat));
         coll.Add(ScriptObject((UINT)ParameterType::LABEL_NUMBER, KnownPage::PARAMETER_TYPES, L"Label Number", GameVersion::Threat));

         // Add missing subtypes
         coll.Add(ScriptObject(228, KnownPage::CONSTANTS, L"SSTYPE_DEBRIS", GameVersion::Threat));
         coll.Add(ScriptObject(229, KnownPage::CONSTANTS, L"SSTYPE_WRECK_SHIP", GameVersion::Threat));
         coll.Add(ScriptObject(230, KnownPage::CONSTANTS, L"SSTYPE_WRECK_DOCK", GameVersion::Threat));
         coll.Add(ScriptObject(231, KnownPage::CONSTANTS, L"SSTYPE_WRECK_FACTORY", GameVersion::Threat));

         // Distinguish Tiny ware from '-'
         //coll.Add(ScriptObject(0, KnownPage::TRANSPORT_CLASSES, L"T", GameVersion::Threat));
         return coll;
      }

   }
}

