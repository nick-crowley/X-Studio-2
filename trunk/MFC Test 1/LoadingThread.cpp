// LoadingThread.cpp : implementation file
//

#include "stdafx.h"
#include "LoadingThread.h"
#include "Logic/XFileSystem.h"
#include "Logic/SyntaxLibrary.h"
#include "Logic/StringLibrary.h"
#include "Logic/LegacySyntaxReader.h"
//#include "Logic/LanguageFileReader.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)


   // -------------------------------- CONSTRUCTION --------------------------------
   

   LoadingThread::LoadingThread() 
   {
   }

   LoadingThread::~LoadingThread()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   DWORD WINAPI LoadingThread::ThreadMain(WorkerData* data)
   {
      const WCHAR* syntaxPath = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt";

      XFileSystem vfs;

      try
      {
         // Build VFS. 
         data->SendFeedback(L"Building VFS");
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

         // Enumerate language files
         data->SendFeedback(L"Enumerating language files");
         StringLib.Enumerate(vfs, GameLanguage::English);

         // Load legacy syntax file
         data->SendFeedback(L"Loading legacy syntax file");
         Console << ENDL << Colour::Cyan << L"Reading legacy syntax file: " << syntaxPath << ENDL;

         StreamPtr fs( new FileStream(syntaxPath, FileMode::OpenExisting, FileAccess::Read) );
         SyntaxLib.Merge( LegacySyntaxReader(fs).ReadFile() );

         Console << Colour::Green << L"Legacy syntax loaded successfully" << ENDL;
         return 0;
      }
      catch (ExceptionBase& e)
      {
         Console << e << ENDL;
         return 0;
      }
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END(GUI)



