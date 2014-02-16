#include "stdafx.h"
#include "SearchOperation.h"
#include "XFileSystem.h"
#include "ScriptFileReader.h"
#include "../ScriptDocument.h"
#include "../DocumentBase.h"

namespace Logic
{
   
   
      // -------------------------------- CONSTRUCTION --------------------------------

      SearchOperation::SearchOperation() : Complete(true)
      {
      }

      SearchOperation::SearchOperation(const wstring& txt, SearchTarget targ) 
         : Term(txt), Target(targ), LastMatch({-1,-1}), Complete(false)
      {
         BuildFileList();
      }

      SearchOperation::~SearchOperation()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      bool  SearchOperation::FindNext()
      {
         // Check for completion
         if (Complete)
            return false;

         // Search thru remaining files for a match
         while (!Files.empty())
         {
            // Already open: Search document
            if (theApp.IsDocumentOpen(Files.front()))
            {
               // Search document + highlight match
               if (theApp.GetDocument(Files.front())->FindNext(*this))
                  return true;
            }
            // File on disc: Open in memory and search
            else
            {
               // Read script
               XFileInfo f(Files.front());
               ScriptFile script = ScriptFileReader(f.OpenRead()).ReadFile(f.FullPath, false);
             
               // Search translated text
               if (script.FindNext(*this))
               {
                  auto doc = (ScriptDocument*)theApp.OpenDocumentFile(f.FullPath.c_str());

                  // highlight match
                  doc->SetSelection(LastMatch);
                  return true;
               }
            }

            // No match: search next file
            Files.pop_front();
         }

         // No match: 
         Complete = true;
         return false;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      void  SearchOperation::BuildFileList()
      {
         // Enumerate target files
         XFileSystem vfs;
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);
         for (auto& f : vfs.Browse(XFolder::Scripts))
            Files.push_back(f.FullPath);


         // Set initial state
         Complete = Files.empty();
      }
   
}

