#include "stdafx.h"
#include "SearchOperation.h"
#include "XFileSystem.h"
#include "ScriptFileReader.h"
#include "../MainWnd.h"
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

      /// <summary>Get search target name</summary>
      wstring  GetString(SearchTarget t)
      {
         switch (t)
         {
         case SearchTarget::Selection:         return L"Selection";
         case SearchTarget::Document:          return L"Document";
         case SearchTarget::OpenDocuments:     return L"OpenDocuments";
         case SearchTarget::ProjectDocuments:  return L"ProjectDocuments";
         case SearchTarget::ScriptFolder:      return L"ScriptFolder";
         }
         return L"Invalid";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      bool  SearchOperation::FindNext()
      {
         // Check for completion
         if (Complete)
            return false;

         // Search thru remaining files for a match
         while (!Files.empty())
         {
            try
            {
               // Already open: Search document
               if (theApp.IsDocumentOpen(Files.front()))
               {
                  // Feedback
                  Console << L"Searching document: " << Colour::Yellow << Files.front() << ENDL;

                  // Search document + highlight match
                  auto& doc = (ScriptDocument&)theApp.GetDocument(Files.front());
                  if (doc.FindNext(*this))
                     return true;
               }
               // File on disc: Open in memory and search
               else
               {
                  // Feedback
                  Console << L"Searching script: " << Colour::Yellow << Files.front() << ENDL;

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
            }
            catch (ExceptionBase& e)
            {
               // Error: Skip file
               auto f = Files.front();
               Files.pop_front();
               // Supply filename
               throw GenericException(HERE, GuiString(L"Unable to search '%s' : %s", f.c_str(), e.Message.c_str()));
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
         // Document: get active script
         case SearchTarget::Document:
            if (auto view = theApp.GetMainWindow()->GetActiveScriptView())
               Files.push_back(view->GetDocument()->GetFullPath());
            break;

         // OpenDocuments: enumerate documents
         case SearchTarget::OpenDocuments:
            for (auto& doc : theApp.GetOpenDocuments())
               Files.push_back(doc->GetFullPath());
            break;

         // ScriptFolder: Enumerate scripts
         case SearchTarget::ScriptFolder:
            vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

            // Use any XML/PCK file
            for (auto& f : vfs.Browse(XFolder::Scripts))
               if (f.FullPath.HasExtension(L".pck") || f.FullPath.HasExtension(L".xml"))
                  Files.push_back(f.FullPath);
            break;
         }

         // Set initial state
         Complete = Files.empty();
      }
   
}

