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
      bool  ScriptObjectLibrary::Contains(const wstring& obj) const
      {
         return Lookup.Contains(obj);
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
      /// <exception cref="Logic::InvalidOperationException">String library is empty</exception>
      UINT  ScriptObjectLibrary::Enumerate(WorkerData* data)
      {
         // Ensure string library exists
         if (StringLib.Files.empty())
            throw InvalidOperationException(HERE, L"String library has not been enumerated");

         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 1, L"Generating script objects from language files");

         // Clear previous
         Clear();

         // Build objects
         PopulateObjects(data);
         PopulateLookup(data);
         
         // Feedback number of conflicts
         if (Objects.size() - Lookup.size() > 0)
            data->SendFeedback(Colour::Red, ProgressType::Error, 2, GuiString(L"Unable to process %d script objects", Objects.size() - Lookup.size()));
         
         return Lookup.size();
      }

      /// <summary>Finds an object by text<summary>
      /// <param name="sz">The text</param>
      /// <returns>Object</returns>
      /// <exception cref="Logic::ScriptObjectNotFoundException">Object not found</exception>
      ScriptObject  ScriptObjectLibrary::Find(const wstring& sz) const
      {
         return Lookup.Find(sz);
      }

      /// <summary>Finds a script object by ID</summary>
      /// <param name="page">The page.</param>
      /// <param name="id">The ID.</param>
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
      ScriptObjectArray  ScriptObjectLibrary::Query(const wstring& str) const
      {
         ScriptObjectArray Results;

         // Linear search for partial substring
         for (auto& pair : Lookup)
            if (str.length() == 0 || pair.first.find(str.c_str()) != wstring::npos)
               Results.push_back(pair.second);

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


      /// <summary>Generates the reverse lookup collection from the object collection</summary>
      /// <param name="data">Feedback data</param>
      /// <returns>Number of lookups generated</returns>
      UINT  ScriptObjectLibrary::PopulateLookup(WorkerData* data)
      {
         // Generate reverse lookup collection
         for (auto& pair : Objects)  
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

         // Return count
         return Lookup.size();   
      }

      
      /// <summary>Extracts strings used as script objects from the string library</summary>
      /// <param name="data">Feedback data</param>
      /// <returns>Number of strings extracted</returns>
      UINT  ScriptObjectLibrary::PopulateObjects(WorkerData* data)
      {
         // Iterate thru all strings in library
         for (auto& file : StringLib.Files)       
            for (auto& page : file)
               for (auto& str : page)
                  // Insert subset of strings from known pages
                  if (str.IsScriptObject())
                     Objects.Add(ScriptObject(str.ID, (KnownPage)page.ID, StringLib.Resolve(page.ID, str.ID), str.Version));
         
         // Feedback
         Console << "Discovered " << (int)Objects.size() << " script objects..." << ENDL;
         return Objects.size();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

