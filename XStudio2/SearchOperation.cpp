#include "stdafx.h"
#include "SearchOperation.h"
#include "FindProgressDialog.h"
#include "MainWnd.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a new search operation</summary>
      /// <param name="t">Search target.</param>
      /// <param name="search">search text.</param>
      /// <param name="replace">replacement text.</param>
      /// <param name="matchCase">match case.</param>
      /// <param name="matchWord">match whole word.</param>
      /// <param name="regEx">Use regular expressions.</param>
      SearchOperation::SearchOperation(SearchTarget t, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
         : Target(t), 
           Search(t, MatchData(search, replace, matchCase, matchWord, regEx), PrefsLib.GameDataFolder, PrefsLib.GameDataVersion)
      {
         // Feedback
         GuiString msg(L"Searching for '%s' in %s", Search.Match.SearchTerm.c_str(), GetString(Target).c_str() );
         Search.SendFeedback(ProgressType::Operation, 0, msg);

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


      SearchOperation::~SearchOperation()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Finds all matches and prints them to the output window</summary>
      void  SearchOperation::FindAll()
      {
         
      }

      /// <summary>Finds the next match and highlights it, opening the document if necessary</summary>
      /// <returns>True if found, False if no more matches</returns>
      bool  SearchOperation::FindNext()
      {
         // Search open documents before files
         while (!Documents.empty())
         {
            // Find+Highlight match
            if (Documents.front()->FindNext(Search.Match))
               return true;

            // Skip to next document
            Search.Match.Clear();
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
         worker.Start(&Search);
         ProgressDlg.DoModal(&worker);
         worker.Close();
            
         // Open+Highlight match
         if (Search.Match.IsMatched)
         {
            // Open document
            auto doc = (DocumentBase*)theApp.OpenDocumentFile(Search.Match.FullPath.c_str());
            Documents.push_back(doc);

            // Highlight first match
            FindNext();
            return true;
         }

         // No more matches
         OnFinish();
         return false;
      }

      /// <summary>Replaces the current match, if any, otherwise finds and replaces the next match</summary>
      /// <param name="term">Replacement term.</param>
      /// <returns></returns>
      bool  SearchOperation::Replace(const wstring& term)
      {
         return false;
      }

      /// <summary>Finds and replaces all matches and prints them to the output window.</summary>
      /// <param name="term">The term.</param>
      void  SearchOperation::ReplaceAll(const wstring& term)
      {
         
      }
   
      // ------------------------------ PROTECTED METHODS -----------------------------

      void  SearchOperation::OnFinish()
      {
         // Feedback
         Search.SendFeedback(Cons::UserAction, ProgressType::Succcess, 0, L"Search completed");
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
