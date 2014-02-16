#include "stdafx.h"
#include "SearchWorker.h"
#include "XFileInfo.h"
#include "ScriptFileReader.h"
#include "../ScriptDocument.h"

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
      
      DWORD WINAPI  SearchWorker::ThreadMain(SearchWorkerData* data)
      {
         SearchData& search = data->Search;

         // Check for completion
         if (search.Complete)
            return false;

         try
         {
            HRESULT  hr;

            // Init COM
            if (FAILED(hr=CoInitialize(NULL)))
               throw ComException(HERE, hr);

            // Search thru remaining files for a match
            while (!search.Files.empty())
            {
               try
               {
                  // Already open: Search document
                  if (theApp.IsDocumentOpen(search.Files.front()))
                  {
                     // Feedback
                     Console << L"Searching document: " << Colour::Yellow << search.Files.front() << ENDL;

                     // Search document + highlight match
                     auto& doc = (ScriptDocument&)theApp.GetDocument(search.Files.front());
                     if (doc.FindNext(search))
                        return true;
                  }
                  // File on disc: Open in memory and search
                  else
                  {
                     // Feedback
                     Console << L"Searching script: " << Colour::Yellow << search.Files.front() << ENDL;

                     // Read script
                     XFileInfo f(search.Files.front());
                     ScriptFile script = ScriptFileReader(f.OpenRead()).ReadFile(f.FullPath, false);
             
                     // Search translated text
                     if (script.FindNext(search))
                     {
                        auto doc = (ScriptDocument*)theApp.OpenDocumentFile(f.FullPath.c_str());

                        // Perform search again (due to indentation causing different character indicies)
                        search.LastMatch = {0,0};
                        doc->FindNext(search);
                        return true;
                     }
                  }
               }
               catch (ExceptionBase& e)
               {
                  Console.Log(HERE, e, GuiString(L"Unable to search '%s' : %s", search.Files.front().c_str()) );
                  // Error: Skip file
                  //auto f = search.Files.front();
                  //search.Files.pop_front();
                  //search.LastMatch = {0,0};
                  //// Supply filename
                  //throw GenericException(HERE, GuiString(L"Unable to search '%s' : %s", f.c_str(), e.Message.c_str()));
               }

               // No match: search next file
               search.Files.pop_front();
               search.LastMatch = {0,0};
            }
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);
         }

         // Complete: No more matches
         search.Complete = true;
         CoUninitialize();
         return false;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

