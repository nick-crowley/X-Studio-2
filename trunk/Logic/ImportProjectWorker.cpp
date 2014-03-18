#include "stdafx.h"
#include "ImportProjectWorker.h"
#include "../Logic/FileStream.h"
#include "../Logic/LegacyProjectFileReader.h"
#include "../Logic/ProjectFileWriter.h"
//#include "../Logic/XFileInfo.h"
//#include "../Logic/ScriptFileReader.h"

namespace Logic
{
   namespace Threads
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      ImportProjectWorker::ImportProjectWorker() : BackgroundWorker((ThreadProc)ThreadMain)
      {
      }


      ImportProjectWorker::~ImportProjectWorker()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Upgrades a legacy project</summary>
      /// <param name="data">arguments.</param>
      /// <returns>TRUE if successful, FALSE if failed</returns>
      DWORD WINAPI ImportProjectWorker::ThreadMain(ImportProjectData* data)
      {
         list<Path> created;

         try
         {
            HRESULT  hr;

            // Init COM
            if (FAILED(hr=CoInitialize(NULL)))
               throw ComException(HERE, hr);

            // Feedback
            Console << Cons::UserAction << "Importing legacy project " << data->LegacyPath << " into " << data->UpgradePath << ENDL;
            data->SendFeedback(ProgressType::Operation, 0, VString(L"Importing legacy project '%s'", data->UpgradePath.c_str()) );

            // Read legacy project
            auto in = StreamPtr(new FileStream(data->LegacyPath, FileMode::OpenExisting, FileAccess::Read));
            auto proj = LegacyProjectFileReader(in).ReadFile(data->LegacyPath);

            // Perform initial commits of file items
            for (auto& item : proj.ToList())
               if (item->IsFile() && item->FileType == FileType::Script)
               {
                  // Feedback
                  data->SendFeedback(ProgressType::Info, 1, VString(L"Performing initial commit of '%s'", item->FullPath.c_str()) );

                  // Generate unique filename + Commit
                  item->SetBackupPath(data->UpgradePath.Folder);
                  item->InitialCommit(data->UpgradePath.Folder);

                  // Remember created
                  created.push_back(data->UpgradePath.Folder + item->BackupName);
               }

            // Write X-Studio2 project
            auto out = StreamPtr(new FileStream(data->UpgradePath, FileMode::CreateNew, FileAccess::Write));
            ProjectFileWriter w(out);
            w.Write(proj);
            w.Close();
         
            // Feedback
            data->SendFeedback(Cons::UserAction, ProgressType::Succcess, 0, L"Imported legacy project successfully");
            CoUninitialize();
            return TRUE;
         }
         catch (ExceptionBase& e) 
         {
            // Feedback
            data->SendFeedback(Cons::Error, ProgressType::Failure, 0, GuiString(L"Failed to import project: ") + e.Message);
            Console.Log(HERE, e);

            // Destroy partially imported files
            for (auto& f : created)
               DeleteFile(f.c_str());

            // Cleanup
            CoUninitialize();
            return FALSE;
         }

      }
      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

