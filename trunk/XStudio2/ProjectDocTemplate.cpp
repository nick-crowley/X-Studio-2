#include "stdafx.h"
#include "ProjectDocument.h"
#include "ImportProjectDialog.h"
#include "Logic/FileIdentifier.h"
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
      bool Created = false;

      // already have a document - reinit it
	   if (pDocument = dynamic_cast<ProjectDocument*>(m_pOnlyDoc))
	   {
		   // Query to save 
		   if (!pDocument->SaveModified())
		      return nullptr; 
	   }
	   else 
      {
	      // create a new document
		   pDocument = dynamic_cast<ProjectDocument*>(CreateNewDocument());     // Sets m_pOnlyDoc
         Created = true;
      }

      // Verify created
	   if (!pDocument)
	   {
		   AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		   return nullptr;
	   }
	   ASSERT(pDocument == m_pOnlyDoc);

      // New Document:
	   if (!szFullPath)
	   {
		   SetDefaultTitle(pDocument);

		   // avoid creating temporary compound file when starting up invisible
		   if (!bMakeVisible)
			   pDocument->m_bEmbedded = TRUE;

         // NEW DOCUMENT:
		   if (!pDocument->OnNewDocument())
		   {
			   // Cleanup
            if (Created)
            {
               delete pDocument;
               m_pOnlyDoc = nullptr;
            }
			   return nullptr;
		   }
	   }
	   else
	   {
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

         // OPEN DOCUMENT
		   if (!pDocument->OnOpenDocument(szFullPath))
		   {
			   // Failed: Destroy
			   if (Created)
			   {
               delete pDocument;
               m_pOnlyDoc = nullptr;
			   }
            // Failed (but Unmodified): Restore 'modified' flag
			   else if (!pDocument->IsModified())
			      pDocument->SetModifiedFlag(bWasModified);
			   else
			   {
				   // Failed (and Modified): Document is now corrupt. Start new document.
				   SetDefaultTitle(pDocument);

               // NEW DOCUMENT:
				   pDocument->OnNewDocument();
			   }
			   return nullptr;        // open failed
		   }

         //
		   pDocument->SetPathName(szFullPath, bAddToMRU);
		   pDocument->OnDocumentEvent(CDocument::onAfterOpenDocument);
	   }

	   return pDocument;
   }

   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Determines whether file is a legacy project</summary>
   /// <param name="path">The path.</param>
   /// <returns></returns>
   bool  ProjectDocTemplate::IsLegacyProject(IO::Path path) const
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
         Console.Log(HERE, e, GuiString(L"Unable to identify project file '%s'", path.c_str()));
         return false;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

