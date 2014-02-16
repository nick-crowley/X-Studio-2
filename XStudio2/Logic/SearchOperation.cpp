#include "stdafx.h"
#include "SearchOperation.h"
#include "XFileSystem.h"
#include "ScriptFileReader.h"
#include "../ScriptDocument.h"
#include "../DocumentBase.h"

namespace Logic
{
      const CHARRANGE  SearchOperation::NO_MATCH = {-1,-1};
   
      // -------------------------------- CONSTRUCTION --------------------------------

      SearchOperation::SearchOperation() : Complete(true)
      {
      }

      SearchOperation::SearchOperation(const wstring& txt, SearchTarget targ) 
         : Term(txt), Target(targ), LastMatch({0,0}), Complete(false)
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
               if (theApp.GetDocument(Files.front()).FindNext(*this))
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

      /// <summary>Sets the position of the last match.</summary>
      /// <param name="pos">Characater position if found, otherwise -1</param>
      void  SearchOperation::SetMatch(int pos)
      {
         if (pos != -1)
            LastMatch = {pos, pos+Term.length()};
         else
            LastMatch = NO_MATCH;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Builds the list of files to search</summary>
      void  SearchOperation::BuildFileList()
      {
         XFileSystem vfs;

         switch (Target)
         {
         // Document: DEBUG
         case SearchTarget::Document:
            Files.push_back(L"D:\\X3 Albion Prelude\\scripts\\!config.faction.plutarch.pck");
            break;

         // OpenDocuments: enumerate documents
         case SearchTarget::OpenDocuments:
            for (auto& doc : theApp.GetOpenDocuments())
               Files.push_back(doc->GetFullPath());
            break;

         // ScriptFolder: Enumerate scripts
         case SearchTarget::ScriptFolder:
            vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

            for (auto& f : vfs.Browse(XFolder::Scripts))
               Files.push_back(f.FullPath);
            break;
         }

         // Set initial state
         Complete = Files.empty();
      }
   
}

