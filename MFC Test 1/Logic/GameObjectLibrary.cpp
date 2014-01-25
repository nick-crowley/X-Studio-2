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
   namespace Scripts
   {
      GameObjectLibrary  GameObjectLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      GameObjectLibrary::GameObjectLibrary()
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
         Files.clear();
         Files.reserve(1+(UINT)MainType::TechWare);
      }

      /// <summary>Enumerates available type files</summary>
      /// <param name="vfs">The VFS.</param>
      /// <param name="data">Worker data.</param>
      /// <returns></returns>
      UINT GameObjectLibrary::Enumerate(const XFileSystem& vfs, WorkerData* data)
      {
         UINT count=0;

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
               count++;

               Console << Colour::Green << L"Success" << ENDL;
            }
            catch (ExceptionBase& e)
            {
               data->SendFeedback(Colour::Red, ProgressType::Warning, 3, GuiString(L"Failed: ") + e.Message);
            }
         }

         return count;
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


      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

