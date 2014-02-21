#include "stdafx.h"
#include "ProjectDocument.h"
#include "Logic/FileIdentifier.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(ProjectDocTemplate, CSingleDocTemplate)
   
   /// <summary>Creates script document template</summary>
   ProjectDocTemplate::ProjectDocTemplate()
         : CSingleDocTemplate(IDR_PROJECTVIEW, RUNTIME_CLASS(ProjectDocument), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CView))
   {}

   /// <summary>Queries whether an external file should be opened as a script</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence  ProjectDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CSingleDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(lpszPathName) == FileType::Project ? yesAttemptNative : noAttempt;
   }

   CFrameWnd*  ProjectDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) 
   {
      return theApp.GetMainWindow();
   }

   CDocument* ProjectDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible)
{
	CDocument* pDocument = NULL;
	BOOL bCreated = FALSE;      // => doc and frame created
	BOOL bWasModified = FALSE;

	if (m_pOnlyDoc != NULL)
	{
		// already have a document - reinit it
		pDocument = m_pOnlyDoc;
		if (!pDocument->SaveModified())
		{
			// set a flag to indicate that the document being opened should not
			// be removed from the MRU list, if it was being opened from there
			//g_bRemoveFromMRU = FALSE;
			return NULL;        // leave the original one
		}

		/*pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT(pFrame != NULL);
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);*/
	}
	else
	{
		// create a new document
		pDocument = CreateNewDocument();
		//ASSERT(pFrame == NULL);     // will be created below
		bCreated = TRUE;
	}

	if (pDocument == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT(pDocument == m_pOnlyDoc);

	//if (pFrame == NULL)
	//{
	//	ASSERT(bCreated);

	//	// create frame - set as main document frame
	//	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	//	pDocument->m_bAutoDelete = FALSE;
	//				// don't destroy if something goes wrong
	//	pFrame = CreateNewFrame(pDocument, NULL);
	//	pDocument->m_bAutoDelete = bAutoDelete;
	//	if (pFrame == NULL)
	//	{
	//		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
	//		delete pDocument;       // explicit delete on error
	//		return NULL;
	//	}
	//}

	if (lpszPathName == NULL)
	{
		// create a new document
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;

		if (!pDocument->OnNewDocument())
		{
			// user has been alerted to what failed in OnNewDocument
			TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
			if (bCreated)
         {
            delete pDocument;
            m_pOnlyDoc = NULL;
            //pFrame->DestroyWindow();    // will destroy document
         }
				
			return NULL;
		}
	}
	else
	{
		CWaitCursor wait;

		// open an existing document
		bWasModified = pDocument->IsModified();
		pDocument->SetModifiedFlag(FALSE);  // not dirty for open

		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has been alerted to what failed in OnOpenDocument
			TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
			if (bCreated)
			{
            delete pDocument;
            m_pOnlyDoc = NULL;
				//pFrame->DestroyWindow();    // will destroy document
			}
			else if (!pDocument->IsModified())
			{
				// original document is untouched
				pDocument->SetModifiedFlag(bWasModified);
			}
			else
			{
				// we corrupted the original document
				SetDefaultTitle(pDocument);

				if (!pDocument->OnNewDocument())
				{
					TRACE(traceAppMsg, 0, "Error: OnNewDocument failed after trying to open a document - trying to continue.\n");
					// assume we can continue
				}
			}
			return NULL;        // open failed
		}
		pDocument->SetPathName(lpszPathName, bAddToMRU);
		pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);
	}

	//CWinThread* pThread = AfxGetThread();
	//ASSERT(pThread);
	//if (bCreated && pThread->m_pMainWnd == NULL)
	//{
	//	// set as main frame (InitialUpdateFrame will show the window)
	//	pThread->m_pMainWnd = pFrame;
	//}
	//InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	return pDocument;
}

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ProjectDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ProjectDocument, DocumentBase)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectDocument::ProjectDocument()
   {
   }

   ProjectDocument::~ProjectDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------


   // ------------------------------- PUBLIC METHODS -------------------------------

   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
