#include "stdafx.h"
#include "SearchWorker.h"
#include "XFileSystem.h"
#include "XFileInfo.h"
#include "ScriptFileReader.h"
#include "../ScriptDocument.h"
#include "../MainWnd.h"

namespace Logic
{
   namespace Threads
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      SearchWorker::SearchWorker() : BackgroundWorker((ThreadProc)ThreadMain)
      {
      }


      SearchWorker::~SearchWorker()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Get search target name</summary>
      wstring  GetString(SearchTarget t)
      {
         switch (t)
         {
         case SearchTarget::Selection:         return L"Selection";
         case SearchTarget::Document:          return L"Current Document";
         case SearchTarget::OpenDocuments:     return L"All Open Documents";
         case SearchTarget::ProjectFiles:      return L"Project Files";
         case SearchTarget::ScriptFolder:      return L"Script Folder";
         }
         return L"Invalid";
      }

      /// <summary>Builds the list of files to search</summary>
      void  SearchWorker::BuildFileList(SearchWorkerData* data)
      {
         XFileSystem vfs;

         switch (data->Target)
         {
         // Document Based: Invalid
         case SearchTarget::Selection:
         case SearchTarget::Document:
         case SearchTarget::OpenDocuments:
            throw InvalidOperationException(HERE, L"");

         // ScriptFolder: Enumerate scripts
         case SearchTarget::ScriptFolder:
            vfs.Enumerate(data->Folder, data->Version);

            // Use any XML/PCK file
            for (auto& f : vfs.Browse(XFolder::Scripts))
               if (f.FullPath.HasExtension(L".pck") || f.FullPath.HasExtension(L".xml"))
                  data->Files.push_back( f.FullPath );
            break;
         }
         
         // Mark as initialised
         data->Initialized = true;
      }

      DWORD WINAPI  SearchWorker::ThreadMain(SearchWorkerData* data)
      {
         try
         {
            HRESULT  hr;

            // Init COM
            if (FAILED(hr=CoInitialize(NULL)))
               throw ComException(HERE, hr);

            // FirstCall: Assemble list of files to search
            if (!data->Initialized)
            {
               data->Match.Clear();
               BuildFileList(data);
            }

            // Feedback
            Console << Cons::Heading << "Searching remaining " << data->Files.size() << " files..." << ENDL;

            // Search thru remaining files for a match
            while (!data->Files.empty() && !data->IsAborted())
            {
               // Get next file
               IO::Path CurrentFile = data->Files.front();
               data->Files.pop_front();

               try
               {
                  // Feedback
                  Console << L"Searching script: " << CurrentFile << ENDL;

                  // Read script
                  XFileInfo f(CurrentFile);
                  ScriptFile script = ScriptFileReader(f.OpenRead()).ReadFile(CurrentFile, false);
             
                  // Search contents
                  data->Match.FullPath = CurrentFile;
                  while (script.FindNext(data->Match))
                  {
                     // Find/Replace: Return match for display
                     if (data->Operation == Operation::Find || data->Operation == Operation::Replace)
                     {
                        data->Match.Location = {0,0};    // Clear location because document co-ordinates are different
                        return 0;
                     }
                     // FindAll: Feedback
                     else if (data->Operation == Operation::FindAll)
                        data->FeedbackMatch();

                     // ReplaceAll: Replace + Feedback
                     else if (data->Operation == Operation::ReplaceAll)
                     {
                        script.Replace(data->Match);
                        data->FeedbackMatch();
                     }
                  }
               }
               catch (ExceptionBase& e)
               {
                  // Error: Feedback
                  GuiString msg(L"Cannot read '%s' : %s", CurrentFile.c_str(), e.Message.c_str());
                  data->FeedbackError(msg);
                  Console.Log(HERE, e, msg);
               }
            }
         }
         catch (ExceptionBase& e) {
            // Feedback
            GuiString msg(L"Unable to perform search: %s", e.Message.c_str());
            data->FeedbackError(msg);
            Console.Log(HERE, e, msg);
         }

         // Complete: No more matches
         data->Match.Clear();
         CoUninitialize();
         return 0;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

