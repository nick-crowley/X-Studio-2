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
      const WCHAR* syntaxPath = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt";

      try
      {
         // Init COM
         HRESULT  hr;
         if (FAILED(hr=CoInitialize(NULL)))
            throw ComException(HERE, hr);

         // Feedback
         data->SendFeedback(ProgressType::Info, L"Building VFS");

         // Build VFS. 
         XFileSystem vfs;
         vfs.Enumerate(data->GameFolder, data->Version);

         // Feedback
         data->SendFeedback(ProgressType::Info, L"Enumerating language files");

         // Enumerate language files
         StringLib.Enumerate(vfs, GameLanguage::English);

         // Feedback
         data->SendFeedback(ProgressType::Info, L"Loading legacy syntax file");
         
         // Load legacy syntax file
         Console << ENDL << Colour::Cyan << L"Reading legacy syntax file: " << syntaxPath << ENDL;
         StreamPtr fs( new FileStream(syntaxPath, FileMode::OpenExisting, FileAccess::Read) );
         SyntaxLib.Merge( LegacySyntaxReader(fs).ReadFile() );
         Console << Colour::Green << L"Legacy syntax loaded successfully" << ENDL;

         // Feedback
         data->SendFeedback(ProgressType::Succcess, L"Loaded game data successfully");
         
         // Cleanup
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



