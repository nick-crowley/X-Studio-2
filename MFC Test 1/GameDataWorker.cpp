// GameDataWorker.cpp : implementation file
//

#include "stdafx.h"
#include "GameDataWorker.h"
#include "Logic/XFileSystem.h"
#include "Logic/SyntaxLibrary.h"
#include "Logic/StringLibrary.h"
#include "Logic/LegacySyntaxReader.h"
//#include "Logic/LanguageFileReader.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Threads)


   // -------------------------------- CONSTRUCTION --------------------------------
   

   GameDataWorker::GameDataWorker() : BackgroundWorker((ThreadProc)ThreadMain)
   {
   }

   GameDataWorker::~GameDataWorker()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   DWORD WINAPI GameDataWorker::ThreadMain(GameDataWorkerData* data)
   {
      try
      {
         XFileSystem vfs;
         HRESULT  hr;

         // Init COM
         if (FAILED(hr=CoInitialize(NULL)))
            throw ComException(HERE, hr);

         // Build VFS. 
         data->SendFeedback(ProgressType::Info, L"Building VFS");
         vfs.Enumerate(data->GameFolder, data->Version);

         // Enumerate language files
         data->SendFeedback(ProgressType::Info, L"Enumerating language files");
         StringLib.Enumerate(vfs, GameLanguage::English);

         // Load legacy syntax file
         data->SendFeedback(ProgressType::Info, L"Loading legacy syntax file");
         SyntaxLib.Enumerate();

         // Cleanup
         data->SendFeedback(ProgressType::Succcess, L"Loaded game data successfully");
         CoUninitialize();
         return 0;
      }
      catch (ExceptionBase& e)
      {
         // Feedback
         data->SendFeedback(ProgressType::Error, GuiString(L"Unable to load game data : ") + e.Message);
         Console << e << ENDL;

         // Cleanup
         CoUninitialize();
         return 0;
      }
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Threads)



