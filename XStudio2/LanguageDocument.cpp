// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"
#include "LanguageFrame.h"
#include "Logic/FileStream.h"
#include "Logic/LanguageFileReader.h"
#include "Logic/FileIdentifier.h"

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
  
   IMPLEMENT_DYNCREATE(LanguageDocument, CDocument)
   
   BEGIN_MESSAGE_MAP(LanguageDocument, CDocument)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageDocument::LanguageDocument()
   {
   }

   LanguageDocument::~LanguageDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   #ifdef _DEBUG
   void LanguageDocument::AssertValid() const
   {
	   CDocument::AssertValid();
   }

   
   void LanguageDocument::Dump(CDumpContext& dc) const
   {
	   CDocument::Dump(dc);
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
	   if (!CDocument::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   BOOL LanguageDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      try
      {
         Console << Cons::Heading << L"Parsing language file: " << szPathName << ENDL;

         // Parse file
         if (GuiString(L"String Library") == szPathName)
            Virtual = true;
         else
         {
            StreamPtr fs2( new FileStream(szPathName, FileMode::OpenExisting, FileAccess::Read) );
            Content = LanguageFileReader(fs2).ReadFile(Path(szPathName).FileName);
            Virtual = false;
         }

         Console << Colour::Green << L"Language file loaded successfully" << ENDL;
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", szPathName, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
         return FALSE;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END2(GUI,Documents)

