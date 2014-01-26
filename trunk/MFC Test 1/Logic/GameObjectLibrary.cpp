#include "stdafx.h"
#include "GameObjectLibrary.h"
#include "XFileSystem.h"
#include "WorkerFeedback.h"
#include "TLaser.h"
#include "TShield.h"
#include "TFactory.h"
#include "TDock.h"
#include "TWare.h"

namespace Logic
{
   namespace Types
   {
      GameObjectLibrary  GameObjectLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      GameObjectLibrary::GameObjectLibrary() : Content(Lookup)
      {
         Clear();
      }


      GameObjectLibrary::~GameObjectLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Clears all loaded objects</summary>
      void GameObjectLibrary::Clear()
      {
         // Clear files
         Files.clear();
         Files.resize(1+(UINT)MainType::TechWare);

         // Clear objects
         Objects.clear();
         Lookup.clear();
      }

      /// <summary>Enumerates available type files</summary>
      /// <param name="vfs">The VFS.</param>
      /// <param name="data">Worker data.</param>
      /// <returns></returns>
      UINT GameObjectLibrary::Enumerate(const XFileSystem& vfs, WorkerData* data)
      {
         // Clear previous contents
         Clear();

         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 1, L"Enumerating type definition files");

         // Enumerate type files
         for (XFileInfo& f : vfs.Browse(XFolder::Types))
         {
            try
            {
               // Identify file type and skip non-TFiles
               TFilenameReader fn(f.FullPath);
               if (!fn.Valid)
                  continue;

               // Feedback
               data->SendFeedback(ProgressType::Info, 2, GuiString(L"Reading type file '%s'...", (const WCHAR*)f.FullPath));
               Console << L"Reading type file: " << f.FullPath << L"...";

               // Create appropriate reader
               TFileReaderPtr reader;
               switch (fn.Type)
               {
               case MainType::Laser:       reader.reset(new TLaserReader(f.OpenRead()));   break;
               case MainType::Shield:      reader.reset(new TShieldReader(f.OpenRead()));  break;
               case MainType::Dock:        reader.reset(new TDockReader(f.OpenRead()));    break;
               case MainType::Factory:     reader.reset(new TFactoryReader(f.OpenRead())); break;
               case MainType::BioWare:     reader.reset(new TWareReader(f.OpenRead()));    break;
               case MainType::FoodWare:    reader.reset(new TWareReader(f.OpenRead()));    break;
               case MainType::EnergyWare:  reader.reset(new TWareReader(f.OpenRead()));    break;
               case MainType::NaturalWare: reader.reset(new TWareReader(f.OpenRead()));    break;
               case MainType::MineralWare: reader.reset(new TWareReader(f.OpenRead()));    break;
               case MainType::TechWare:    reader.reset(new TWareReader(f.OpenRead()));    break;
               }

               // Read/store file directly 
               Files[(UINT)fn.Type] = reader->ReadFile(fn.Type, vfs.GetVersion());

               Console << Colour::Green << L"Success" << ENDL;
            }
            catch (ExceptionBase& e)
            {
               data->SendFeedback(Colour::Red, ProgressType::Warning, 3, GuiString(L"Failed: ") + e.Message);
            }
         }

         // Populate object collection
         PopulateObjects(data);

         // Return object count
         return Content.size();
      }

      /// <summary>Finds an object</summary>
      /// <param name="main">The main type</param>
      /// <param name="subtype">The subtype</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Matching type file not loaded</exception>
      /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
      TObject*  GameObjectLibrary::Find(MainType main, UINT subtype)
      {
         // Ensure types loaded
         if (Files[(UINT)main] == nullptr)
            throw ArgumentException(HERE, L"main", GuiString(L"Object types for maintype %d not loaded", main));

         // Lookup object
         return Files[(UINT)main]->FindAt(subtype);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      UINT  GameObjectLibrary::PopulateObjects(WorkerData* data)
      {
         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 1, L"Generating game objects from type definition files");

         // Populate objects 
         for (auto& f : Files)
            if (f != nullptr)
               for (UINT id = 0; id < f->Count; ++id)
                  Objects.Add( GameObject(id, f->FindAt(id)) );

         // Populate lookup
         for (auto& o : Objects)
         {
            const GameObject& obj = o.second;
            if (!Lookup.Add(obj))
               Console << Colour::Red << L"Unable to add game object: " << Colour::White << obj.Text << ENDL;
         }

         // Return object count
         Console << "Generated " << Content.size() << " game objects" << ENDL;
         return Content.size();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

