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
      /// <param name="op">Search operation.</param>
      /// <param name="targ">Search target.</param>
      /// <param name="search">search text.</param>
      /// <param name="replace">replacement text.</param>
      /// <param name="matchCase">match case.</param>
      /// <param name="matchWord">match whole word.</param>
      /// <param name="regEx">Use regular expressions.</param>
      SearchOperation::SearchOperation(Operation op, SearchTarget targ, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
         : Target(targ), 
           Search(op, targ, MatchData(search, replace, matchCase, matchWord, regEx), PrefsLib.GameDataFolder, PrefsLib.GameDataVersion)
      {
         // Feedback
         Search.FeedbackStart();

         // Generate documents lists
         switch (Target)
         {
         // Document/Selection: Examine the active document
         case SearchTarget::Selection:
         case SearchTarget::Document:
            // Ensure document exists
            if (!DocumentBase::GetActive())
               throw InvalidOperationException(HERE, L"Missing active document");

            // Add current document
            Documents.push_back(DocumentBase::GetActive());
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
         Find();
      }

      /// <summary>Finds the next match and highlights it, opening the document if necessary</summary>
      /// <returns>True if found, False if no more matches</returns>
      bool  SearchOperation::FindNext()
      {
         return Find();
      }

      /// <summary>Replaces the current match, if any, otherwise finds and replaces the next match</summary>
      /// <param name="term">Replacement term.</param>
      /// <returns></returns>
      bool  SearchOperation::Replace(const wstring& term)
      {
         return Find();
      }

      /// <summary>Finds and replaces all matches and prints them to the output window.</summary>
      /// <param name="term">The term.</param>
      void  SearchOperation::ReplaceAll(const wstring& term)
      {
         Find();
      }
   
      // ------------------------------ PROTECTED METHODS -----------------------------

      bool  SearchOperation::Find()
      {
         // Search open documents before files
         while (!Documents.empty())
         {
            auto doc = Documents.front();

            // Skip doc if user has since closed it
            if (theApp.IsDocumentOpen(doc))
            {
               // Feedback
               Console << L"Searching document: " << doc->GetFullPath() << ENDL;

               // Replace: Replace current match
               if (Search.Operation == Operation::Replace)
                  doc->Replace(Search.Match);

               // Find matches
               Search.Match.FullPath = (LPCWSTR)doc->GetTitle();
               while (doc->FindNext(Search.Match))
               {
                  // Find/Replace: Display document + Highlight match
                  if (Search.Operation == Operation::Find || Search.Operation == Operation::Replace)
                  {
                     // Feedback 
                     Console << Cons::Cyan << Cons::Bold << "Highlighting Match" << ENDL;
                     Search.FeedbackMatch();

                     // Highlight+Activate
                     doc->SetSelection(Search.Match.Location);
                     doc->Activate();
                     return true;
                  }
                  // FindAll: Feedback
                  else if (Search.Operation == Operation::FindAll)
                     Search.FeedbackMatch();

                  // ReplaceAll: Replace + Feedback
                  else if (Search.Operation == Operation::ReplaceAll)
                  {
                     doc->Replace(Search.Match);
                     Search.FeedbackMatch();
                  }
               }
            }

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
            Search.FeedbackFinish();
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
            Find();
            return true;
         }

         // No more matches
         Search.FeedbackFinish();
         return false;
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
