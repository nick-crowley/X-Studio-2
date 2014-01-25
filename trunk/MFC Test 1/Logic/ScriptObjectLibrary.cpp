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

      void  ScriptObjectLibrary::Clear()
      {
         Objects.clear();
         Lookup.clear();
      }

      UINT  ScriptObjectLibrary::Enumerate(WorkerData* data)
      {
         // Ensure string library exists
         if (StringLib.Files.empty())
            throw InvalidOperationException(HERE, L"String library has not been enumerated");

         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 2, L"Generating script objects from language files");

         // Clear previous
         Clear();

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
         Console << L"Discovered " << (int)Objects.size() << " script objects..." << ENDL;

         list<ScriptObject> Conflicts;

         // Generate reverse lookup collection
         for (auto& pair : Objects)  
         {
            const ObjectID& id = pair.first;
            const ScriptObject& obj = pair.second;

            // Insert string 
            if (!Lookup.Add(id.Page, obj))
            {
               // Identify conflict
               auto& conf = Lookup.Find(obj.Text);
                  
               // Feedback
               GuiString err(L"Unable to insert '%s' {%d:%d} due to conflict with '%s' {%d:%d}", obj.Text.c_str(), obj.Page, obj.ID, conf.Text.c_str(), conf.Page, conf.ID);
               data->SendFeedback(Colour::Red, ProgressType::Error, 3, err);
            }
         }

         // Feedback number of conflicts
         if (Objects.size() - Lookup.size() > 0)
            data->SendFeedback(ProgressType::Error, 3, GuiString(L"Unable to process %d script objects", Objects.size() - Lookup.size()));
         
         return Lookup.size();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

