#include "stdafx.h"
#include "SearchWorker.h"
#include "XFileSystem.h"
#include "XFileInfo.h"
#include "ScriptFileReader.h"

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
      LogicExport wstring  GetString(SearchTarget t)
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

      /// <summary>Finds either the next matching file, or all matches in all files.</summary>
      /// <param name="data">The data.</param>
      /// <returns></returns>
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
               Path CurrentFile = data->Files.front();
               data->Files.pop_front();

               try
               {
                  // Feedback
                  Console << L"Searching script: " << CurrentFile << ENDL;

                  // Read script
                  XFileInfo f(CurrentFile);
                  ScriptFile script = ScriptFileReader(f.OpenRead()).ReadFile(CurrentFile, false);
             
                  // Search contents
                  data->Match.SetPath(CurrentFile);
                  switch (data->Command)
                  {
                  // Find/Replace: Return match for display
                  case SearchCommand::Find:
                  case SearchCommand::Replace:
                     // Search from beginning of file
                     if (script.FindNext(0, data->Match))
                     {
                        data->Match.ClearLocation();   // Clear location because document co-ordinates are different
                        return 0;
                     }
                     break;

                  // FindAll/ReplaceAll: Feedback [+Replace]
                  case SearchCommand::FindAll:
                  case SearchCommand::ReplaceAll:
                     // Search thru all matches
                     for (UINT start = 0; script.FindNext(start, data->Match); start = data->Match.End)
                     {
                        // Replace match
                        if (data->Command == SearchCommand::ReplaceAll)
                           script.Replace(data->Match);

                        // Feedback
                        data->FeedbackMatch();
                     }
                     break;
                  }
               }
               catch (ExceptionBase& e)
               {
                  // Error: Feedback
                  VString msg(L"Cannot read '%s' : %s", CurrentFile.c_str(), e.Message.c_str());
                  data->FeedbackError(msg);
                  Console.Log(HERE, e, msg);
               }
            }
         }
         catch (ExceptionBase& e) {
            // Feedback
            VString msg(L"Unable to perform search: %s", e.Message.c_str());
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

