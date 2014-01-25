#include "stdafx.h"
#include "ScriptObjectLibrary.h"


namespace Logic
{
   namespace Scripts
   {
      ScriptObjectLibrary  ScriptObjectLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptObjectLibrary::ScriptObjectLibrary() : Content(Lookup)
      {
      }


      ScriptObjectLibrary::~ScriptObjectLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      ScriptObject&  ScriptObjectLibrary::FindAt(UINT index) 
      {
         // Validate index
         if (index >= Count)
            throw IndexOutOfRangeException(HERE, index, Count);

         // Lookup item
         auto it = Lookup.begin();
         for (UINT i = 0; i < index; ++i)  
            ++it;
         return it->second;
      }

      UINT  ScriptObjectLibrary::FindMatch(const wstring& str) const
      {
         // Linear search for partial substring
         int index = 0;
         for (auto& it : Lookup)
            if (it.second.Text.find(str) != wstring::npos ? true : (++index,false))
               return index;

         return -1;
      }

      UINT  ScriptObjectLibrary::GetCount() const
      {
         return Lookup.size();
      }

      /// <summary>Clears all objects from the library.</summary>
      void  ScriptObjectLibrary::Clear()
      {
         Objects.clear();
         Lookup.clear();
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
         Console << Colour::Green << GuiString(L"Mangle success '%s' {%d:%d} and '%s' {%d:%d}", 
                                               a.Text.c_str(), a.Page, a.ID, b.Text.c_str(), b.Page, b.ID) << ENDL;

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
         // different pages: append category acronym
         if (a.Page != b.Page)
            return InsertConflicts(a+a.Page, b+b.Page);

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
            const ObjectID& id = pair.first;
            const ScriptObject& obj = pair.second;

            // Attempt to insert by text
            if (!Lookup.Add(id.Page, obj))
            {
               // DEBUG: 
               auto& conf = Lookup.Find(obj.Text);
               Console << GuiString(L"Conflict detected '%s' between {%d:%d} and {%d:%d}", 
                                    obj.Text.c_str(), obj.Page, obj.ID, conf.Page, conf.ID) << ENDL;

               // Extract conflict
               ScriptObject conflict = Lookup.Find(obj.Text);
               Lookup.Remove(obj.Text);

               // Mangle them
               if (!MangleConflicts(obj, conflict))
               {
                  GuiString err(L"Unable to mangle '%s' {%d:%d} due to conflict with '%s' {%d:%d}", 
                                obj.Text.c_str(), obj.Page, obj.ID, conflict.Text.c_str(), conflict.Page, conflict.ID);
                  data->SendFeedback(Colour::Red, ProgressType::Error, 2, err);
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
         for (auto& f : StringLib.Files)       
            for (auto& p : f.Pages)            
            {  
               const LanguagePage& page = p.second;
               for (auto& s : page.Strings)    
               {
                  // Insert subset of strings from known pages
                  const LanguageString& str = s.second;
                  switch (page.ID)
                  {
                  case KnownPage::CONSTANTS:
                  case KnownPage::DATA_TYPES:
                  case KnownPage::FLIGHT_RETURNS:
                  case KnownPage::OBJECT_CLASSES:
                  case KnownPage::OBJECT_COMMANDS:
                  case KnownPage::PARAMETER_TYPES:
                  case KnownPage::SECTORS:
                  case KnownPage::STATION_SERIALS:
                  case KnownPage::TRANSPORT_CLASSES:
                  case KnownPage::WING_COMMANDS:
                     Objects.Add((KnownPage)page.ID, str);
                     break;
                  }
               }
            }
         
         // Feedback
         Console << "Discovered " << (int)Objects.size() << " script objects..." << ENDL;
         return Objects.size();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

