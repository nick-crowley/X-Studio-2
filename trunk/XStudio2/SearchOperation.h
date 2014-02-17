#pragma once

#include "Logic/Common.h"
#include "Logic/SearchWorker.h"
#include "FindProgressDialog.h"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class SearchOperation
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         SearchOperation();
         //SearchOperation(const wstring& term, SearchTarget t) : Target(t), FileSearchData(term, t)
         //{
         //   // Feedback
         //   GuiString msg(L"Searching for '%s' in %s", term.c_str(), GetString(t).c_str() );
         //   Console << Cons::Heading << Cons::Bold << msg << ENDL;
         //   FileSearchData.SendFeedback(ProgressType::Operation, 0, msg);
         //}
         virtual ~SearchOperation();

         DEFAULT_COPY(SearchOperation);	// Default copy semantics
         DEFAULT_MOVE(SearchOperation);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Determines whether operation has been created.</summary>
         /// <returns></returns>
         bool  IsStarted() const
         {
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Creates the find/replace operation.</summary>
         /// <param name="term">Search term.</param>
         /// <param name="t">Target.</param>
         void  Create(const wstring& term, SearchTarget t)
         {
            switch (Target = t)
            {
            // Document/Selection: Examine the active document
            case SearchTarget::Selection:
            case SearchTarget::Document:
               // Ensure document exists
               if (!theApp.GetMainWindow()->GetActiveDocument())
                  throw InvalidOperationException();

               // Add current document
               Documents.push_back((DocumentBase*)theApp.GetMainWindow()->GetActiveDocument());
               break;

            // AllDocuments: Examine open documents
            case SearchTarget::OpenDocuments:
               for (auto doc : theApp)
                  Documents.push_back(&doc);
               break;

            // Project/Scripts: Examine open documents before searching files
            case SearchTarget::ProjectFiles:
            case SearchTarget::ScriptFolder:
               for (auto doc : theApp)
                  Documents.push_back(&doc);

               //FileSearchData = SearchWorkerData(
               break;
            }
         }

         /// <summary>Finds all matches and prints them to the output window</summary>
         void  FindAll()
         {
         }

         /// <summary>Finds the next match and highlights it, opening the document if necessary</summary>
         /// <returns>True if found, False if no more matches</returns>
         bool  FindNext()
         {
            // Search open documents before files
            while (!Documents.empty())
            {
               // Find+Highlight match
               if (Documents.front()->FindNext(FileSearchData.Match))
                  return true;

               // Skip to next document
               Documents.pop_front();
            }

            // Abort if searching document(s) only
            switch (Target)
            {
            case SearchTarget::Selection:
            case SearchTarget::Document:
            case SearchTarget::OpenDocuments:
               return false;
            }

            FindProgressDialog ProgressDlg;

            // Search files for next match
            FileSearch.Start(&FileSearchData);
            ProgressDlg.DoModal(&FileSearch);
            FileSearch.Stop();
            
            // Open+Highlight match
            if (FileSearchData.HasMatch)
            {
               // Open document
               auto doc = (DocumentBase*)theApp.OpenDocumentFile(FileSearchData.Match.FullPath.c_str());
               Documents.push_back(doc);

               // Highlight first match
               FileSearchData.Match.Reset();  // Reset match position due to char indicies being different between same file/document
               return FindNext();
            }

            // No more matches
            return false;
         }

         /// <summary>Replaces the current match, if any, otherwise finds and replaces the next match</summary>
         /// <param name="term">Replacement term.</param>
         /// <returns></returns>
         bool  Replace(const wstring& term)
         {
         }

         /// <summary>Finds and replaces all matches and prints them to the output window.</summary>
         /// <param name="term">The term.</param>
         void  ReplaceAll(const wstring& term)
         {
         }

         /// <summary>Clears the search.</summary>
         void  Reset()
         {
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         DocumentList      Documents;
         SearchWorkerData  FileSearchData;
         SearchWorker      FileSearch;
         SearchTarget      Target;
      };

   }
}

using namespace GUI::Utils;
