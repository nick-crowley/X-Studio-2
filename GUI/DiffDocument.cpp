// DiffDocument.cpp : implementation file
//

#include "stdafx.h"
#include "DiffDocument.h"
#include "DiffFrame.h"
#include "DiffView.h"
#include "PropertiesWnd.h"
#include "../Logic/WorkerData.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(DiffDocTemplate, CMultiDocTemplate)

   /// <summary>Creates diff document template</summary>
   DiffDocTemplate::DiffDocTemplate() 
      : CMultiDocTemplate(IDR_DIFFVIEW, RUNTIME_CLASS(DiffDocument), RUNTIME_CLASS(DiffFrame), nullptr)
   {}

   /// <summary>Matches open documents only</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, otherwise noAttempt</returns>
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

   /// <summary>Creates and opens a diff document.</summary>
   /// <param name="doc">Script document.</param>
   /// <param name="alternate">Text to perform diff against.</param>
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
      pDocument->GetView(0)->SetType(DiffViewType::Original);
      pDocument->GetView(1)->SetType(DiffViewType::Alternate);

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
		pDocument->SetPathName(VString(L"[Diff] %s", (LPCWSTR)doc.GetTitle()).c_str(), FALSE);
		pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);

      // Update frame
	   InitialUpdateFrame(pFrame, pDocument, TRUE);
	   return pDocument;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DiffDocument, DocumentBase)
   
   BEGIN_MESSAGE_MAP(DiffDocument, DocumentBase)
      ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &DiffDocument::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &DiffDocument::OnQueryCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   DiffDocument::DiffDocument() : DocumentBase(DocumentType::Diff, true)
   {
   }

   DiffDocument::~DiffDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Toggle a diff view type</summary>
   DiffViewType operator!(const DiffViewType& t)
   {
      return t == DiffViewType::Original ? DiffViewType::Alternate : DiffViewType::Original;
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Gets either diff view</summary>
   /// <param name="index">View index.</param>
   /// <returns></returns>
   /// <exception cref="Logic::AlgorithmException">View not found</exception>
   DiffView*  DiffDocument::GetView(UINT index) const
   {
      POSITION pos = GetFirstViewPosition();
      UINT i = 0;

      // Iterate thru views
      while (DiffView* v = dynamic_cast<DiffView*>(GetNextView(pos)))
         if (index == i++)
            return v;

      // Error: Not found
      throw AlgorithmException(HERE, L"Cannot find desired View");
   }
   
   /// <summary>Gets either diff view</summary>
   /// <param name="t">View type.</param>
   /// <returns></returns>
   /// <exception cref="Logic::AlgorithmException">View not found</exception>
   DiffView*  DiffDocument::GetView(DiffViewType t) const
   {
      return GetView((UINT)t);
   }

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
   
   /// <summary>No longer used</summary>
   /// <param name="deEvent">The de event.</param>
   void DiffDocument::OnDocumentEvent(DocumentEvent deEvent)
   {
   }

   /// <summary>Populates from source document and alternate text</summary>
   /// <param name="doc"></param>
   /// <param name="alternate"></param>
   /// <returns></returns>
   BOOL DiffDocument::OnOpenDocument(ScriptDocument& doc, const wstring& alternate)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << L"Loading diff for: " << doc.FullPath << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Loading diff for '%s'", doc.FullPath.FileName.c_str()));

         // Store document
         Source = &doc;

         // Perform DIFF
         dtl::Diff<wchar, wstring> d(doc.GetAllText(), ConvertLineBreaks(alternate));
         d.compose();

         // Generate phrases
         GeneratePhrases(d.getSes());

         // Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Diff document loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to load diff document");
         theApp.ShowError(HERE, e, VString(L"Failed to load diff for '%s'", doc.FullPath.FileName.c_str()));
         return FALSE;
      }
   }

   /// <summary>Do nothing</summary>
   /// <param name="szPathName">Full path</param>
   /// <returns></returns>
   BOOL DiffDocument::OnSaveDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback/Display error
      data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Cannot to save difference document");
      //theApp.ShowError(HERE, e, VString(L"Cannot to save difference document '%s'", szPathName));
      return FALSE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Converts whatever line breaks the input uses into RichEdit compatible '\v' linebreaks.</summary>
   /// <param name="str">The string.</param>
   /// <returns>Lines delimited by single char vertical tabs '\v'</returns>
   wstring DiffDocument::ConvertLineBreaks(wstring str)
   {
      // Harmonize line-breaks
      for (auto ch = str.begin(), end = str.end(); ch != end; )
      {
         // Drop '\r'
         if (*ch == '\r')
         {
            str.erase(ch++);
            continue;
         }
         // Convert '\n' to '\v'
         else if (*ch == '\n')
            *ch = '\v';
         
         ++ch;
      }

      return str;
   }

   /// <summary>Generates the phrases.</summary>
   /// <param name="sequence">The sequence.</param>
   void  DiffDocument::GeneratePhrases(dtl::Ses<wchar>& ses)
   {
      DiffPhrase* phrase = nullptr;
      UINT index = 0;

      // Generate text
      for (auto s : ses.getSequence())
      {
         wchar       chr = s.first;
         dtl::edit_t type = s.second.type;

         // Common/NewLine: Insert both
         if (chr == '\v' || type == dtl::SES_COMMON)
         {
            Original.push_back(chr);
            Alternate.push_back(chr);
            phrase = nullptr;
         }
         // Added/Removed: 
         else 
         {
            // Start new phrase
            if (!phrase || phrase->Type != type)
            {
               Phrases.push_back(DiffPhrase(type, index));
               phrase = &Phrases.back();
            }
            // Append phrase
            phrase->Text += chr;

            // Add: Insert placeholder into original
            if (type == dtl::SES_ADD)
            {
               Original.push_back(' ');
               Alternate.push_back(chr);
            }
            else
            {  // Removed: Insert placeholder into alternate
               Original.push_back(chr);
               Alternate.push_back(' ');
            }
         }

         // Advance
         ++index;
      }
   }

   /// <summary>Blocks undo/redo</summary>
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
      // Save/SaveAs: Not supported
      case ID_FILE_SAVE:
      case ID_FILE_SAVE_AS:
         break;

      // Undo: Not supported
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


