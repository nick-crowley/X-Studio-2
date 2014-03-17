#include "stdafx.h"
#include "DocTemplateBase.h"
//#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   // ---------------------------------- TEMPLATE ----------------------------------
   
   IMPLEMENT_DYNAMIC(DocTemplateBase, CMultiDocTemplate)
   
   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------
   
   
   /// <summary>Create document template</summary>
   /// <param name="nIDResource">Template string resource.</param>
   /// <param name="type">Default document type.</param>
   /// <param name="pDocClass">The document class.</param>
   /// <param name="pFrameClass">The frame class.</param>
   /// <param name="pViewClass">The view class.</param>
   DocTemplateBase::DocTemplateBase(UINT nIDResource, FileType type, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
         : DocType(type), CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
   {}


   // ------------------------------- STATIC METHODS -------------------------------
   
   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Queries whether an external file should be opened as a script</summary>
   /// <param name="szPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence  DocTemplateBase::MatchDocType(LPCTSTR szPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = __super::MatchDocType(szPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify type from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(szPathName) == DocType ? yesAttemptNative : noAttempt;
   }

   
   /// <summary>Opens a new document using a template file.</summary>
   /// <param name="t">template.</param>
   /// <param name="bMakeVisible">make visible.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ApplicationException">Unable to create document or view<exception>
   /// <exception cref="Logic::FileNotFoundException">Template file is missing<exception>
   DocumentBase* DocTemplateBase::OpenDocumentTemplate(const FileTemplate& t, BOOL bMakeVisible)
   {
      AppPath path(t.SubPath);

      // Verify template file exists
      if (!path.Exists())
         throw FileNotFoundException(HERE, path);

      // Create document
	   DocumentBase* pDocument = dynamic_cast<DocumentBase*>(CreateNewDocument());
	   if (!pDocument)
         throw ApplicationException(HERE, L"Failed to create document class");

      // don't destroy if something goes wrong
	   BOOL bAutoDelete = pDocument->m_bAutoDelete;
	   pDocument->m_bAutoDelete = FALSE;   

      // Create frame
	   CFrameWnd* pFrame = CreateNewFrame(pDocument, nullptr);
	   pDocument->m_bAutoDelete = bAutoDelete;
	   if (!pFrame)
	   {
		   delete pDocument;       
		   throw ApplicationException(HERE, L"Failed to create frame view class");
	   }
	   ASSERT_VALID(pFrame);

      // Use default title
		SetDefaultTitle(pDocument);

      // Open template file
		if (!pDocument->OnOpenTemplate(t))
		{
			// Failed: Cleanup
			pFrame->DestroyWindow();
			return nullptr;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;

      // Raise 'After New Document'
      pDocument->OnDocumentEvent(CDocument::onAfterNewDocument);

      // Initial Update
	   InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	   return pDocument;
   }


   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

