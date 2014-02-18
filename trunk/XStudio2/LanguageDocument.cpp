// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"
#include "LanguageFrame.h"
#include "Logic/FileStream.h"
#include "Logic/LanguageFileReader.h"
#include "Logic/FileIdentifier.h"
#include "Logic/WorkerFeedback.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(LanguageDocTemplate, CMultiDocTemplate)

   /// <summary>Creates language document template</summary>
   LanguageDocTemplate::LanguageDocTemplate() 
      : CMultiDocTemplate(IDR_LANGUAGEVIEW, RUNTIME_CLASS(LanguageDocument), RUNTIME_CLASS(LanguageFrame), nullptr)
   {}

   /// <summary>Queries whether an external file should be opened as a language file</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if language file, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence LanguageDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CMultiDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return GuiString(L"String Library") == lpszPathName || FileIdentifier::Identify(lpszPathName) == FileType::Language ? yesAttemptNative : noAttempt;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageDocument, DocumentBase)
   
   BEGIN_MESSAGE_MAP(LanguageDocument, DocumentBase)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageDocument::LanguageDocument()
   {
      Type = DocumentType::Language;
   }

   LanguageDocument::~LanguageDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   #ifdef _DEBUG
   void LanguageDocument::AssertValid() const
   {
	   DocumentBase::AssertValid();
   }

   
   void LanguageDocument::Dump(CDumpContext& dc) const
   {
	   DocumentBase::Dump(dc);
   }
   #endif //_DEBUG
   
   void LanguageDocument::Serialize(CArchive& ar)
   {
	   if (ar.IsStoring())
	   {
		   // TODO: add storing code here
	   }
	   else
	   {
		   // TODO: add loading code here
	   }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   BOOL LanguageDocument::OnNewDocument()
   {
	   if (!DocumentBase::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   BOOL LanguageDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << L"Loading language file: " << Path(szPathName) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading language file '%s'", szPathName));

         // Parse file
         if (GuiString(L"String Library") == szPathName)
            Virtual = true;
         else
         {
            StreamPtr fs2( new FileStream(szPathName, FileMode::OpenExisting, FileAccess::Read) );
            Content = LanguageFileReader(fs2).ReadFile(Path(szPathName).FileName);
            Virtual = false;
         }

         data.SendFeedback(Cons::Green, ProgressType::Succcess, 0, L"Language file loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to load language file");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load language file '%s'", szPathName));
         return FALSE;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END2(GUI,Documents)

