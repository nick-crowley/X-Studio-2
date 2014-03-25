#include "stdafx.h"
#include "ProjectDocument.h"
#include "ImportProjectDialog.h"
#include "../Logic/FileIdentifier.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   // ---------------------------------- TEMPLATE ----------------------------------

   
   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------
   
   /// <summary>Creates script document template</summary>
   ProjectDocTemplate::ProjectDocTemplate()
         : CSingleDocTemplate(IDR_PROJECTVIEW, RUNTIME_CLASS(ProjectDocument), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(CView))
   {}


   // ------------------------------- STATIC METHODS -------------------------------
   
   IMPLEMENT_DYNAMIC(ProjectDocTemplate, CSingleDocTemplate)
   
   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Block creation of new frame.</summary>
   /// <param name="pDoc">The document.</param>
   /// <param name="pOther">other.</param>
   /// <returns></returns>
   CFrameWnd*  ProjectDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
   {
      Console << Cons::Error << "Framework attempting to create View for Project document" << ENDL;
      return theApp.GetMainWindow();
   }

   /// <summary>Queries whether an external file should be opened as a script</summary>
   /// <param name="szPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence  ProjectDocTemplate::MatchDocType(LPCTSTR szPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CSingleDocTemplate::MatchDocType(szPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(szPathName) == FileType::Project ? yesAttemptNative : noAttempt;
   }

   /// <summary>Opens the document file.</summary>
   /// <param name="szFullPath">The sz full path.</param>
   /// <param name="bAddToMRU">The b add to MRU.</param>
   /// <param name="bMakeVisible">The b make visible.</param>
   /// <returns></returns>
   CDocument* ProjectDocTemplate::OpenDocumentFile(LPCTSTR szFullPath, BOOL bAddToMRU, BOOL bMakeVisible)
   {
	   ProjectDocument* pDocument = nullptr;

      // Check: Ensure not called by framework in unsupported manner
	   if (!szFullPath)
      {
         AfxMessageBox(L"Invariant violated -- ProjectDocTemplate::OpenDocumentFile() attempting to create new project");
         return nullptr;
      }

      // Existing project: Save/Close
	   if (pDocument = dynamic_cast<ProjectDocument*>(m_pOnlyDoc))
         if (!pDocument->CloseModified())
		      return nullptr; 
	   
      // create a new document    (Sets m_pOnlyDoc)
		pDocument = dynamic_cast<ProjectDocument*>(CreateNewDocument());     
	   if (!pDocument)
	   {
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   return nullptr;
	   }
	   ASSERT(pDocument == m_pOnlyDoc);

      
	   CWaitCursor wait;
      ImportProjectDialog dlg(szFullPath);

		// Preserve 'Modified' flag
		BOOL bWasModified = pDocument->IsModified();
		pDocument->SetModifiedFlag(FALSE);  // not dirty for open

      // Legacy: Upgrade 
      if (IsLegacyProject(szFullPath))
      {
         // IMPORT DOCUMENT
         if (dlg.DoModal() == IDCANCEL || !pDocument->OnImportDocument(szFullPath, dlg.NewPath))
            return nullptr;

         // Open upgraded project, not legacy
         szFullPath = dlg.NewPath.c_str();
         bAddToMRU = TRUE;
      }

      // Set path before open - relied upon for resolving backup paths
      pDocument->SetPathName(szFullPath, bAddToMRU);

      // OPEN DOCUMENT
		if (!pDocument->OnOpenDocument(szFullPath))
		{
			// Failed: Destroy
			delete pDocument;
         m_pOnlyDoc = nullptr;
			return nullptr;
		}

      // Success: Raise 'After Open Document'
		pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);
	   return pDocument;
   }

   
   /// <summary>Opens a new document using a template file.</summary>
   /// <param name="docPath">Initial document path.</param>
   /// <param name="t">template.</param>
   /// <param name="bMakeVisible">make visible.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ApplicationException">Unable to create document or view<exception>
   /// <exception cref="Logic::FileNotFoundException">Template file is missing<exception>
   DocumentBase* ProjectDocTemplate::OpenDocumentTemplate(Path docPath, const TemplateFile& t, BOOL bMakeVisible)
   {
      AppPath templatePath(t.SubPath);
      ProjectDocument* pDocument = nullptr;

      // Verify template file exists
      if (!templatePath.Exists())
         throw FileNotFoundException(HERE, templatePath);

      // Existing document - close/save
	   if (pDocument = dynamic_cast<ProjectDocument*>(m_pOnlyDoc))
         if (!pDocument->CloseModified())
		      return nullptr; 
	   
      // create new doc
		pDocument = dynamic_cast<ProjectDocument*>(CreateNewDocument());     // Sets m_pOnlyDoc
      if (!pDocument)
         throw ApplicationException(HERE, L"Failed to create document class");

      // Use default title
		//SetDefaultTitle(pDocument);

      // Open template file
		if (!pDocument->OnOpenTemplate(docPath, t))
		{
			// Failed: Cleanup
         delete pDocument;
			return nullptr;
		}
      
      // Set user-selected path (+ update title)
      pDocument->SetPathName(docPath.c_str(), FALSE);

      // Raise 'After New Document'
      pDocument->OnDocumentEvent(CDocument::onAfterNewDocument);

      // Ensure *MODIFIED* because file doesn't exist on disc yet
      pDocument->SetModifiedFlag(TRUE);
	   return pDocument;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Determines whether file is a legacy project</summary>
   /// <param name="path">The path.</param>
   /// <returns></returns>
   bool  ProjectDocTemplate::IsLegacyProject(Path path) const
   {
      try
      {
         StreamPtr    s(XFileInfo(path).OpenRead());
         ByteArrayPtr buf(new BYTE[1024]);

         // Peek first 1024 bytes
         DWORD count = s->Read(buf.get(), 1024);
         buf.get()[min(count,1023)] = '\0';

         // Check for 'version=1' attribute
         return StrStrIA((char*)buf.get(), "<project version=\"1\"") != nullptr;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, VString(L"Unable to identify project file '%s'", path.c_str()));
         return false;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

