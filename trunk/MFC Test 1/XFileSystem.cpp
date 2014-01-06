#include "stdafx.h"
#include "XFileSystem.h"


namespace Library
{
   namespace FileSystem
   {

      XFileSystem::XFileSystem(Path folder)
      {
         if (!folder.Exists())
            throw ArgumentException(HERE, L"folder", L"path does not exist");
      }


      XFileSystem::~XFileSystem()
      {
      }

   }
}
