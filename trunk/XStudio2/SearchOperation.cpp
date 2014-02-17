#include "stdafx.h"
#include "SearchOperation.h"
#include "FindProgressDialog.h"
#include "MainWnd.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      SearchOperation::SearchOperation()
      {
      }


      SearchOperation::~SearchOperation()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Creates the find/replace operation.</summary>
      /// <param name="term">Search term.</param>
      /// <param name="t">Target.</param>
      void  SearchOperation::Create(SearchTarget t, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
      {
         // Init search data
         Target = t;
         Search = SearchData(new SearchWorkerData(Target, MatchData(search, replace, matchCase, matchWord, regEx), 
                                                  L"D:\\X3 Albion Prelude", GameVersion::TerranConflict));

         // Feedback
         OnStart();

         // Generate documents lists
         switch (Target)
         {
         // Document/Selection: Examine the active document
         case SearchTarget::Selection:
         case SearchTarget::Document:
            // Ensure document exists
            if (!theApp.GetMainWindow()->GetActiveDocument())
               throw InvalidOperationException(HERE, L"Missing active document");

            // Add current document
            Documents.push_back((DocumentBase*)theApp.GetMainWindow()->GetActiveDocument());
            break;

         // Remainder: Examine open documents 
         case SearchTarget::ProjectFiles:
         case SearchTarget::ScriptFolder:
         case SearchTarget::OpenDocuments:
            for (auto& doc : theApp)
               Documents.push_back(&doc);
            break;
         }
      }

      /// <summary>Finds all matches and prints them to the output window</summary>
      void  SearchOperation::FindAll()
      {
         // Ensure started
         if (!IsStarted())
            throw InvalidOperationException(HERE, L"Search must be initialized first");
      }

      /// <summary>Finds the next match and highlights it, opening the document if necessary</summary>
      /// <returns>True if found, False if no more matches</returns>
      bool  SearchOperation::FindNext()
      {
         // Ensure started
         if (!IsStarted())
            throw InvalidOperationException(HERE, L"Search must be initialized first");

         // Search open documents before files
         while (!Documents.empty())
         {
            // Find+Highlight match
            if (Documents.front()->FindNext(Search->Match))
               return true;

            // Skip to next document
            Search->Match.Clear();
            Documents.pop_front();
         }

         // Abort if searching document(s) only
         switch (Target)
         {
         case SearchTarget::Selection:
         case SearchTarget::Document:
         case SearchTarget::OpenDocuments:
            OnFinish();
            return false;
         }

         FindProgressDialog ProgressDlg;
         SearchWorker worker;

         // Search files for next match
         worker.Start(Search.get());
         ProgressDlg.DoModal(&worker);
         worker.Stop();
            
         // Open+Highlight match
         if (Search->Match.IsMatched)
         {
            // Open document
            auto doc = (DocumentBase*)theApp.OpenDocumentFile(Search->Match.FullPath.c_str());
            Documents.push_back(doc);

            // Highlight first match
            FindNext();
            return true;
         }

         // No more matches
         OnFinish();
         return false;
      }

      /// <summary>Determines whether operation has been created.</summary>
      /// <returns></returns>
      bool  SearchOperation::IsStarted() const
      {
         return Search != nullptr;
      }

      /// <summary>Replaces the current match, if any, otherwise finds and replaces the next match</summary>
      /// <param name="term">Replacement term.</param>
      /// <returns></returns>
      bool  SearchOperation::Replace(const wstring& term)
      {
         // Ensure started
         if (!IsStarted())
            throw InvalidOperationException(HERE, L"Search must be initialized first");

         return false;
      }

      /// <summary>Finds and replaces all matches and prints them to the output window.</summary>
      /// <param name="term">The term.</param>
      void  SearchOperation::ReplaceAll(const wstring& term)
      {
         // Ensure started
         if (!IsStarted())
            throw InvalidOperationException(HERE, L"Search must be initialized first");
      }
   
      /// <summary>Clears the search.</summary>
      void  SearchOperation::Reset()
      {
         Documents.clear();
         Search.reset(nullptr);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      void  SearchOperation::OnStart()
      {
         // Feedback
         GuiString msg(L"Searching for '%s' in %s", Search->Match.SearchTerm.c_str(), GetString(Target).c_str() );
         Console << Cons::Heading << Cons::Bold << msg << ENDL;
         Search->SendFeedback(ProgressType::Operation, 0, msg);
      }

      void  SearchOperation::OnFinish()
      {
         // Feedback
         Console << Cons::Heading << "Search completed" << ENDL;
         Search->SendFeedback(ProgressType::Succcess, 0, L"Search completed");
         
         // Destroy search
         Search.reset(nullptr);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
