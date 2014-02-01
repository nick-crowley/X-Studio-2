#include "stdafx.h"
#include "GameObjectLibrary.h"
#include "XFileSystem.h"
#include "WorkerFeedback.h"
#include "TLaser.h"
#include "TMissile.h"
#include "TShip.h"
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
         // Clear files
         Files.clear();
         Files.resize(1+(UINT)MainType::TechWare);

         // Clear objects
         Objects.clear();
         Lookup.clear();
      }

      /// <summary>Query whether an object is present</summary>
      /// <param name="main">Encoded main type and subtype</param>
      /// <returns></returns>
      bool  GameObjectLibrary::Contains(UINT value) const
      {
         return Objects.Contains((MainType)HIWORD(value), LOWORD(value));
      }
      
      /// <summary>Query whether an object is present</summary>
      /// <param name="name">The object name</param>
      /// <returns></returns>
      bool GameObjectLibrary::Contains(const GuiString& name) const
      {
         return Lookup.Contains(name);
      }

      /// <summary>Query whether an object is present</summary>
      /// <param name="main">The main type</param>
      /// <param name="subtype">The subtype</param>
      /// <returns></returns>
      bool GameObjectLibrary::Contains(MainType main, UINT subtype) const
      {
         // Lookup object
         return Objects.Contains(main, subtype);
      }

      /// <summary>Enumerates available type files</summary>
      /// <param name="vfs">The VFS.</param>
      /// <param name="data">Worker data.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentNullException">Worker data is null</exception>
      /// <exception cref="Logic::NotSupportedException">Unsupported file type identified, but reader not available</exception>
      UINT GameObjectLibrary::Enumerate(const XFileSystem& vfs, WorkerData* data)
      {
         REQUIRED(data);

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
               case MainType::Ship:        reader.reset(new TShipReader(f.OpenRead()));    break;
               case MainType::Missile:     reader.reset(new TMissileReader(f.OpenRead())); break;
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
               default:
                  throw NotSupportedException(HERE, GuiString(L"%s files are not supported", GetString(fn.Type).c_str()));
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
         return Objects.size();
      }

      /// <summary>Finds an object</summary>
      /// <param name="main">The main type</param>
      /// <param name="subtype">The subtype</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Matching type file not loaded</exception>
      /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
      //const TObject*  GameObjectLibrary::Find(MainType main, UINT subtype) const
      //{
      //   // Ensure types loaded
      //   if (Files[(UINT)main] == nullptr)
      //      throw ArgumentException(HERE, L"main", GuiString(L"Object types for maintype %d not loaded", main));

      //   // Lookup object
      //   return Files[(UINT)main]->FindAt(subtype);
      //}

      /// <summary>Finds an object</summary>
      /// <param name="main">Encoded main type and subtype</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Matching type file not loaded</exception>
      /// <exception cref="Logic::GameObjectNotFoundException">Object not found</exception>
      GameObject  GameObjectLibrary::Find(UINT value) const
      {
         return Find((MainType)HIWORD(value), LOWORD(value));
      }

      /// <summary>Finds an object</summary>
      /// <param name="main">The main type</param>
      /// <param name="subtype">The subtype</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Matching type file not loaded</exception>
      /// <exception cref="Logic::GameObjectNotFoundException">Object not found</exception>
      GameObject  GameObjectLibrary::Find(MainType main, UINT subtype) const
      {
         // Ensure types loaded
         if (Files[(UINT)main] == nullptr)
            throw GameObjectNotFoundException(HERE, main);

         // Lookup object
         return Objects.Find(main, subtype);
      }

      /// <summary>Searches the library for objects containing a substring (Case insensitive)</summary>
      /// <param name="search">substring</param>
      /// <returns></returns>
      GameObjectArray  GameObjectLibrary::Query(const GuiString& search) const
      {
         GameObjectArray Results;
         bool  FindAll = search.length() == 0;
         
         // Linear search for partial substring
         for (auto& pair : Lookup)
            if (FindAll || pair.first.Contains(search, false))
               Results.push_back(pair.second);

         return Results;
      }
      
      /// <summary>Finds a game object by name</summary>
      /// <param name="name">object name</param>
      /// <param name="obj">object</param>
      /// <returns>true if found, false otherwise</returns>
      bool  GameObjectLibrary::TryFind(const GuiString& name, const GameObject* &obj) const
      {
         obj = nullptr;
         return Lookup.TryFind(name, obj);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Populates the objects and lookup collection</summary>
      /// <param name="data">Feedback data.</param>
      /// <returns></returns>
      UINT  GameObjectLibrary::PopulateObjects(WorkerData* data)
      {
         typedef list<GameObject>            ObjectList;
         typedef map<GuiString, ObjectList>  ConflictMap;
         
         ConflictMap Conflicts;
         ObjectList Input;

         // Feedback
         data->SendFeedback(Colour::Cyan, ProgressType::Info, 1, L"Generating game objects from type definition files");

         // Extract all objects from files
         for (auto& f : Files)
            if (f != nullptr)
               for (UINT id = 0; id < f->Count; ++id)
                  Input.push_back( GameObject(id, f->FindAt(id)) );

         // Populate lookup
         for (auto& obj : Input)
         {
            // Insert/Ensure unique
            if (!Lookup.Add(obj))
            {
               GuiString key = obj.Name;
               
               // Conflict: Add conflict+object on first occurence, but only conflict on subsequent occurences
               if (Conflicts.find(key) == Conflicts.end())
                  Conflicts[key].push_back(Lookup.Find(key));
               
               Conflicts[key].push_back(obj);
            }
         }

         // Mangle names of conflicting objects
         for (auto& list : Conflicts)
         {
            Lookup.Remove(list.first);

            for (auto& obj : list.second)
               // Append ID, then attempt to insert
               if (!Lookup.Add(obj + obj.ID))
               {  
                  // Feedback: Failed
                  data->SendFeedback(ProgressType::Error, 2, GuiString(L"Unable to add game object '%s'", obj.Name.c_str()) );
                  Console << Colour::Red << L"Unable to add game object: " << Colour::White << obj.Name << ENDL;
               }
         }

         // Populate objects collection from lookup (thereby preserving the name mangling)
         for (auto& pair : Lookup)
            Objects.Add(pair.second);

         // Return object count
         Console << "Generated " << Objects.size() << " game objects" << ENDL;
         return Objects.size();
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

