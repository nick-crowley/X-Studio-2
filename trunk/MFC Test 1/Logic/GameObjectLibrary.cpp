#include "stdafx.h"
#include "GameObjectLibrary.h"
#include "XFileSystem.h"
#include "WorkerFeedback.h"

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

