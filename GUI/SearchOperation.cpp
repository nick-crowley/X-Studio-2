#include "stdafx.h"
#include "SearchOperation.h"
#include "ProgressDialog.h"
#include "MainWnd.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a new search operation</summary>
      /// <param name="output">Output window to use.</param>
      /// <param name="target">Search target.</param>
      /// <param name="search">search text.</param>
      /// <param name="replace">replacement text.</param>
      /// <param name="matchCase">match case.</param>
      /// <param name="matchWord">match whole word.</param>
      /// <param name="regEx">Use regular expressions.</param>
      SearchOperation::SearchOperation(Operation output, SearchTarget target, const wstring& search, const wstring& replace, bool matchCase, bool matchWord, bool regEx)
         : Target(target), 
           Search(output, 
                  target, 
                  MatchData(target, search, replace, matchCase, matchWord, regEx), 
                  PrefsLib.GameDataFolder, 
                  PrefsLib.GameDataVersion)
      {
         // Feedback
         Search.FeedbackStart();

         // Generate documents lists
         switch (Target)
         {
         // Selection: Use active document + set search range
         case SearchTarget::Selection:
            Search.Match.SetRange(DocumentBase::GetActive()->GetSelection());
            // Fall thru...

         // Document: Use active document
         case SearchTarget::Document:
            if (!DocumentBase::GetActive())
               throw InvalidOperationException(HERE, L"Search requires an active document");

            Documents.push_back(DocumentBase::GetActive());
            break;

         // OpenDocuments: Examine open documents 
         case SearchTarget::OpenDocuments:
            if (!DocumentBase::GetActive())
               throw InvalidOperationException(HERE, L"Search requires an active document");
            // Fall thru..

         // Project/ScriptFiles: Also examine open documents
         case SearchTarget::ProjectFiles:
         case SearchTarget::ScriptFolder:
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
         Find(SearchCommand::FindAll);
      }

      /// <summary>Finds the next match and highlights it, opening the document if necessary</summary>
      /// <returns>True if found, False if no more matches</returns>
      bool  SearchOperation::FindNext()
      {
         return Find(SearchCommand::Find);
      }

      /// <summary>Replaces the current match, if any, otherwise finds and replaces the next match</summary>
      /// <param name="term">Replacement term.</param>
      /// <returns></returns>
      bool  SearchOperation::Replace(const wstring& term)
      {
         return Find(SearchCommand::Replace);
      }

      /// <summary>Finds and replaces all matches and prints them to the output window.</summary>
      /// <param name="term">The term.</param>
      void  SearchOperation::ReplaceAll(const wstring& term)
      {
         Find(SearchCommand::ReplaceAll);
      }
   
      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Finds the next matching document/file, or all matches in all documents/files.</summary>
      /// <param name="cmd">The command.</param>
      /// <returns></returns>
      bool  SearchOperation::Find(SearchCommand cmd)
      {
         // Search open documents before files
         while (!Documents.empty())
         {
            auto doc = Documents.front();

            // Document may have been closed since initial call
            if (theApp.IsDocumentOpen(doc))
            {
               // Feedback
               Console << L"Searching document: " << doc->GetFullPath() << ENDL;
               Search.Match.SetPath((LPCWSTR)doc->GetTitle());

               // Replace: Replace current match
               if (cmd == SearchCommand::Replace)
                  doc->Replace(Search.Match);

               // ActiveDocument: Search from caret, not previous match
               UINT start = (doc != DocumentBase::GetActive() || Target == SearchTarget::Selection ? Search.Match.End
                                                                                                   : doc->GetSelection().cpMax);

               // Iterate thru matches
               for (UINT pos = start; doc->FindNext(start, Search.Match); start = Search.Match.End)
               {
                  // Find/Replace: Display document + Highlight match
                  switch (cmd)
                  {
                  case SearchCommand::Find:
                  case SearchCommand::Replace:
                     // Feedback match
                     Console << Cons::Cyan << Cons::Bold << "Found match in " << Search.Match.FullPath << " on line " << Search.Match.LineNumber << ENDL;
                     Search.FeedbackMatch();

                     // Activate document
                     doc->Activate();
                     return true;
                  
                  // Find/ReplaceAll: Feedback [+Replace]
                  case SearchCommand::FindAll:
                  case SearchCommand::ReplaceAll:
                     // Replace
                     if (cmd == SearchCommand::ReplaceAll)
                        doc->Replace(Search.Match);

                     // Feedback replacement
                     Search.FeedbackMatch();
                     break;
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
            Search.FeedbackFinish(false);
            return false;
         }

         ProgressDialog ProgressDlg(L"Find and Replace", L"Finding next match...", true);
         SearchWorker worker;

         // Search files for next match
         worker.Start(cmd, &Search);

         // Display progress dialog until found
         ProgressDlg.DoModal(&worker);
         worker.Close();
            
         // Match:
         if (Search.Match.Matched)
         {
            // Open document
            auto doc = (DocumentBase*)theApp.OpenDocumentFile(Search.Match.FullPath.c_str());
            Documents.push_back(doc);

            // Highlight first match
            Find(cmd);
            return true;
         }

         // No more matches
         Search.FeedbackFinish(Search.IsAborted());
         return false;
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
