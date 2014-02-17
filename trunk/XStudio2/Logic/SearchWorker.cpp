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
         case SearchTarget::ProjectDocuments:  return L"Project Files";
         case SearchTarget::ScriptFolder:      return L"Script Folder";
         }
         return L"Invalid";
      }

      /// <summary>Builds the list of files to search</summary>
      void  SearchWorker::BuildFileList(SearchWorkerData* data)
      {
         XFileSystem vfs;

         switch (data->GetTarget())
         {
         // Document: get active script
         case SearchTarget::Document:
            if (auto view = theApp.GetMainWindow()->GetActiveScriptView())
               data->AddFile(view->GetDocument()->GetFullPath());
            break;

         // OpenDocuments: enumerate documents
         case SearchTarget::OpenDocuments:
            for (auto& doc : theApp.GetOpenDocuments())
               data->AddFile(doc->GetFullPath());
            break;

         // ScriptFolder: Enumerate scripts
         case SearchTarget::ScriptFolder:
            vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

            // Use any XML/PCK file
            for (auto& f : vfs.Browse(XFolder::Scripts))
               if (f.FullPath.HasExtension(L".pck") || f.FullPath.HasExtension(L".xml"))
                  data->AddFile(f.FullPath);
            break;
         }
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
            if (!data->HasFiles() && !data->IsComplete())
               BuildFileList(data);

            // Search thru remaining files for a match
            while (data->HasFiles())
            {
               try
               {
                  // Already open: Search document
                  if (theApp.IsDocumentOpen(data->CurrentFile))
                  {
                     // Feedback
                     Console << L"Searching document: " << Colour::Yellow << data->CurrentFile << ENDL;

                     // Search document + highlight match
                     auto& doc = (ScriptDocument&)theApp.GetDocument(data->CurrentFile);
                     if (doc.FindNext(data->GetSearchData()))
                        return 0;
                  }
                  // File on disc: Open in memory and search
                  else
                  {
                     // Feedback
                     Console << L"Searching script: " << Colour::Yellow << data->CurrentFile << ENDL;

                     // Read script
                     XFileInfo f(data->CurrentFile);
                     ScriptFile script = ScriptFileReader(f.OpenRead()).ReadFile(f.FullPath, false);
             
                     // Search translated text
                     if (script.FindNext(data->GetSearchData()))
                     {
                        auto doc = (ScriptDocument*)theApp.OpenDocumentFile(f.FullPath.c_str());

                        // Perform search again (due to indentation causing different character indicies)
                        data->ResetLastMatch();
                        doc->FindNext(data->GetSearchData());
                        return 0;
                     }
                  }
               }
               catch (ExceptionBase& e)
               {
                  // Error: Feedback
                  GuiString msg(L"Cannot read '%s' : %s", data->CurrentFile.c_str(), e.Message.c_str());
                  data->SendFeedback(ProgressType::Error, 1, msg);
                  Console.Log(HERE, e, msg);
               }

               // No match: search next file
               data->NextFile();
               data->ResetLastMatch();
            }
         }
         catch (ExceptionBase& e) {
            // Feedback
            GuiString msg(L"Unable to perform search: %s", e.Message.c_str());
            data->SendFeedback(ProgressType::Error, 1, msg);
            Console.Log(HERE, e, msg);
         }

         // Complete: No more matches
         data->SetComplete();
         CoUninitialize();
         return 0;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

