// DiffDocument.cpp : implementation file
//

#include "stdafx.h"
#include "DiffDocument.h"
#include "DiffFrame.h"
#include "DiffView.h"
#include "PropertiesWnd.h"
#include "Logic/WorkerFeedback.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(DiffDocTemplate, CMultiDocTemplate)

   /// <summary>Creates diff document template</summary>
   DiffDocTemplate::DiffDocTemplate() 
      : CMultiDocTemplate(IDR_LANGUAGEVIEW, RUNTIME_CLASS(DiffDocument), RUNTIME_CLASS(DiffFrame), nullptr)
   {}

   /// <summary>Queries whether an external file should be opened as a language file</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if language file, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence DiffDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CMultiDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Do not open
      rpDocMatch = nullptr;
      return noAttempt;
   }

   /// <summary>Opens the document file.</summary>
   /// <param name="doc">Script document.</param>
   /// <param name="alternate">Alternate text.</param>
   /// <returns></returns>
   DiffDocument* DiffDocTemplate::OpenDocumentFile(ScriptDocument& doc, const wstring& alternate)
   {
      // Create document
      DiffDocument* pDocument = dynamic_cast<DiffDocument*>(CreateNewDocument());
	   if (!pDocument)
	   {
		   TRACE(traceAppMsg, 0, "CDocTemplate::CreateNewDocument returned NULL.\n");
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   return NULL;
	   }
	   ASSERT_VALID(pDocument);

      // don't destroy if something goes wrong
	   BOOL bAutoDelete = pDocument->m_bAutoDelete;
	   pDocument->m_bAutoDelete = FALSE;   

      // Create frame
	   CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	   pDocument->m_bAutoDelete = bAutoDelete;
	   if (!pFrame)
	   {
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   delete pDocument;       // explicit delete on error
		   return NULL;
	   }
	   ASSERT_VALID(pFrame);

      // Identify views
      auto pos = pDocument->GetFirstViewPosition();
      reinterpret_cast<DiffView*>(pDocument->GetNextView(pos))->SetType(DiffViewType::Original);
      reinterpret_cast<DiffView*>(pDocument->GetNextView(pos))->SetType(DiffViewType::Alternate);

	   // open document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(doc, alternate))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}

      // Set title
		pDocument->SetPathName(GuiString(L"[Diff] %s", (LPCWSTR)doc.GetTitle()).c_str(), FALSE);
		pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);

      // Update frame
	   InitialUpdateFrame(pFrame, pDocument, TRUE);
	   return pDocument;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DiffDocument, DocumentBase)
   
   BEGIN_MESSAGE_MAP(DiffDocument, DocumentBase)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   DiffDocument::DiffDocument() : DocumentBase(DocumentType::Diff)
   {
   }

   DiffDocument::~DiffDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Populates the properties window</summary>
   /// <param name="grid">The grid.</param>
   void  DiffDocument::OnDisplayProperties(CMFCPropertyGridCtrl& grid)
   {
      
   }
   
   /// <summary>Called when document closed.</summary>
   void DiffDocument::OnCloseDocument()
   {
      // Disconnect properties
      CPropertiesWnd::Connect(this, false);

      // Close
      __super::OnCloseDocument();
   }
   
   /// <summary>Manually set the path of the string library document</summary>
   /// <param name="deEvent">The de event.</param>
   void DiffDocument::OnDocumentEvent(DocumentEvent deEvent)
   {
      // Manually set 'String Library' path
      /*if (Virtual && deEvent == onAfterOpenDocument)
         m_strPathName = L"Diff";*/
   }

   /// <summary>Populates from a script document</summary>
   /// <param name="doc"></param>
   /// <param name="alternate"></param>
   /// <returns></returns>
   BOOL DiffDocument::OnOpenDocument(ScriptDocument& doc, const wstring& alternate)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << L"Loading diff: " << doc.FullPath << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading diff '%s'", (LPCWSTR)doc.GetTitle()));

         // Store document
         Source = &doc;

         // Perform DIFF
         dtl::Diff<wchar, wstring> d(doc.GetAllText(), alternate);
         d.compose();

         // Generate text
         auto seq = d.getSes().getSequence();
         for (auto pos = seq.begin(), end = seq.end(); pos != end; ++pos)
         {
            wchar&      chr = pos->first;
            dtl::edit_t type = pos->second.type;

            // Common/NewLine: Insert both
            if (chr == '\v' || type == dtl::SES_COMMON)
            {
               Original.push_back(chr);
               Alternate.push_back(chr);
            }
            // Added/Removed: Generate+consume phrase
            else 
            {
               wstring text;
               int start = (int)pos->second.beforeIdx;

               // Consume chars of matching type
               for (auto p = pos; p != end && p->second.type == type; ++p)
               {
                  wchar chr = p->first;

                  // Insert text/placeholders
                  Original.push_back(type == dtl::SES_ADD && chr != '\v' ? '+' : chr);
                  Alternate.push_back(type == dtl::SES_DELETE && chr != '\v' ? '?' : chr);
                  
                  // Append phrase
                  text.push_back(chr);
                  pos = p;
               }

               // Create new phrase
               Phrases.push_back(DiffPhrase(type, start, (int)pos->second.beforeIdx, text));
            }
         }

         // Generate text
         //for (auto s : d.getSes().getSequence())
         //{
         //   // Common/NewLine: Insert both
         //   if (s.first == '\v' || s.second.type == dtl::SES_COMMON)
         //   {
         //      Original.push_back(s.first);
         //      Alternate.push_back(s.first);
         //   }
         //   // Added: 
         //   else if (s.second.type == dtl::SES_ADD)
         //   {
         //      Original.push_back('+');
         //      Alternate.push_back(s.first);
         //   }
         //   // Deleted:
         //   else if (s.second.type == dtl::SES_DELETE)
         //   {
         //      Original.push_back(s.first);
         //      Alternate.push_back('?');
         //   }
         //}

         // Feedback
         data.SendFeedback(Cons::Green, ProgressType::Succcess, 0, L"Language file loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to load diff");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load diff '%s'", (LPCWSTR)doc.GetTitle()));
         return FALSE;
      }
   }

   /// <summary>Saves contents to disc</summary>
   /// <param name="szPathName">Full path</param>
   /// <returns></returns>
   BOOL DiffDocument::OnSaveDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback/Display error
      data.SendFeedback(ProgressType::Failure, 0, L"Cannot to save difference document");
      //theApp.ShowError(HERE, e, GuiString(L"Cannot to save difference document '%s'", szPathName));
      return FALSE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Performs a menu command</summary>
   /// <param name="nID">Command identifier.</param>
   void DiffDocument::OnPerformCommand(UINT nID)
   {
      //AfxMessageBox(L"DiffDocument::OnPerformCommand");

      try 
      {
         switch (nID)
         {
         // Undo/Redo
         case ID_EDIT_UNDO:  
         case ID_EDIT_REDO:  
            break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }
   

   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void DiffDocument::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      switch (pCmdUI->m_nID)
      {
      // Save/SaveAs: Require file document
      case ID_FILE_SAVE:
      case ID_FILE_SAVE_AS:
         break;

      // Undo: 
      case ID_EDIT_UNDO:   
      case ID_EDIT_REDO:   
         break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END2(GUI,Documents)


